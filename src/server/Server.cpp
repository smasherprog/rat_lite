#include "Server.h"
#include <assert.h>
#include <chrono>
#include <mutex>
#include <string.h>
#include <thread>

#include "Clipboard_Lite.h"
#include "Input_Lite.h"
#include "Logging.h"
#include "RAT.h"
#include "ScreenCapture.h"

#include "WS_Lite.h"
using namespace std::chrono_literals;

namespace SL {
namespace RAT_Server {
    struct NewClient {
        std::shared_ptr<WS_LITE::IWSocket> s;
        std::vector<int> mids;
    };
    class ServerImpl {
      public:
        std::shared_ptr<Screen_Capture::ScreenCaptureManager> ScreenCaptureManager_;
        std::shared_ptr<Clipboard_Lite::IClipboard_Manager> Clipboard_;
        std::shared_ptr<RAT_Lite::IServerDriver> IServerDriver_;

        Server_Status Status_ = Server_Status::SERVER_STOPPED;
        std::mutex ClientsLock;
        std::vector<std::shared_ptr<WS_LITE::IWSocket>> Clients;

        std::mutex ClientsThatNeedFullFramesLock;
        std::vector<NewClient> ClientsThatNeedFullFrames;

        bool ShareClip = false;
        int ImageCompressionSettingRequested = 70;
        int ImageCompressionSettingActual = 70;
        int MouseCaptureRate = 50;
        int ScreenImageCaptureRateRequested = 100;
        int ScreenImageCaptureRateActual = 100;

        bool IgnoreIncomingKeyboardEvents = false;
        bool IgnoreIncomingMouseEvents = false;
        bool EncodeImagesAsGrayScale = false;
        size_t MaxPendingData = 1024 * 1024 * 100;

