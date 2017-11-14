#include "Server.h"
#include <assert.h>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <string.h>
#include <thread>
#include <vector>

#include "Clipboard_Lite.h"
#include "Logging.h"
#include "RAT.h"
#include "ScreenCapture.h"

#include "WS_Lite.h"

#include "ServerFunctions.h"

using namespace std::chrono_literals;

namespace SL {
namespace RAT_Server {
    class ServerImpl {
      public:
        std::shared_ptr<Screen_Capture::IScreenCaptureManager> ScreenCaptureManager_;
        std::shared_ptr<Clipboard_Lite::IClipboard_Manager> Clipboard_;
        std::shared_ptr<RAT_Lite::IServerDriver> IServerDriver_;

        RAT_Lite::Server_Status Status_ = RAT_Lite::Server_Status::SERVER_STOPPED;
        std::shared_mutex ClientsLock;
        std::vector<std::shared_ptr<Client>> Clients;
        std::vector<Screen_Capture::Monitor> AllMonitors;

        RAT_Lite::ClipboardSharing ShareClip = RAT_Lite::ClipboardSharing::NOT_SHARED;
        int ImageCompressionSettingRequested = 70;
        int ImageCompressionSettingActual = 70;
        int MouseCaptureRate = 50;
        int ScreenImageCaptureRateRequested = 100;
        int ScreenImageCaptureRateActual = 100;

        bool IgnoreIncomingKeyboardEvents = false;
        bool IgnoreIncomingMouseEvents = false;
        RAT_Lite::ImageEncoding EncodeImagesAsGrayScale = RAT_Lite::ImageEncoding::COLOR;
        size_t MaxMemoryUsed = 1024 * 1024 * 100;

