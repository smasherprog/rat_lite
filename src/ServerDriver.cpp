#include "Configs.h"
#include "IServerDriver.h"
#include "Logging.h"
#include "NetworkStructs.h"
#include "ScreenCapture.h"
#include "ServerDriver.h"
#include "Shapes.h"
#include "turbojpeg.h"

#include "SCCommon.h"
#include "WS_Lite.h"

#include <atomic>

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

namespace SL {
namespace RAT {

    class ServerDriverImpl {

      public:
        std::shared_ptr<Server_Config> Config_;
        IServerDriver *IServerDriver_;
        std::atomic<int> ClientCount;
        WS_LITE::WSListener h;

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
        ServerDriverImpl(IServerDriver *r, std::shared_ptr<Server_Config> config) : Config_(config), IServerDriver_(r)
        {
            ClientCount = 0;
            h = WS_LITE::CreateContext(WS_LITE::ThreadCount(1))
                    .CreateListener(config->WebSocketTLSLPort)
                    .onConnection([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, const std::unordered_map<std::string, std::string> &header) {
                        if (Config_->MaxNumConnections > 0 && ClientCount + 1 > Config_->MaxNumConnections) {
                            socket->close(1000, "Closing due to max number of connections!");
                        }
                        else {
                            IServerDriver_->onConnection(socket);
                            ClientCount += 1;
                        }
                    })
                    .onDisconnection([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, unsigned short code, const std::string &msg) {
                        SL_RAT_LOG(Logging_Levels::INFO_log_level, "onDisconnection  ");
                        ClientCount -= 1;
                        IServerDriver_->onDisconnection(socket, code, msg);
                    })
                    .onMessage([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, const WS_LITE::WSMessage &message) {

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
                    })
                    .listen(true, true);
        }
        ~ServerDriverImpl() {}

        WS_LITE::WSMessage PrepareMonitorsChanged(const std::vector<Screen_Capture::Monitor> &monitors)
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

        WS_LITE::WSMessage PrepareImage(const Screen_Capture::Image &img, const Screen_Capture::Monitor &monitor, PACKET_TYPES p)
        {
            Rect r(Point(img.Bounds.left, img.Bounds.top), Height(img), Width(img));
            auto set = Config_->SendGrayScaleImages ? TJSAMP_GRAY : TJSAMP_420;
            unsigned long maxsize =
                tjBufSize(Screen_Capture::Width(img), Screen_Capture::Height(img), set) + sizeof(r) + sizeof(p) + sizeof(monitor.Id);
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

            if (tjCompress2(jpegCompressor, srcbuf, r.Width, 0, r.Height, colorencoding, &dst, &outjpegsize, set, Config_->ImageCompressionSetting,
                            TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
                SL_RAT_LOG(Logging_Levels::ERROR_log_level, tjGetErrorStr());
            }
            //	std::cout << "Sending " << r << std::endl;
            auto finalsize = sizeof(p) + sizeof(r) + sizeof(monitor.Id) + outjpegsize; // adjust the correct size
            tjDestroy(jpegCompressor);
            return WS_LITE::WSMessage{buffer.get(), finalsize, WS_LITE::OpCode::BINARY, buffer};
        }

        WS_LITE::WSMessage PrepareMouseImageChanged(const Screen_Capture::Image &img)
        {
            Rect r(Point(0, 0), Height(img), Width(img));
            auto p = static_cast<unsigned int>(PACKET_TYPES::ONMOUSEIMAGECHANGED);
            auto finalsize = (Screen_Capture::RowStride(img) * Screen_Capture::Height(img)) + sizeof(p) + sizeof(r);
            auto buffer = std::shared_ptr<unsigned char>(new unsigned char[finalsize], [](auto *p) { delete[] p; });
            auto dst = buffer.get();
            memcpy(dst, &p, sizeof(p));
            dst += sizeof(p);
            memcpy(dst, &r, sizeof(r));
            dst += sizeof(r);
            Screen_Capture::Extract(img, (unsigned char *)dst, Screen_Capture::RowStride(img) * Screen_Capture::Height(img));
            return WS_LITE::WSMessage{buffer.get(), finalsize, WS_LITE::OpCode::BINARY, buffer};
        }
        WS_LITE::WSMessage PrepareMousePositionChanged(const SL::Screen_Capture::Point &pos)
        {
            auto p = static_cast<unsigned int>(PACKET_TYPES::ONMOUSEPOSITIONCHANGED);
            const auto finalsize = sizeof(pos) + sizeof(p);
            auto buffer = std::shared_ptr<unsigned char>(new unsigned char[finalsize], [](auto *p) { delete[] p; });
            memcpy(buffer.get(), &p, sizeof(p));
            memcpy(buffer.get() + sizeof(p), &pos, sizeof(pos));
            return WS_LITE::WSMessage{buffer.get(), finalsize, WS_LITE::OpCode::BINARY, buffer};
        }
        WS_LITE::WSMessage PrepareClipboardChanged(const std::string &text)
        {
            auto p = static_cast<unsigned int>(PACKET_TYPES::ONCLIPBOARDTEXTCHANGED);
            auto finalsize = text.size() + sizeof(p);
            auto buffer = std::shared_ptr<unsigned char>(new unsigned char[finalsize], [](auto *p) { delete[] p; });
            memcpy(buffer.get(), &p, sizeof(p));
            memcpy(buffer.get() + sizeof(p), text.data(), text.size());
            return WS_LITE::WSMessage{buffer.get(), finalsize, WS_LITE::OpCode::BINARY, buffer};
        }
    };
    ServerDriver::ServerDriver(IServerDriver *r, std::shared_ptr<Server_Config> config)
    {
        ServerDriverImpl_ = std::make_unique<ServerDriverImpl>(r, config);
    }
    ServerDriver::~ServerDriver() {}

    WS_LITE::WSMessage ServerDriver::PrepareMonitorsChanged(const std::vector<Screen_Capture::Monitor> &monitors)
    {
        return ServerDriverImpl_->PrepareMonitorsChanged(monitors);
    }
    WS_LITE::WSMessage ServerDriver::PrepareFrameChanged(const Screen_Capture::Image &image, const Screen_Capture::Monitor &monitor)
    {
        return ServerDriverImpl_->PrepareImage(image, monitor, PACKET_TYPES::ONFRAMECHANGED);
    }
    WS_LITE::WSMessage ServerDriver::PrepareNewFrame(const Screen_Capture::Image &image, const Screen_Capture::Monitor &monitor)
    {
        return ServerDriverImpl_->PrepareImage(image, monitor, PACKET_TYPES::ONNEWFRAME);
    }
    WS_LITE::WSMessage ServerDriver::PrepareMouseImageChanged(const Screen_Capture::Image &image)
    {
        return ServerDriverImpl_->PrepareMouseImageChanged(image);
    }
    WS_LITE::WSMessage ServerDriver::PrepareMousePositionChanged(const SL::Screen_Capture::Point &mevent)
    {
        return ServerDriverImpl_->PrepareMousePositionChanged(mevent);
    }
    WS_LITE::WSMessage ServerDriver::PrepareClipboardChanged(const std::string &text) { return ServerDriverImpl_->PrepareClipboardChanged(text); }

} // namespace RAT
} // namespace SL
