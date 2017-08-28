#include "ClientDriver.h"
#include "IClientDriver.h"
#include "NetworkStructs.h"
#include "Shapes.h"
#include "turbojpeg.h"

#include "Configs.h"
#include "Logging.h"
#include "SCCommon.h"

#include "WS_Lite.h"

#include <assert.h>
#include <mutex>

namespace SL {
namespace RAT {

    class ClientDriverImpl {

        IClientDriver *IClientDriver_;
        std::shared_ptr<Client_Config> Config_;
        WS_LITE::WSClient h;
        Point LastMousePosition_;
        std::shared_ptr<WS_LITE::IWSocket> Socket_;
        std::thread Runner;
        std::mutex outputbufferLock;
        std::vector<unsigned char> outputbuffer;
        std::vector<Screen_Capture::Monitor> Monitors;

        void onMouseImageChanged(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {
            if (len >= sizeof(Rect)) {
                Image img(*reinterpret_cast<const Rect *>(data), data + sizeof(Rect), len - sizeof(Rect));
                if (len >= sizeof(Rect) + (img.Rect_.Width * img.Rect_.Height * PixelStride)) {
                    return IClientDriver_->onMouseImageChanged(img);
                }
            }
            socket->close(1000, "Received invalid lenght on onMouseImageChanged");
        }
        void onMousePositionChanged(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {
            if (len == sizeof(SL::Screen_Capture::Point)) {
                return IClientDriver_->onMousePositionChanged(*reinterpret_cast<const RAT::Point *>(data));
            }
            socket->close(1000, "Received invalid lenght on onMousePositionChanged");
        }

        void onMonitorsChanged(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {
            auto num = len / sizeof(Screen_Capture::Monitor);
            Monitors.clear();
            if (len == num * sizeof(Screen_Capture::Monitor) && num < 8) {
                for (decltype(num) i = 0; i < num; i++) {
                    Monitors.push_back(*(Screen_Capture::Monitor *)data);
                }
                return IClientDriver_->onMonitorsChanged(Monitors);
            }
            else if (len == 0) {
                // it is possible to have no monitors.. shouldnt disconnect in that case
                return IClientDriver_->onMonitorsChanged(Monitors);
            }
            socket->close(1000, "Invalid Monitor Count");
        }
        void onClipboardTextChanged(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {
            if (len < 1024 * 100) { // 100K max
                std::string str(reinterpret_cast<const char *>(data), len);
                return IClientDriver_->onClipboardChanged(str);
            }
        }
        template <typename CALLBACKTYPE>
        void onFrame(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len, CALLBACKTYPE &&cb)
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
                SL_RAT_LOG(Logging_Levels::INFO_log_level, "Monitor Id doesnt exist!");
                return;
            }
            len -= sizeof(Rect) + sizeof(monitor_id);

            if (tjDecompressHeader2(jpegDecompressor, src, static_cast<unsigned long>(len), &outwidth, &outheight, &jpegSubsamp) == -1) {
                SL_RAT_LOG(Logging_Levels::ERROR_log_level, tjGetErrorStr());
            }
            std::lock_guard<std::mutex> lock(outputbufferLock);
            outputbuffer.reserve(outwidth * outheight * PixelStride);

            if (tjDecompress2(jpegDecompressor, src, static_cast<unsigned long>(len), (unsigned char *)outputbuffer.data(), outwidth, 0, outheight,
                              TJPF_RGBX, 2048 | TJFLAG_NOREALLOC) == -1) {
                SL_RAT_LOG(Logging_Levels::ERROR_log_level, tjGetErrorStr());
            }
            Image img(rect, outputbuffer.data(), outwidth * outheight * PixelStride);

            assert(outwidth == img.Rect_.Width && outheight == img.Rect_.Height);
            cb(img, *monitor);
            tjDestroy(jpegDecompressor);
        }

      public:
        ClientDriverImpl(IClientDriver *r) : IClientDriver_(r) {}
        void Connect(std::shared_ptr<Client_Config> config, const char *dst_host)
        {
            Config_ = config;
            auto hc = std::string(dst_host);
            h = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))
                    .CreateClient()
                    .onConnection(
                        [&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const std::unordered_map<std::string, std::string> &header) {
                            SL_RAT_LOG(Logging_Levels::INFO_log_level, "onConnection ");
                            Socket_ = socket;
                            IClientDriver_->onConnection(socket);
                        })
                    .onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, unsigned short code, const std::string &msg) {
                        SL_RAT_LOG(Logging_Levels::INFO_log_level, "onDisconnection ");
                        Socket_.reset();
                        IClientDriver_->onDisconnection(socket, code, msg);
                    })
                    .onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const SL::WS_LITE::WSMessage &message) {
                        auto p = PACKET_TYPES::INVALID;
                        assert(message.len >= sizeof(p));

                        p = *reinterpret_cast<const PACKET_TYPES *>(message.data);
                        auto datastart = message.data + sizeof(p);
                        auto datasize = message.len - sizeof(p);
                        switch (p) {
                        case PACKET_TYPES::ONMONITORSCHANGED:
                            onMonitorsChanged(socket, datastart, datasize);
                            break;
                        case PACKET_TYPES::ONFRAMECHANGED:
                            onFrame(socket, datastart, datasize,
                                    [&](const auto &img, const auto &monitor) { IClientDriver_->onFrameChanged(img, monitor); });
                            break;
                        case PACKET_TYPES::ONNEWFRAME:
                            onFrame(socket, datastart, datasize,
                                    [&](const auto &img, const auto &monitor) { IClientDriver_->onNewFrame(img, monitor); });
                            break;
                        case PACKET_TYPES::ONMOUSEIMAGECHANGED:
                            onMouseImageChanged(socket, datastart, datasize);
                            break;
                        case PACKET_TYPES::ONMOUSEPOSITIONCHANGED:
                            onMousePositionChanged(socket, datastart, datasize);
                            break;
                        case PACKET_TYPES::ONCLIPBOARDTEXTCHANGED:
                            onClipboardTextChanged(socket, datastart, datasize);
                            break;
                        default:
                            IClientDriver_->onMessage(socket, message); // pass up the chain
                            break;
                        }

                    })
                    .connect(hc.c_str(), config->WebSocketTLSLPort);
        }

        virtual ~ClientDriverImpl() {}

        template <typename STRUCT> void SendStruct_Impl(STRUCT key, PACKET_TYPES ptype)
        {
            if (!Socket_) {
                SL_RAT_LOG(Logging_Levels::INFO_log_level, "SendKey called on a socket that is not open yet");
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
            Socket_->send(buf, false);
        }
        void SendClipboardChanged(const std::string &text)
        {
            if (!Socket_) {
                SL_RAT_LOG(Logging_Levels::INFO_log_level, "SendClipboardText called on a socket that is not open yet");
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
            Socket_->send(buf, false);
        }
    };
    ClientDriver::ClientDriver(IClientDriver *r) { ClientDriverImpl_ = std::make_shared<ClientDriverImpl>(r); }
    ClientDriver::~ClientDriver() {}
    void ClientDriver::Connect(std::shared_ptr<Client_Config> config, const char *dst_host) { ClientDriverImpl_->Connect(config, dst_host); }
    void ClientDriver::SendKeyUp(Input_Lite::KeyCodes key) { ClientDriverImpl_->SendStruct_Impl(key, PACKET_TYPES::ONKEYUP); }
    void ClientDriver::SendKeyDown(Input_Lite::KeyCodes key) { ClientDriverImpl_->SendStruct_Impl(key, PACKET_TYPES::ONKEYDOWN); }
    void ClientDriver::SendMouseUp(const Input_Lite::MouseButtons button) { ClientDriverImpl_->SendStruct_Impl(button, PACKET_TYPES::ONMOUSEUP); }
    void ClientDriver::SendMouseDown(const Input_Lite::MouseButtons button) { ClientDriverImpl_->SendStruct_Impl(button, PACKET_TYPES::ONMOUSEDOWN); }
    void ClientDriver::SendMouseScroll(int offset) { ClientDriverImpl_->SendStruct_Impl(offset, PACKET_TYPES::ONMOUSESCROLL); }
    void ClientDriver::SendMousePosition(const Point &pos) { ClientDriverImpl_->SendStruct_Impl(pos, PACKET_TYPES::ONMOUSEPOSITIONCHANGED); }
    void ClientDriver::SendClipboardChanged(const std::string &text) { return ClientDriverImpl_->SendClipboardChanged(text); }
} // namespace RAT
} // namespace SL