        ServerImpl()
        {
            Status_ = RAT_Lite::Server_Status::SERVER_STOPPED;
            Clipboard_ = Clipboard_Lite::CreateClipboard()
                             ->onText([&](const std::string &text) {
                                 if (ShareClip == RAT_Lite::ClipboardSharing::SHARED && IServerDriver_) {
                                     SendtoAll(IServerDriver_->PrepareClipboardChanged(text));
                                 }
                             })
                             ->run();

            ScreenCaptureManager_ =
                Screen_Capture::CreateCaptureConfiguration([&]() {
                    AllMonitors = Screen_Capture::GetMonitors();
                    if (!IServerDriver_)
                        return AllMonitors;
                    SendtoAll(IServerDriver_->PrepareMonitorsChanged(AllMonitors));
                    // add everyone to the list!

                    std::unique_lock<std::shared_mutex> lock(ClientsLock);
                    onGetMonitors(Clients, AllMonitors);
                    return AllMonitors;
                })
                    ->onNewFrame([&](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor) {
                        if (!IServerDriver_)
                            return;
                        decltype(Clients) clients;
                        clients.reserve(Clients.size());
                        {
                            std::shared_lock<std::shared_mutex> lock(ClientsLock);
                            for (auto &a : Clients) {
                                auto found =
                                    std::find_if(begin(a->MonitorsNeeded), end(a->MonitorsNeeded), [&monitor](auto m) { return monitor.Id == m.Id; });
                                if (found != end(a->MonitorsNeeded)) {
                                    clients.push_back(a);
                                    a->MonitorsNeeded.erase(found);
                                }
                            }
                        }
                        for (auto &a : clients) {
                            auto msg = IServerDriver_->PrepareNewFrame(img, monitor, ImageCompressionSettingActual,
                                                                       EncodeImagesAsGrayScale == RAT_Lite::ImageEncoding::GRAYSCALE ||
                                                                           a->EncodeImagesAsGrayScale == RAT_Lite::ImageEncoding::GRAYSCALE);
                            a->Socket->send(msg, SL::WS_LITE::CompressionOptions::NO_COMPRESSION);
                        }
                    })
                    ->onFrameChanged([&](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor) {
                        if (!IServerDriver_)
                            return;

                        decltype(Clients) clients;
                        clients.reserve(Clients.size());
                        {
                            std::shared_lock<std::shared_mutex> lock(ClientsLock);
                            for (auto &a : Clients) {
                                auto found = std::find_if(begin(a->MonitorsToWatch), end(a->MonitorsToWatch),
                                                          [&monitor](auto m) { return monitor.Id == m.Id; });
                                if (found != end(a->MonitorsToWatch)) {
                                    clients.push_back(a);
                                }
                            }
                        }

                        auto newscreencapturerate = GetNewScreenCaptureRate(IServerDriver_->MemoryUsed(), MaxMemoryUsed, ScreenImageCaptureRateActual,
                                                                            ScreenImageCaptureRateRequested);
                        if (newscreencapturerate != ScreenImageCaptureRateActual) {
                            ScreenImageCaptureRateActual = newscreencapturerate;
                            ScreenCaptureManager_->setFrameChangeInterval(std::chrono::milliseconds(newscreencapturerate));
                        }
                        ImageCompressionSettingActual = GetNewImageCompression(IServerDriver_->MemoryUsed(), MaxMemoryUsed,
                                                                               ImageCompressionSettingActual, ImageCompressionSettingRequested);
                        for (auto &a : clients) {
                            auto msg = IServerDriver_->PrepareFrameChanged(img, monitor, ImageCompressionSettingActual,
                                                                           EncodeImagesAsGrayScale == RAT_Lite::ImageEncoding::GRAYSCALE ||
                                                                               a->EncodeImagesAsGrayScale == RAT_Lite::ImageEncoding::GRAYSCALE);
                            a->Socket->send(msg, SL::WS_LITE::CompressionOptions::NO_COMPRESSION);
                        }

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
            Status_ = RAT_Lite::Server_Status::SERVER_STOPPED;
        }
        void SendtoAll(const WS_LITE::WSMessage &msg)
        {
            std::shared_lock<std::shared_mutex> lock(ClientsLock);
            for (const auto &a : Clients) {
                a->Socket->send(msg, SL::WS_LITE::CompressionOptions::NO_COMPRESSION);
            }
        }
        void Run(unsigned short port, std::string PasswordToPrivateKey, std::string PathTo_Private_Key, std::string PathTo_Public_Certficate)
        {
            Status_ = RAT_Lite::Server_Status::SERVER_RUNNING;

            auto clientctx = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))

                                 ->NoTLS()
/*
                                 ->UseTLS(
                                     [&](SL::WS_LITE::ITLSContext *context) {
                                         context->set_options(SL::WS_LITE::options::default_workarounds | SL::WS_LITE::options::no_sslv2 |
                                                              SL::WS_LITE::options::no_sslv3 | SL::WS_LITE::options::single_dh_use);
                                         std::error_code ec;

                                         context->set_password_callback(
                                             [PasswordToPrivateKey](std::size_t s, SL::WS_LITE::password_purpose p) { return PasswordToPrivateKey; },
                                             ec);
                                         if (ec) {
                                             std::cout << "set_password_callback failed: " << ec.message();
                                             ec.clear();
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
                                     SL::WS_LITE::method::tlsv11_server)*/


                                 ->CreateListener(port);
            IServerDriver_ =
                RAT_Lite::CreateServerDriverConfiguration()
                    ->onConnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket) {
                        auto c = std::make_shared<Client>();
                        c->Socket = socket;
                        auto m = Screen_Capture::GetMonitors();
                        c->MonitorsNeeded = m;
                        c->MonitorsToWatch = m;
                        socket->send(IServerDriver_->PrepareMonitorsChanged(m), SL::WS_LITE::CompressionOptions::NO_COMPRESSION);
                        std::unique_lock<std::shared_mutex> lock(ClientsLock);
                        Clients.push_back(c);
                        ScreenCaptureManager_->resume();
                    })
                    ->onMessage([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const WS_LITE::WSMessage &msg) {
                        UNUSED(socket);
                        UNUSED(msg);
                    })
                    ->onDisconnection([&](const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, unsigned short code, const std::string &msg) {
                        std::unique_lock<std::shared_mutex> lock(ClientsLock);
                        Clients.erase(std::remove_if(std::begin(Clients), std::end(Clients), [&](auto &s) { return s->Socket == socket; }),
                                      std::end(Clients));
                        if (Clients.empty()) {
                            // make sure to stop capturing if isnt needed
                            ScreenCaptureManager_->pause();
                        }
                        UNUSED(code);
                        UNUSED(msg);
                    })
                    ->onKeyUp([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::KeyCodes key) {
                        onKeyUp(IgnoreIncomingKeyboardEvents, socket, key);
                    })
                    ->onKeyDown([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::KeyCodes key) {
                        onKeyDown(IgnoreIncomingKeyboardEvents, socket, key);
                    })
                    ->onMouseUp([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::MouseButtons button) {
                        onMouseUp(IgnoreIncomingMouseEvents, socket, button);
                    })
                    ->onMouseDown([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::MouseButtons button) {
                        onMouseDown(IgnoreIncomingMouseEvents, socket, button);
                    })
                    ->onMouseScroll([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, int offset) {
                        onMouseScroll(IgnoreIncomingMouseEvents, socket, offset);
                    })
                    ->onMousePosition([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, const RAT_Lite::Point &pos) {
                        onMousePosition(IgnoreIncomingMouseEvents, socket, pos);
                    })
                    ->onClipboardChanged(
                        [&](const std::string &text) { onClipboardChanged(ShareClip == RAT_Lite::ClipboardSharing::SHARED, text, Clipboard_); })
                    ->onClientSettingsChanged([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, const RAT_Lite::ClientSettings &clientsettings) {
                        std::unique_lock<std::shared_mutex> lock(ClientsLock);
                        onClientSettingsChanged(socket.get(), Clients, AllMonitors, clientsettings);
                    })
                    ->Build(clientctx);
            clientctx->listen();
        }
    }; // namespace RAT

    Server::Server() { ServerImpl_ = new ServerImpl(); }
    Server::~Server() { delete ServerImpl_; }
    void Server::ShareClipboard(RAT_Lite::ClipboardSharing share) { ServerImpl_->ShareClip = share; }
    RAT_Lite::ClipboardSharing Server::ShareClipboard() const { return ServerImpl_->ShareClip; }
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
    void Server::EncodeImagesAsGrayScale(RAT_Lite::ImageEncoding usegrayscale) { ServerImpl_->EncodeImagesAsGrayScale = usegrayscale; }
    RAT_Lite::ImageEncoding Server::EncodeImagesAsGrayScale() const { return ServerImpl_->EncodeImagesAsGrayScale; }

    void Server::Server::Run(unsigned short port, std::string PasswordToPrivateKey, std::string PathTo_Private_Key,
                             std::string PathTo_Public_Certficate)
    {
        ServerImpl_->Run(port, PasswordToPrivateKey, PathTo_Private_Key, PathTo_Public_Certficate);
        while (ServerImpl_->Status_ == RAT_Lite::Server_Status::SERVER_RUNNING) {
            std::this_thread::sleep_for(50ms);
        }
    }

} // namespace RAT_Server
} // namespace SL