        ServerImpl()
        {
            Status_ = Server_Status::SERVER_STOPPED;
            Clipboard_ = Clipboard_Lite::CreateClipboard()
                             ->onText([&](const std::string &text) {
                                 if (ShareClip && IServerDriver_) {
                                     SendtoAll(IServerDriver_->PrepareClipboardChanged(text));
                                 }
                             })
                             ->run();

            ScreenCaptureManager_ =
                Screen_Capture::CreateCaptureConfiguration([&]() {
                    auto monitors = Screen_Capture::GetMonitors();
                    if (!IServerDriver_)
                        return monitors;
                    SendtoAll(IServerDriver_->PrepareMonitorsChanged(monitors));
                    // add everyone to the list!

                    std::vector<NewClient> newclients;
                    {
                        std::lock_guard<std::mutex> lock(ClientsLock);
                        for (const auto &a : Clients) {
                            std::vector<int> ids;
                            for (auto &monitor : monitors) {
                                ids.push_back(Screen_Capture::Id(monitor));
                            }
                            newclients.push_back({a, ids});
                        }
                    }
                    {
                        std::lock_guard<std::mutex> lock(ClientsThatNeedFullFramesLock);
                        ClientsThatNeedFullFrames = newclients;
                    }

                    return monitors;
                })
                    ->onNewFrame([&](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor) {
                        if (!ClientsThatNeedFullFrames.empty() && IServerDriver_) {
                            auto msg = IServerDriver_->PrepareNewFrame(img, monitor, ImageCompressionSettingActual, EncodeImagesAsGrayScale);
                            std::lock_guard<std::mutex> lock(ClientsThatNeedFullFramesLock);
                            for (auto &a : ClientsThatNeedFullFrames) {
                                auto itr = std::find_if(std::begin(a.mids), std::end(a.mids),
                                                        [&](const auto i) { return i == Screen_Capture::Id(monitor); });
                                if (itr != std::end(a.mids)) {
                                    // send and erase the montor from the list
                                    a.s->send(msg, false);
                                    a.mids.erase(itr);
                                }
                            }
                            ClientsThatNeedFullFrames.erase(std::remove_if(begin(ClientsThatNeedFullFrames), end(ClientsThatNeedFullFrames),
                                                                           [&](const auto i) { return i.mids.empty(); }),
                                                            end(ClientsThatNeedFullFrames));
                        }
                    })
                    ->onFrameChanged([&](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor) {
                        if (!IServerDriver_)
                            return;
                        auto msg = IServerDriver_->PrepareFrameChanged(img, monitor, ImageCompressionSettingActual, EncodeImagesAsGrayScale);
                        if (IServerDriver_->MemoryUsed() >= MaxPendingData) {
                            // decrease the capture rate to accomidate slow connections
                            std::this_thread::sleep_for(100ms);
                            ScreenImageCaptureRateActual += 100; // slower the capture rate ....
                            ScreenCaptureManager_->setFrameChangeInterval(std::chrono::milliseconds(ScreenImageCaptureRateActual));
                            ImageCompressionSettingActual -= 10; // decrease the quality of the image compression ....
                            if (ImageCompressionSettingActual < 30)
                                ImageCompressionSettingActual = 30; // anything below 30 will be pretty bad....
                            //  SL_RAT_LOG(RAT_Lite::Logging_Levels::INFO_log_level, "setFrameChangeInterval " << ScreenImageCaptureRateActual);
                        }
                        else if (IServerDriver_->MemoryUsed() < MaxPendingData &&
                                 (ScreenImageCaptureRateActual != ScreenImageCaptureRateRequested ||
                                  ImageCompressionSettingActual != ImageCompressionSettingRequested)) {
                            // increase the capture rate here
                            ScreenImageCaptureRateActual -= 100; // increase the capture rate ....
                            if (ScreenImageCaptureRateActual <= 0)
                                ScreenImageCaptureRateActual = ScreenImageCaptureRateRequested;
                            ScreenCaptureManager_->setFrameChangeInterval(std::chrono::milliseconds(ScreenImageCaptureRateActual));
                            ImageCompressionSettingActual += 10;
                            if (abs(ImageCompressionSettingActual - ImageCompressionSettingRequested) < 10 ||
                                ImageCompressionSettingActual >= ImageCompressionSettingRequested)
                                ImageCompressionSettingActual = ImageCompressionSettingRequested;
                            // SL_RAT_LOG(RAT_Lite::Logging_Levels::INFO_log_level, "setFrameChangeInterval " << ScreenImageCaptureRateActual);
                        }
                        SendtoAll(msg);
                    })
                    ->onMouseChanged([&](const SL::Screen_Capture::Image *img, const SL::Screen_Capture::Point &point) {
                        if (!IServerDriver_)
                            return;
                        if (img) {
                            SendtoAll(IServerDriver_->PrepareMouseImageChanged(*img));
                        }
                        SendtoAll(IServerDriver_->PrepareMousePositionChanged(point));
                    })
                    ->start_capturing();

            ScreenCaptureManager_->setMouseChangeInterval(std::chrono::milliseconds(MouseCaptureRate));
            ScreenCaptureManager_->setFrameChangeInterval(std::chrono::milliseconds(ScreenImageCaptureRateActual));
            ScreenCaptureManager_->pause();
        }

