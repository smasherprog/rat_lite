#include "ServerDriver.h"
#include "Shapes.h"
#include "IServerDriver.h"
#include "Configs.h"
#include "turbojpeg.h"
#include "Input.h"
#include "Logging.h"
#include "ScreenCapture.h"

#include "SCCommon.h"

namespace SL {
    namespace RAT {

        class ServerDriverImpl {

        public:
            std::shared_ptr<Server_Config> Config_;
            std::atomic_int ClientCount;
            IServerDriver * IServerDriver_;
            WS_LITE::WSListener h;

            ServerDriverImpl(IServerDriver * r, std::shared_ptr<Server_Config> config) : Config_(config) {
                ClientCount = 0;
                h = WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))
                    .CreateListener(config->WebSocketTLSLPort)
                    .onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const std::unordered_map<std::string, std::string>& header) {
                    static int counter = 0;
                    if (ClientCount + 1 > Config_->MaxNumConnections) {
                        socket->close(1000, "Closing due to max number of connections!");
                    }
                    else {
                        int t = counter++ % Config_->MaxWebSocketThreads;
                        SL_RAT_LOG(Logging_Levels::INFO_log_level, "Transfering connection to thread " << t);
                        ClientCount += 1;
                        IServerDriver_->onConnection(socket);
                    }
                }).onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) {
                    SL_RAT_LOG(Logging_Levels::INFO_log_level, "onDisconnection  ");
                    ClientCount -= 1;
                    IServerDriver_->onDisconnection(socket, code, msg);
                }).onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& message) {
                    auto length = message.len;
                    auto pactype = PACKET_TYPES::INVALID;
                    assert(length >= sizeof(pactype));

                    pactype = *reinterpret_cast<const PACKET_TYPES*>(message.data);
                    length -= sizeof(pactype);
                    auto data = message.data + sizeof(pactype);
                    switch (pactype) {
                    case PACKET_TYPES::MOUSEEVENT:
                        assert(length == sizeof(MouseEvent));
                        IServerDriver_->onReceive_Mouse(reinterpret_cast<const MouseEvent*>(data));
                        break;
                    case PACKET_TYPES::KEYEVENT:
                        assert(length == sizeof(KeyEvent));
                        IServerDriver_->onReceive_Key(reinterpret_cast<const KeyEvent*>(data));
                        break;
                    case PACKET_TYPES::CLIPBOARDTEXTEVENT:
                        IServerDriver_->onReceive_ClipboardText(data, length);
                        break;
                    default:
                        IServerDriver_->onMessage(socket, message);
                        break;
                    }
                }).listen(true, true);
            }
            ~ServerDriverImpl() {

            }
            void Send(const WS_LITE::WSocket* socket, char* data, size_t len) {
                if (socket) {

                  //  socket->send(data, len);
                }
                else {
                    //ServerHub_.Broadcast(data, len);
                }
            }

            void SendScreen(const WS_LITE::WSocket* socket, const Screen_Capture::Image & img, const SL::Screen_Capture::Monitor& monitor, PACKET_TYPES p) {

                if (ClientCount <= 0) return;
                Rect r(Point(img.Bounds.left, img.Bounds.top), Height(img), Width(img));

                auto set = Config_->SendGrayScaleImages ? TJSAMP_GRAY : TJSAMP_420;
                auto maxsize = tjBufSize(Screen_Capture::Width(img), Screen_Capture::Height(img), set) + sizeof(r) + sizeof(p) + sizeof(monitor.Id);

                auto jpegCompressor = tjInitCompress();
                auto  buffer = std::make_unique<char[]>(maxsize);

                auto dst = (unsigned char*)buffer.get();
                memcpy(dst, &p, sizeof(p));
                dst += sizeof(p);
                memcpy(dst, &monitor.Id, sizeof(monitor.Id));
                dst += sizeof(monitor.Id);
                memcpy(dst, &r, sizeof(r));
                dst += sizeof(r);

                auto srcbuffer = std::make_unique<char[]>(RowStride(img)*Height(img));
                Screen_Capture::Extract(img, srcbuffer.get(), RowStride(img)*Height(img));
                auto srcbuf = (unsigned char*)srcbuffer.get();

#if __ANDROID__
                auto colorencoding = TJPF_RGBX;
#else 
                auto colorencoding = TJPF_BGRX;
#endif
                auto outjpegsize = maxsize;

                if (tjCompress2(jpegCompressor, srcbuf, r.Width, 0, r.Height, colorencoding, &dst, &outjpegsize, set, Config_->ImageCompressionSetting, TJFLAG_FASTDCT | TJFLAG_NOREALLOC) == -1) {
                    SL_RAT_LOG(Logging_Levels::ERROR_log_level, tjGetErrorStr());
                }
                //	std::cout << "Sending " << r << std::endl;
                auto finalsize = sizeof(p) + sizeof(r) + sizeof(monitor.Id) + outjpegsize;//adjust the correct size
                tjDestroy(jpegCompressor);
                Send(socket, buffer.get(), finalsize);
            }
            void SendMouse(const WS_LITE::WSocket* socket, const Screen_Capture::Image & img) {

                if (ClientCount <= 0) return;
                Rect r(Point(0, 0), Height(img), Width(img));

                auto p = static_cast<unsigned int>(PACKET_TYPES::MOUSEIMAGE);
                auto finalsize = (Screen_Capture::RowStride(img) * Screen_Capture::Height(img)) + sizeof(p) + sizeof(r);
                auto buffer = std::make_unique<char[]>(finalsize);

                auto dst = buffer.get();
                memcpy(dst, &p, sizeof(p));
                dst += sizeof(p);
                memcpy(dst, &r, sizeof(r));
                dst += sizeof(r);
                Screen_Capture::Extract(img, dst, Screen_Capture::RowStride(img) * Screen_Capture::Height(img));

                Send(socket, buffer.get(), finalsize);

            }
            void SendMonitorInfo(const WS_LITE::WSocket* socket, const std::vector<std::shared_ptr<Screen_Capture::Monitor>>& monitors) {
                if (ClientCount <= 0) return;
                auto p = static_cast<unsigned int>(PACKET_TYPES::MONITORINFO);
                const auto size = (monitors.size() * sizeof(Screen_Capture::Monitor)) + sizeof(p);

                auto buffer = std::make_unique<char[]>(size);
                auto buf = buffer.get();
                memcpy(buf, &p, sizeof(p));
                buf += sizeof(p);
                for (auto& a : monitors) {
                    memcpy(buf, a.get(), sizeof(Screen_Capture::Monitor));
                    buf += sizeof(Screen_Capture::Monitor);
                }
                Send(socket, buffer.get(), size);

            }
            void SendMouse(const WS_LITE::WSocket* socket, const Point& pos)
            {
                if (ClientCount <= 0) return;
                auto p = static_cast<unsigned int>(PACKET_TYPES::MOUSEPOS);
                const auto size = sizeof(pos) + sizeof(p);
                char buffer[size];

                memcpy(buffer, &p, sizeof(p));
                memcpy(buffer + sizeof(p), &pos, sizeof(pos));

                Send(socket, buffer, size);
            }

            void SendClipboardText(const WS_LITE::WSocket* socket, const char* data, unsigned int len) {
                if (ClientCount <= 0) return;
                auto p = static_cast<unsigned int>(PACKET_TYPES::CLIPBOARDTEXTEVENT);
                auto size = len + sizeof(p);

                auto buffer = std::make_unique<char[]>(size);
                memcpy(buffer.get(), &p, sizeof(p));
                memcpy(buffer.get() + sizeof(p), data, len);

                Send(socket, buffer.get(), size);

            }
        };
        ServerDriver::ServerDriver(IServerDriver * r, std::shared_ptr<Server_Config> config) {
            ServerDriverImpl_ = std::make_unique<ServerDriverImpl>(r, config);

        }
        ServerDriver::~ServerDriver()
        {

        }
        void ServerDriver::SendFrameChange(const WS_LITE::WSocket* socket, const Screen_Capture::Image & img, const SL::Screen_Capture::Monitor& monitor)
        {
            ServerDriverImpl_->SendScreen(socket, img, monitor, PACKET_TYPES::SCREENIMAGEDIF);
        }
        void ServerDriver::SendMonitorInfo(const WS_LITE::WSocket* socket, const std::vector<std::shared_ptr<Screen_Capture::Monitor>>& monitors)
        {
            ServerDriverImpl_->SendMonitorInfo(socket, monitors);
        }
        void ServerDriver::SendMouse(const WS_LITE::WSocket* socket, const Screen_Capture::Image & img)
        {
            ServerDriverImpl_->SendMouse(socket, img);
        }
        void ServerDriver::SendMouse(const WS_LITE::WSocket* socket, const Point & pos)
        {
            ServerDriverImpl_->SendMouse(socket, pos);
        }

        void ServerDriver::SendClipboardText(const WS_LITE::WSocket* socket, const char* data, unsigned int len) {
            ServerDriverImpl_->SendClipboardText(socket, data, len);
        }


    }
}
