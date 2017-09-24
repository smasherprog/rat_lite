#include "Server.h"
#include <assert.h>
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
namespace RAT {
    struct NewClient {
        std::shared_ptr<WS_LITE::IWSocket> s;
        std::vector<int> mids;
    };
    class ServerImpl {
      public:
        std::shared_ptr<Screen_Capture::ScreenCaptureManager> ScreenCaptureManager_;
        std::shared_ptr<Clipboard_Lite::Clipboard_Manager> Clipboard_;
        std::shared_ptr<RAT::IServerDriver> IServerDriver_;

        Server_Status Status_ = Server_Status::SERVER_STOPPED;
        std::mutex ClientsLock;
        std::vector<std::shared_ptr<WS_LITE::IWSocket>> Clients;

        std::mutex ClientsThatNeedFullFramesLock;
        std::vector<NewClient> ClientsThatNeedFullFrames;

        bool ShareClip = false;
        int MaxNumConnections = 10;
        int ImageCompressionSetting = 70;
        int MouseCaptureRate = 50;
        int ScreenImageCaptureRate = 10;
        bool IgnoreIncomingKeyboardEvents = false;
        bool IgnoreIncomingMouseEvents = false;

        ServerImpl()
        {
            Status_ = Server_Status::SERVER_RUNNING;

            Clipboard_ = Clipboard_Lite::CreateClipboard()
                             ->onText([&](const std::string &text) {
                                 if (ShareClip) {
                                     SendtoAll(RAT::IServerDriver::PrepareClipboardChanged(text));
                                 }
                             })
                             ->run();

            ScreenCaptureManager_ =
                Screen_Capture::CreateCaptureConfiguration([&]() {
                    auto monitors = Screen_Capture::GetMonitors();
                    SendtoAll(RAT::IServerDriver::PrepareMonitorsChanged(monitors));
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
                        if (!ClientsThatNeedFullFrames.empty()) {
                            auto msg = RAT::IServerDriver::PrepareNewFrame(img, monitor);
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
                        SendtoAll(RAT::IServerDriver::PrepareFrameChanged(img, monitor));
                    })
                    ->onMouseChanged([&](const SL::Screen_Capture::Image *img, const SL::Screen_Capture::Point &point) {
                        if (img) {
                            SendtoAll(RAT::IServerDriver::PrepareMouseImageChanged(*img));
                        }
                        SendtoAll(RAT::IServerDriver::PrepareMousePositionChanged(point));
                    })
                    ->start_capturing();

            ScreenCaptureManager_->setMouseChangeInterval(std::chrono::milliseconds(MouseCaptureRate));
            ScreenCaptureManager_->setFrameChangeInterval(std::chrono::milliseconds(ScreenImageCaptureRate));
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
            auto clientctx = SL::WS_LITE::CreateContext(SL::WS_LITE::ThreadCount(1))->NoTLS()->CreateListener(port);
            IServerDriver_ =
                RAT::CreateServerDriverConfiguration()
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
                        socket->send(RAT::IServerDriver::PrepareMonitorsChanged(p), false);
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
                    ->onMousePosition([&](const std::shared_ptr<WS_LITE::IWSocket> &socket, const Point &pos) {
                        if (!IgnoreIncomingMouseEvents) {
                            Input_Lite::MousePositionAbsoluteEvent mbevent;
                            mbevent.X = pos.X;
                            mbevent.Y = pos.Y;
                            Input_Lite::SendInput(mbevent);
                        }
                    })
                    ->onClipboardChanged([&](const std::string &text) {
                        SL_RAT_LOG(Logging_Levels::INFO_log_level, "onClipboardChanged " << text.size());
                        Clipboard_->copy(text);
                    })
                    ->Build(clientctx);
        }
    }; // namespace RAT

    Server::Server() { ServerImpl_ = new ServerImpl(); }
    Server::~Server() { delete ServerImpl_; }
    void Server::ShareClipboard(bool share) { ServerImpl_->ShareClip = share; }
    bool Server::ShareClipboard() const { return ServerImpl_->ShareClip; }
    void Server::MaxConnections(int maxconnections) { ServerImpl_->MaxNumConnections = maxconnections; }
    int Server::MaxConnections() const { return ServerImpl_->MaxNumConnections; }
    void Server::FrameChangeInterval(int delay_in_ms) { ServerImpl_->ScreenImageCaptureRate = delay_in_ms; }
    int Server::FrameChangeInterval() const { return ServerImpl_->ScreenImageCaptureRate; }
    void Server::MouseChangeInterval(int delay_in_ms) { ServerImpl_->MouseCaptureRate = delay_in_ms; }
    int Server::MouseChangeInterval() const { return ServerImpl_->MouseCaptureRate; }
    void Server::ImageCompressionSetting(int compression) { ServerImpl_->ImageCompressionSetting = compression; }
    int Server::ImageCompressionSetting() const { return ServerImpl_->ImageCompressionSetting; }

    void Server::Server::Run(unsigned short port, std::string PasswordToPrivateKey, std::string PathTo_Private_Key,
                             std::string PathTo_Public_Certficate)
    {
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

} // namespace RAT
} // namespace SL