        virtual ~ServerImpl()
        {
            ScreenCaptureManager_.reset();
            Clipboard_.reset(); // make sure to prevent race conditions
            Status_ = Server_Status::SERVER_STOPPED;
        }
        void SendtoAll(const WS_LITE::WSMessage &msg)
        {
            std::lock_guard<std::mutex> lock(ClientsLock);
            for (const auto &a : Clients) {
                a->send(msg, false);
            }
        }
        void Run(unsigned short port, std::string PasswordToPrivateKey, std::string PathTo_Private_Key, std::string PathTo_Public_Certficate)
        {
            Status_ = Server_Status::SERVER_RUNNING;

            auto clientctx =
                SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))
                    ->NoTLS()
                    /*
                                        ->UseTLS(
                                            [&](SL::WS_LITE::ITLSContext *context) {
                                                context->set_options(SL::WS_LITE::options::default_workarounds | SL::WS_LITE::options::no_sslv2 |
                                                                     SL::WS_LITE::options::no_sslv3 | SL::WS_LITE::options::single_dh_use);
                                                std::error_code ec;

                                                context->set_password_callback(
                                                    [PasswordToPrivateKey](std::size_t s, SL::WS_LITE::password_purpose p) { return
                       PasswordToPrivateKey; }, ec); if (ec) { std::cout << "set_password_callback failed: " << ec.message(); ec.clear();
                                                }
                                                context->use_certificate_chain_file(PathTo_Public_Certficate, ec);
                                                if (ec) {
                                                    std::cout << "use_certificate_chain_file failed: " << ec.message();
                                                    ec.clear();
                                                }
                                                context->set_default_verify_paths(ec);
                                                if (ec) {
                                                    std::cout << "set_default_verify_paths failed: " << ec.message();
                                                    ec.clear();
                                                }
                                                context->use_private_key_file(std::string(PathTo_Private_Key), SL::WS_LITE::file_format::pem, ec);
                                                if (ec) {
                                                    std::cout << "use_private_key_file failed: " << ec.message();
                                                    ec.clear();
                                                }
                                            },
                                            SL::WS_LITE::method::tls_server)
                    */
                    ->CreateListener(port);
            IServerDriver_ =
                RAT_Lite::CreateServerDriverConfiguration()
                    ->onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket) {
                        {
                            std::lock_guard<std::mutex> lock(ClientsLock);
                            Clients.push_back(socket);
                        }
                        std::vector<int> ids;
                        auto p = Screen_Capture::GetMonitors();
                        for (auto &a : p) {
                            ids.push_back(Screen_Capture::Id(a));
                        }
                        socket->send(IServerDriver_->PrepareMonitorsChanged(p), false);
                        {
                            std::lock_guard<std::mutex> lock(ClientsThatNeedFullFramesLock);
                            ClientsThatNeedFullFrames.push_back({socket, ids});
                        }
                        ScreenCaptureManager_->resume();
                    })
                    ->onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const WS_LITE::WSMessage &msg) {
                        UNUSED(socket);
                        UNUSED(msg);
                    })
                    ->onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, unsigned short code, const std::string &msg) {
                        {
                            std::lock_guard<std::mutex> lock(ClientsLock);
                            Clients.erase(std::remove_if(std::begin(Clients), std::end(Clients), [&](auto &s) { return s == socket; }),
                                          std::end(Clients));
                            if (Clients.empty()) {
                                // make sure to stop capturing if isnt needed
                                ScreenCaptureManager_->pause();
                            }
                        }
                        { // make sure to remove any clients here too
                            std::lock_guard<std::mutex> lock(ClientsThatNeedFullFramesLock);
                            ClientsThatNeedFullFrames.erase(std::remove_if(std::begin(ClientsThatNeedFullFrames), std::end(ClientsThatNeedFullFrames),
                                                                           [&](auto &s) { return s.s.get() == socket.get(); }),
                                                            std::end(ClientsThatNeedFullFrames));
                        }
                        UNUSED(code);
                        UNUSED(msg);
                    })
                    ->onKeyUp([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::KeyCodes key) {
                        if (!IgnoreIncomingKeyboardEvents) {
                            Input_Lite::KeyEvent kevent;
                            kevent.Key = key;
                            kevent.Pressed = false;
                            Input_Lite::SendInput(kevent);
                        }
                    })
                    ->onKeyDown([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::KeyCodes key) {
                        if (!IgnoreIncomingKeyboardEvents) {
                            Input_Lite::KeyEvent kevent;
                            kevent.Key = key;
                            kevent.Pressed = true;
                            Input_Lite::SendInput(kevent);
                        }
                    })
                    ->onMouseUp([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::MouseButtons button) {
                        if (!IgnoreIncomingMouseEvents) {
                            Input_Lite::MouseButtonEvent mbevent;
                            mbevent.Pressed = false;
                            mbevent.Button = button;
                            Input_Lite::SendInput(mbevent);
                        }
                    })
                    ->onMouseDown([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::MouseButtons button) {
                        if (!IgnoreIncomingMouseEvents) {
                            Input_Lite::MouseButtonEvent mbevent;
                            mbevent.Pressed = true;
                            mbevent.Button = button;
                            Input_Lite::SendInput(mbevent);
                        }
                    })
                    ->onMouseScroll([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, int offset) {
                        if (!IgnoreIncomingMouseEvents) {
                            Input_Lite::MouseScrollEvent mbevent;
                            mbevent.Offset = offset;
                            Input_Lite::SendInput(mbevent);
                        }
                    })
                    ->onMousePosition([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, const RAT_Lite::Point &pos) {
                        if (!IgnoreIncomingMouseEvents) {
                            Input_Lite::MousePositionAbsoluteEvent mbevent;
                            mbevent.X = pos.X;
                            mbevent.Y = pos.Y;
                            Input_Lite::SendInput(mbevent);
                        }
                    })
                    ->onClipboardChanged([&](const std::string &text) {
                        SL_RAT_LOG(RAT_Lite::Logging_Levels::INFO_log_level, "onClipboardChanged " << text.size());
                        Clipboard_->copy(text);
                    })
                    ->Build(clientctx);
            clientctx->listen();
        }
    }; // namespace RAT

    Server::Server() { ServerImpl_ = new ServerImpl(); }
    Server::~Server() { delete ServerImpl_; }
    void Server::ShareClipboard(bool share) { ServerImpl_->ShareClip = share; }
    bool Server::ShareClipboard() const { return ServerImpl_->ShareClip; }
    void Server::MaxConnections(int maxconnections)
    {
        if (ServerImpl_->IServerDriver_) {
            ServerImpl_->IServerDriver_->MaxConnections(maxconnections);
        }
    }
    int Server::MaxConnections() const
    {
        if (ServerImpl_->IServerDriver_) {
            return ServerImpl_->IServerDriver_->MaxConnections();
        }
        return 0;
    }
    void Server::FrameChangeInterval(int delay_in_ms)
    {
        ServerImpl_->ScreenImageCaptureRateRequested = ServerImpl_->ScreenImageCaptureRateActual = delay_in_ms;
        if (ServerImpl_->IServerDriver_) {
            ServerImpl_->ScreenCaptureManager_->setFrameChangeInterval(std::chrono::milliseconds(delay_in_ms));
        }
    }
    int Server::FrameChangeInterval() const { return ServerImpl_->ScreenImageCaptureRateRequested; }
    void Server::MouseChangeInterval(int delay_in_ms)
    {
        ServerImpl_->MouseCaptureRate = delay_in_ms;
        if (ServerImpl_->IServerDriver_) {
            ServerImpl_->ScreenCaptureManager_->setMouseChangeInterval(std::chrono::milliseconds(delay_in_ms));
        }
    }
    int Server::MouseChangeInterval() const { return ServerImpl_->MouseCaptureRate; }
    void Server::ImageCompressionSetting(int compression)
    {
        ServerImpl_->ImageCompressionSettingRequested = ServerImpl_->ImageCompressionSettingActual = compression;
    }
    int Server::ImageCompressionSetting() const { return ServerImpl_->ImageCompressionSettingRequested; }
    void Server::EncodeImagesAsGrayScale(bool usegrayscale) { ServerImpl_->EncodeImagesAsGrayScale = usegrayscale; }
    bool Server::EncodeImagesAsGrayScale() const { return ServerImpl_->EncodeImagesAsGrayScale; }

    void Server::Server::Run(unsigned short port, std::string PasswordToPrivateKey, std::string PathTo_Private_Key,
                             std::string PathTo_Public_Certficate)
    {
        ServerImpl_->Run(port, PasswordToPrivateKey, PathTo_Private_Key, PathTo_Public_Certficate);
        while (ServerImpl_->Status_ == Server_Status::SERVER_RUNNING) {
            std::this_thread::sleep_for(50ms);
        }
    }

#if __ANDROID__
    void Server::Server::OnImage(char *buf, int width, int height)
    {
        return _ServerImpl->OnScreen(Utilities::Image::CreateImage(height, width, buf, 4));
    }
#endif

} // namespace RAT_Server
} // namespace SL
