#include "RAT.h"
#include "turbojpeg.h"

#include "Input_Lite.h"
#include "Logging.h"
#include "ScreenCapture.h"
#include "WS_Lite.h"

#include <assert.h>
#include <mutex>

namespace SL {
namespace RAT_Lite {

    class ClientDriver : public IClientDriver {

        Point LastMousePosition_;
        std::shared_ptr<WS_LITE::IWSocket> Socket_;

        std::mutex outputbufferLock;
        std::vector<unsigned char> outputbuffer;
        std::vector<Screen_Capture::Monitor> Monitors;

        void MouseImageChanged(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {
            if (!onMouseImageChanged)
                return;
            if (len >= sizeof(Rect)) {
                Image img(*reinterpret_cast<const Rect *>(data), data + sizeof(Rect), len - sizeof(Rect));
                if (len >= sizeof(Rect) + (img.Rect_.Width * img.Rect_.Height * PixelStride)) {
                    return onMouseImageChanged(img);
                }
            }
            socket->close(1000, "Received invalid lenght on onMouseImageChanged");
        }
        void MousePositionChanged(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {
            if (!onMousePositionChanged)
                return;
            if (len == sizeof(SL::Screen_Capture::Point)) {
                return onMousePositionChanged(*reinterpret_cast<const RAT_Lite::Point *>(data));
            }
            socket->close(1000, "Received invalid lenght on onMousePositionChanged");
        }

        void MonitorsChanged(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {
            if (!onMonitorsChanged)
                return;
            auto num = len / sizeof(Screen_Capture::Monitor);
            Monitors.clear();
            if (len == num * sizeof(Screen_Capture::Monitor) && num < 8) {
                for (decltype(num) i = 0; i < num; i++) {
                    Monitors.push_back(*(Screen_Capture::Monitor *)data);
                    data += sizeof(Screen_Capture::Monitor);
                }
                return onMonitorsChanged(Monitors);
            }
            else if (len == 0) {
                // it is possible to have no monitors.. shouldnt disconnect in that case
                return onMonitorsChanged(Monitors);
            }
            socket->close(1000, "Invalid Monitor Count");
        }
        void ClipboardTextChanged(const unsigned char *data, size_t len)
        {
            if (ShareClip == ClipboardSharing::NOT_SHARED || !onClipboardChanged)
                return;
            if (len < 1024 * 100) { // 100K max
                std::string str(reinterpret_cast<const char *>(data), len);
                onClipboardChanged(str);
            }
        }
        template <typename CALLBACKTYPE> void Frame(const unsigned char *data, size_t len, CALLBACKTYPE &&cb)
        {
            int monitor_id = 0;
            if (len < sizeof(Rect) + sizeof(monitor_id)) {
                return Socket_->close(1000, "Invalid length on onFrameChanged");
            }

            auto jpegDecompressor = tjInitDecompress();
            int jpegSubsamp(0), outwidth(0), outheight(0);

            auto src = (unsigned char *)data;
            memcpy(&monitor_id, src, sizeof(monitor_id));
            src += sizeof(monitor_id);
            Rect rect;
            memcpy(&rect, src, sizeof(rect));
            src += sizeof(rect);

            auto monitor = std::find_if(begin(Monitors), end(Monitors), [monitor_id](const auto &m) { return m.Id == monitor_id; });
            if (monitor == end(Monitors)) {
                SL_RAT_LOG(RAT_Lite::Logging_Levels::INFO_log_level, "Monitor Id doesnt exist!");
                return;
            }
            len -= sizeof(Rect) + sizeof(monitor_id);

            if (tjDecompressHeader2(jpegDecompressor, src, static_cast<unsigned long>(len), &outwidth, &outheight, &jpegSubsamp) == -1) {
                SL_RAT_LOG(RAT_Lite::Logging_Levels::ERROR_log_level, tjGetErrorStr());
            }
            std::lock_guard<std::mutex> lock(outputbufferLock);
            outputbuffer.reserve(outwidth * outheight * PixelStride);

            if (tjDecompress2(jpegDecompressor, src, static_cast<unsigned long>(len), (unsigned char *)outputbuffer.data(), outwidth, 0, outheight,
                              TJPF_RGBX, 2048 | TJFLAG_NOREALLOC) == -1) {
                SL_RAT_LOG(RAT_Lite::Logging_Levels::ERROR_log_level, tjGetErrorStr());
            }
            Image img(rect, outputbuffer.data(), outwidth * outheight * PixelStride);

            assert(outwidth == img.Rect_.Width && outheight == img.Rect_.Height);
            cb(img, *monitor);
            tjDestroy(jpegDecompressor);
        }

      public:
        std::function<void(const std::vector<Screen_Capture::Monitor> &)> onMonitorsChanged;
        std::function<void(const Image &, const SL::Screen_Capture::Monitor &)> onFrameChanged;
        std::function<void(const Image &, const SL::Screen_Capture::Monitor &)> onNewFrame;
        std::function<void(const Image &)> onMouseImageChanged;
        std::function<void(const Point &)> onMousePositionChanged;
        std::function<void(const std::string &)> onClipboardChanged;
        std::function<void(const std::shared_ptr<SL::WS_LITE::IWSocket>)> onConnection;
        std::function<void(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const WS_LITE::WSMessage)> onMessage;
        std::function<void(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, unsigned short code, const std::string)> onDisconnection;
        ClipboardSharing ShareClip = ClipboardSharing::NOT_SHARED;

        ClientDriver() {}
        virtual ~ClientDriver() {}
        void Build(const std::shared_ptr<SL::WS_LITE::IWSClient_Configuration> &wsclientconfig)
        {

            wsclientconfig
                ->onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const std::unordered_map<std::string, std::string> &header) {
                    SL_RAT_LOG(RAT_Lite::Logging_Levels::INFO_log_level, "onConnection ");
                    Socket_ = socket;
                    if (onConnection)
                        onConnection(socket);
                    UNUSED(header);
                })
                ->onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, unsigned short code, const std::string &msg) {
                    SL_RAT_LOG(RAT_Lite::Logging_Levels::INFO_log_level, "onDisconnection ");
                    Socket_.reset();
                    if (onDisconnection)
                        onDisconnection(socket, code, msg);
                })
                ->onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const SL::WS_LITE::WSMessage &message) {
                    auto p = PACKET_TYPES::INVALID;
                    assert(message.len >= sizeof(p));

                    p = *reinterpret_cast<const PACKET_TYPES *>(message.data);
                    const auto datastart = message.data + sizeof(p);
                    size_t datasize = message.len - sizeof(p);
                    switch (p) {
                    case PACKET_TYPES::ONMONITORSCHANGED:
                        MonitorsChanged(socket, datastart, datasize);
                        break;
                    case PACKET_TYPES::ONFRAMECHANGED:
                        if (onFrameChanged) {
                            Frame(datastart, datasize, [&](const auto &img, const auto &monitor) { onFrameChanged(img, monitor); });
                        }
                        break;
                    case PACKET_TYPES::ONNEWFRAME:
                        if (onNewFrame) {
                            Frame(datastart, datasize, [&](const auto &img, const auto &monitor) { onNewFrame(img, monitor); });
                        }
                        break;
                    case PACKET_TYPES::ONMOUSEIMAGECHANGED:
                        MouseImageChanged(socket, datastart, datasize);
                        break;
                    case PACKET_TYPES::ONMOUSEPOSITIONCHANGED:
                        MousePositionChanged(socket, datastart, datasize);
                        break;
                    case PACKET_TYPES::ONCLIPBOARDTEXTCHANGED:
                        ClipboardTextChanged(datastart, datasize);
                        break;
                    default:
                        if (onMessage)
                            onMessage(socket, message); // pass up the chain
                        break;
                    }

                });
        }
        virtual void ShareClipboard(ClipboardSharing share) override { ShareClip = share; }
        virtual ClipboardSharing ShareClipboard() const override { return ShareClip; }
        template <typename STRUCT> void SendStruct_Impl(STRUCT key, PACKET_TYPES ptype)
        {
            if (!Socket_) {
                SL_RAT_LOG(RAT_Lite::Logging_Levels::INFO_log_level, "SendKey called on a socket that is not open yet");
                return;
            }
            const auto size = sizeof(ptype) + sizeof(key);
            auto ptr(std::shared_ptr<unsigned char>(new unsigned char[size], [](auto *p) { delete[] p; }));
            *reinterpret_cast<PACKET_TYPES *>(ptr.get()) = ptype;
            memcpy(ptr.get() + sizeof(ptype), &key, sizeof(key));

            SL::WS_LITE::WSMessage buf;
            buf.code = WS_LITE::OpCode::BINARY;
            buf.Buffer = ptr;
            buf.len = size;
            buf.data = ptr.get();
            Socket_->send(buf, SL::WS_LITE::CompressionOptions::NO_COMPRESSION);
        }
        virtual void SendClipboardChanged(const std::string &text) override
        {
            if (!Socket_) {
                SL_RAT_LOG(RAT_Lite::Logging_Levels::INFO_log_level, "SendClipboardText called on a socket that is not open yet");
                return;
            }
            if (Socket_->is_loopback())
                return; // dont send clipboard info to ourselfs as it will cause a loop

            auto ptype = PACKET_TYPES::ONCLIPBOARDTEXTCHANGED;
            auto size = sizeof(ptype) + text.size();
            auto ptr(std::shared_ptr<unsigned char>(new unsigned char[size], [](auto *p) { delete[] p; }));
            *reinterpret_cast<PACKET_TYPES *>(ptr.get()) = ptype;
            memcpy(ptr.get() + sizeof(ptype), text.data(), text.size());

            SL::WS_LITE::WSMessage buf;
            buf.code = WS_LITE::OpCode::BINARY;
            buf.Buffer = ptr;
            buf.len = size;
            buf.data = ptr.get();
            Socket_->send(buf, SL::WS_LITE::CompressionOptions::NO_COMPRESSION);
        }
        virtual void SendKeyUp(Input_Lite::KeyCodes key) override { SendStruct_Impl(key, PACKET_TYPES::ONKEYUP); }
        virtual void SendKeyDown(Input_Lite::KeyCodes key) override { SendStruct_Impl(key, PACKET_TYPES::ONKEYDOWN); }
        virtual void SendMouseUp(const Input_Lite::MouseButtons button) override { SendStruct_Impl(button, PACKET_TYPES::ONMOUSEUP); }
        virtual void SendMouseDown(const Input_Lite::MouseButtons button) override { SendStruct_Impl(button, PACKET_TYPES::ONMOUSEDOWN); }
        virtual void SendMouseScroll(int offset) override { SendStruct_Impl(offset, PACKET_TYPES::ONMOUSESCROLL); }
        virtual void SendMousePosition(const Point &pos) override { SendStruct_Impl(pos, PACKET_TYPES::ONMOUSEPOSITIONCHANGED); }
    };

    class ClientDriverConfiguration : public IClientDriverConfiguration {
        std::shared_ptr<ClientDriver> ClientDriverImpl;

      public:
        ClientDriverConfiguration(const std::shared_ptr<SL::RAT_Lite::ClientDriver> &c) : ClientDriverImpl(c) {}
        virtual ~ClientDriverConfiguration() {}
        virtual std::shared_ptr<IClientDriverConfiguration>
        onMonitorsChanged(const std::function<void(const std::vector<Screen_Capture::Monitor> &)> &callback) override
        {
            assert(!ClientDriverImpl->onMonitorsChanged);
            ClientDriverImpl->onMonitorsChanged = callback;
            return std::make_shared<ClientDriverConfiguration>(ClientDriverImpl);
        }
        virtual std::shared_ptr<IClientDriverConfiguration>
        onFrameChanged(const std::function<void(const Image &, const SL::Screen_Capture::Monitor &)> &callback) override
        {
            assert(!ClientDriverImpl->onFrameChanged);
            ClientDriverImpl->onFrameChanged = callback;
            return std::make_shared<ClientDriverConfiguration>(ClientDriverImpl);
        }
        virtual std::shared_ptr<IClientDriverConfiguration>
        onNewFrame(const std::function<void(const Image &, const SL::Screen_Capture::Monitor &)> &callback) override
        {
            assert(!ClientDriverImpl->onNewFrame);
            ClientDriverImpl->onNewFrame = callback;
            return std::make_shared<ClientDriverConfiguration>(ClientDriverImpl);
        }
        virtual std::shared_ptr<IClientDriverConfiguration> onMouseImageChanged(const std::function<void(const Image &)> &callback) override
        {
            assert(!ClientDriverImpl->onMouseImageChanged);
            ClientDriverImpl->onMouseImageChanged = callback;
            return std::make_shared<ClientDriverConfiguration>(ClientDriverImpl);
        }
        virtual std::shared_ptr<IClientDriverConfiguration> onMousePositionChanged(const std::function<void(const Point &)> &callback) override
        {
            assert(!ClientDriverImpl->onMousePositionChanged);
            ClientDriverImpl->onMousePositionChanged = callback;
            return std::make_shared<ClientDriverConfiguration>(ClientDriverImpl);
        }
        virtual std::shared_ptr<IClientDriverConfiguration> onClipboardChanged(const std::function<void(const std::string &)> &callback) override
        {
            assert(!ClientDriverImpl->onClipboardChanged);
            ClientDriverImpl->onClipboardChanged = callback;
            return std::make_shared<ClientDriverConfiguration>(ClientDriverImpl);
        }

        virtual std::shared_ptr<IClientDriverConfiguration>
        onConnection(const std::function<void(const std::shared_ptr<SL::WS_LITE::IWSocket>)> &callback) override
        {
            assert(!ClientDriverImpl->onConnection);
            ClientDriverImpl->onConnection = callback;
            return std::make_shared<ClientDriverConfiguration>(ClientDriverImpl);
        }
        virtual std::shared_ptr<IClientDriverConfiguration>
        onMessage(const std::function<void(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const WS_LITE::WSMessage)> &callback) override
        {
            assert(!ClientDriverImpl->onMessage);
            ClientDriverImpl->onMessage = callback;
            return std::make_shared<ClientDriverConfiguration>(ClientDriverImpl);
        }
        virtual std::shared_ptr<IClientDriverConfiguration> onDisconnection(
            const std::function<void(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, unsigned short code, const std::string)> &callback)
            override
        {
            assert(!ClientDriverImpl->onDisconnection);
            ClientDriverImpl->onDisconnection = callback;
            return std::make_shared<ClientDriverConfiguration>(ClientDriverImpl);
        }
        virtual std::shared_ptr<IClientDriver> Build(const std::shared_ptr<SL::WS_LITE::IWSClient_Configuration> &wsclientconfig) override
        {
            ClientDriverImpl->Build(wsclientconfig);
            return ClientDriverImpl;
        }
    };

    std::shared_ptr<IClientDriverConfiguration> CreateClientDriverConfiguration()
    {
        return std::make_shared<ClientDriverConfiguration>(std::make_shared<ClientDriver>());
    }

} // namespace RAT_Lite
} // namespace SL
