#include "Logging.h"
#include "RAT.h"
#include "ScreenCapture.h"
#include "turbojpeg.h"

#include "Input_Lite.h"
#include "WS_Lite.h"
#include <atomic>

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

namespace SL {
namespace RAT {

    class ServerDriver : public IServerDriver {

      public:
        ServerDriver() {}
        virtual ~ServerDriver() {}
        std::atomic<int> ClientCount;

        std::shared_ptr<WS_LITE::WSListener> h;

        int MaxNumConnections = 10;
        virtual void MaxConnections(int maxconnections) override
        {
            assert(maxconnections >= 0);
            MaxNumConnections = maxconnections;
        }
        virtual int MaxConnections() const override { return MaxNumConnections; }

        void onKeyUp(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {
            if (len == sizeof(Input_Lite::KeyCodes)) {
                IServerDriver_->onKeyUp(socket, *reinterpret_cast<const Input_Lite::KeyCodes *>(data));
            }
            else {
                return socket->close(1000, "Received invalid onKeyUp Event");
            }
        }
        void onKeyDown(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {

            if (len == sizeof(Input_Lite::KeyCodes)) {
                IServerDriver_->onKeyDown(socket, *reinterpret_cast<const Input_Lite::KeyCodes *>(data));
            }
            else {
                return socket->close(1000, "Received invalid onKeyDown Event");
            }
        }
        void onMouseUp(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {
            if (len == sizeof(Input_Lite::MouseButtons)) {
                return IServerDriver_->onMouseUp(socket, *reinterpret_cast<const Input_Lite::MouseButtons *>(data));
            }
            socket->close(1000, "Received invalid onMouseUp Event");
        }

        void onMouseDown(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {
            if (len == sizeof(Input_Lite::MouseButtons)) {
                return IServerDriver_->onMouseDown(socket, *reinterpret_cast<const Input_Lite::MouseButtons *>(data));
            }
            socket->close(1000, "Received invalid onMouseDown Event");
        }
        void onMousePosition(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {
            if (len == sizeof(Point)) {
                auto p = *reinterpret_cast<const Point *>(data);

#ifdef __APPLE__

                CGDisplayCount count = 0;
                CGGetActiveDisplayList(0, 0, &count);
                std::vector<CGDirectDisplayID> displays;
                displays.resize(count);
                CGGetActiveDisplayList(count, displays.data(), &count);

                for (auto i = 0; i < count; i++) {
                    // only include non-mirrored displays
                    if (CGDisplayMirrorsDisplay(displays[i]) == kCGNullDirectDisplay) {

                        auto dismode = CGDisplayCopyDisplayMode(displays[i]);
                        auto scaledsize = CGDisplayBounds(displays[i]);

                        auto pixelwidth = CGDisplayModeGetPixelWidth(dismode);
                        auto pixelheight = CGDisplayModeGetPixelHeight(dismode);

                        CGDisplayModeRelease(dismode);

                        if (scaledsize.size.width != pixelwidth) { // scaling going on!
                            p.X = static_cast<float>(p.X) * static_cast<float>(scaledsize.size.width) / static_cast<float>(pixelwidth);
                        }
                        if (scaledsize.size.height != pixelheight) { // scaling going on!
                            p.Y = static_cast<float>(p.Y) * static_cast<float>(scaledsize.size.height) / static_cast<float>(pixelheight);
                        }
                        break;
                    }
                }

#endif

                return IServerDriver_->onMousePosition(socket, p);
            }
            socket->close(1000, "Received invalid onMouseDown Event");
        }
        void onMouseScroll(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {
            if (len == sizeof(int)) {
                return IServerDriver_->onMouseScroll(socket, *reinterpret_cast<const int *>(data));
            }
            socket->close(1000, "Received invalid onMouseScroll Event");
        }
        void onClipboardChanged(const std::shared_ptr<WS_LITE::IWSocket> &socket, const unsigned char *data, size_t len)
        {
            if (len < 1024 * 100) { // 100K max
                std::string str(reinterpret_cast<const char *>(data), len);
                return IServerDriver_->onClipboardChanged(str);
            }
        }

        void Build(const std::shared_ptr<SL::WS_LITE::IWSListener_Configuration> &wslistenerconfig)
        {

            ClientCount = 0;
            wslistenerconfig
                ->onConnection([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, const std::unordered_map<std::string, std::string> &header) {
                    if (MaxNumConnections > 0 && ClientCount + 1 > MaxNumConnections) {
                        socket->close(1000, "Closing due to max number of connections!");
                    }
                    else {
                        IServerDriver_->onConnection(socket);
                        ClientCount += 1;
                    }
                })
                ->onDisconnection([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, unsigned short code, const std::string &msg) {
                    SL_RAT_LOG(Logging_Levels::INFO_log_level, "onDisconnection  ");
                    ClientCount -= 1;
                    IServerDriver_->onDisconnection(socket, code, msg);
                })
                ->onMessage([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, const WS_LITE::WSMessage &message) {

                    auto p = PACKET_TYPES::INVALID;
                    assert(message.len >= sizeof(p));

                    p = *reinterpret_cast<const PACKET_TYPES *>(message.data);
                    auto datastart = message.data + sizeof(p);
                    auto datasize = message.len - sizeof(p);

                    switch (p) {
                    case PACKET_TYPES::ONKEYDOWN:
                        onKeyDown(socket, datastart, datasize);
                        break;
                    case PACKET_TYPES::ONKEYUP:
                        onKeyUp(socket, datastart, datasize);
                        break;
                    case PACKET_TYPES::ONMOUSEUP:
                        onMouseUp(socket, datastart, datasize);
                        break;
                    case PACKET_TYPES::ONMOUSEDOWN:
                        onMouseDown(socket, datastart, datasize);
                        break;
                    case PACKET_TYPES::ONMOUSEPOSITIONCHANGED:
                        onMousePosition(socket, datastart, datasize);
                        break;
                    case PACKET_TYPES::ONMOUSESCROLL:
                        onMouseScroll(socket, datastart, datasize);
                        break;
                    case PACKET_TYPES::ONCLIPBOARDTEXTCHANGED:
                        onClipboardChanged(socket, datastart, datasize);
                        break;
                    default:
                        IServerDriver_->onMessage(socket, message);
                        break;
                    }
                });
        }

        ~ServerDriver() {}
    };

    WS_LITE::WSMessage PrepareImage(const Screen_Capture::Image &img, int imagecompression, bool usegrayscale, const Screen_Capture::Monitor &monitor,
                                    PACKET_TYPES p)
    {
        Rect r(Point(img.Bounds.left, img.Bounds.top), Height(img), Width(img));
        auto set = usegrayscale ? TJSAMP_GRAY : TJSAMP_420;
        unsigned long maxsize = tjBufSize(Screen_Capture::Width(img), Screen_Capture::Height(img), set) + sizeof(r) + sizeof(p) + sizeof(monitor.Id);
        auto jpegCompressor = tjInitCompress();
        auto buffer = std::shared_ptr<unsigned char>(new unsigned char[maxsize], [](auto *p) { delete[] p; });
        auto dst = (unsigned char *)buffer.get();
        memcpy(dst, &p, sizeof(p));
        dst += sizeof(p);
        memcpy(dst, &monitor.Id, sizeof(monitor.Id));
        dst += sizeof(monitor.Id);
        memcpy(dst, &r, sizeof(r));
        dst += sizeof(r);
        auto srcbuffer = std::make_unique<unsigned char[]>(RowStride(img) * Height(img));
        Screen_Capture::Extract(img, srcbuffer.get(), RowStride(img) * Height(img));
        auto srcbuf = (unsigned char *)srcbuffer.get();
        auto colorencoding = TJPF_RGBX;
        auto outjpegsize = maxsize;

        if (tjCompress2(jpegCompressor, srcbuf, r.Width, 0, r.Height, colorencoding, &dst, &outjpegsize, set, imagecompression,
                        TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
            SL_RAT_LOG(Logging_Levels::ERROR_log_level, tjGetErrorStr());
        }
        //	std::cout << "Sending " << r << std::endl;
        auto finalsize = sizeof(p) + sizeof(r) + sizeof(monitor.Id) + outjpegsize; // adjust the correct size
        tjDestroy(jpegCompressor);
        return WS_LITE::WSMessage{buffer.get(), finalsize, WS_LITE::OpCode::BINARY, buffer};
    }

    WS_LITE::WSMessage IServerDriver::PrepareMonitorsChanged(const std::vector<Screen_Capture::Monitor> &monitors)
    {
        auto p = static_cast<unsigned int>(PACKET_TYPES::ONMONITORSCHANGED);
        const auto finalsize = (monitors.size() * sizeof(Screen_Capture::Monitor)) + sizeof(p);

        auto buffer = std::shared_ptr<unsigned char>(new unsigned char[finalsize], [](auto *p) { delete[] p; });
        auto buf = buffer.get();
        memcpy(buf, &p, sizeof(p));
        buf += sizeof(p);
        for (auto &a : monitors) {
            memcpy(buf, &a, sizeof(a));
            buf += sizeof(Screen_Capture::Monitor);
        }
        return WS_LITE::WSMessage{buffer.get(), finalsize, WS_LITE::OpCode::BINARY, buffer};
    }
    WS_LITE::WSMessage IServerDriver::PrepareFrameChanged(const Screen_Capture::Image &image, const Screen_Capture::Monitor &monitor,
                                                          int imagecompression, bool usegrayscale)
    {
        assert(imagecompression > 0 && imagecompression <= 100);
        return PrepareImage(image, imagecompression, usegrayscale, monitor, PACKET_TYPES::ONFRAMECHANGED);
    }
    WS_LITE::WSMessage IServerDriver::PrepareNewFrame(const Screen_Capture::Image &image, const Screen_Capture::Monitor &monitor,
                                                      int imagecompression, bool usegrayscale)
    {
        assert(imagecompression > 0 && imagecompression <= 100);
        return PrepareImage(image, imagecompression, usegrayscale, monitor, PACKET_TYPES::ONNEWFRAME);
    }
    WS_LITE::WSMessage IServerDriver::PrepareMouseImageChanged(const Screen_Capture::Image &image)
    {
        Rect r(Point(0, 0), Height(image), Width(image));
        auto p = static_cast<unsigned int>(PACKET_TYPES::ONMOUSEIMAGECHANGED);
        auto finalsize = (Screen_Capture::RowStride(image) * Screen_Capture::Height(image)) + sizeof(p) + sizeof(r);
        auto buffer = std::shared_ptr<unsigned char>(new unsigned char[finalsize], [](auto *p) { delete[] p; });
        auto dst = buffer.get();
        memcpy(dst, &p, sizeof(p));
        dst += sizeof(p);
        memcpy(dst, &r, sizeof(r));
        dst += sizeof(r);
        Screen_Capture::Extract(image, (unsigned char *)dst, Screen_Capture::RowStride(image) * Screen_Capture::Height(image));
        return WS_LITE::WSMessage{buffer.get(), finalsize, WS_LITE::OpCode::BINARY, buffer};
    }
    WS_LITE::WSMessage IServerDriver::PrepareMousePositionChanged(const SL::Screen_Capture::Point &pos)
    {
        auto p = static_cast<unsigned int>(PACKET_TYPES::ONMOUSEPOSITIONCHANGED);
        const auto finalsize = sizeof(pos) + sizeof(p);
        auto buffer = std::shared_ptr<unsigned char>(new unsigned char[finalsize], [](auto *p) { delete[] p; });
        memcpy(buffer.get(), &p, sizeof(p));
        memcpy(buffer.get() + sizeof(p), &pos, sizeof(pos));
        return WS_LITE::WSMessage{buffer.get(), finalsize, WS_LITE::OpCode::BINARY, buffer};
    }
    WS_LITE::WSMessage IServerDriver::PrepareClipboardChanged(const std::string &text)
    {
        auto p = static_cast<unsigned int>(PACKET_TYPES::ONCLIPBOARDTEXTCHANGED);
        auto finalsize = text.size() + sizeof(p);
        auto buffer = std::shared_ptr<unsigned char>(new unsigned char[finalsize], [](auto *p) { delete[] p; });
        memcpy(buffer.get(), &p, sizeof(p));
        memcpy(buffer.get() + sizeof(p), text.data(), text.size());
        return WS_LITE::WSMessage{buffer.get(), finalsize, WS_LITE::OpCode::BINARY, buffer};
    }

    class ServerDriverConfiguration : public IServerDriverConfiguration {
        std::shared_ptr<ServerDriver> ServerDriver;

      public:
        ServerDriverConfiguration(const std::shared_ptr<SL::RAT::ServerDriver> &c) : ServerDriver(c) {}
        virtual ~ServerDriverConfiguration() {}
        virtual std::shared_ptr<IServerDriverConfiguration>
        onKeyUp(const std::function<void(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::KeyCodes key)> &callback) override
        {
            assert(!ServerDriver->onKeyUp);
            ServerDriver->onKeyUp = callback;
            return std::make_shared<ServerDriverConfiguration>(ServerDriver);
        }
        virtual std::shared_ptr<IServerDriverConfiguration>
        onKeyDown(const std::function<void(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::KeyCodes key)> &callback) override
        {
            assert(!ServerDriver->onKeyDown);
            ServerDriver->onKeyDown = callback;
            return std::make_shared<ServerDriverConfiguration>(ServerDriver);
        }
        virtual std::shared_ptr<IServerDriverConfiguration>
        onMouseUp(const std::function<void(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::MouseButtons button)> &callback) override
        {
            assert(!ServerDriver->onMouseUp);
            ServerDriver->onMouseUp = callback;
            return std::make_shared<ServerDriverConfiguration>(ServerDriver);
        }
        virtual std::shared_ptr<IServerDriverConfiguration>
        onMouseDown(const std::function<void(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::MouseButtons button)> &callback) override
        {
            assert(!ServerDriver->onMouseDown);
            ServerDriver->onMouseDown = callback;
            return std::make_shared<ServerDriverConfiguration>(ServerDriver);
        }
        virtual std::shared_ptr<IServerDriverConfiguration>
        onMouseScroll(const std::function<void(const std::shared_ptr<WS_LITE::IWSocket> &socket, int offset)> &callback) override
        {
            assert(!ServerDriver->onMouseScroll);
            ServerDriver->onMouseScroll = callback;
            return std::make_shared<ServerDriverConfiguration>(ServerDriver);
        }
        virtual std::shared_ptr<IServerDriverConfiguration>
        onMousePosition(const std::function<void(const std::shared_ptr<WS_LITE::IWSocket> &socket, const Point &pos)> &callback) override
        {
            assert(!ServerDriver->onMousePosition);
            ServerDriver->onMousePosition = callback;
            return std::make_shared<ServerDriverConfiguration>(ServerDriver);
        }
        virtual std::shared_ptr<IServerDriverConfiguration> onClipboardChanged(const std::function<void(const std::string &text)> &callback) override
        {
            assert(!ServerDriver->onClipboardChanged);
            ServerDriver->onClipboardChanged = callback;
            return std::make_shared<ServerDriverConfiguration>(ServerDriver);
        }
        virtual std::shared_ptr<IServerDriver> Build(const std::shared_ptr<SL::WS_LITE::IWSListener_Configuration> &wslistenerconfig) override
        {
            ServerDriver->Build(wslistenerconfig);
            return ServerDriver;
        }
    };

    std::shared_ptr<IServerDriverConfiguration> CreateServerDriverConfiguration()
    {
        return std::make_shared<ServerDriverConfiguration>(std::make_shared<ServerDriver>());
    }

} // namespace RAT
} // namespace SL
