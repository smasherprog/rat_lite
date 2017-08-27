#include "Server.h"
#include <assert.h>
#include <mutex>
#include <string.h>
#include <thread>

#include "Clipboard_Lite.h"
#include "Configs.h"
#include "IServerDriver.h"
#include "Input_Lite.h"
#include "Logging.h"
#include "ScreenCapture.h"
#include "ServerDriver.h"
#include "Shapes.h"

#include "WS_Lite.h"
using namespace std::chrono_literals;

namespace SL {
namespace RAT {
    struct NewClient {
        std::shared_ptr<WS_LITE::IWSocket> s;
        std::vector<int> mids;
    };
    class ServerImpl : public IServerDriver {
      public:
        std::shared_ptr<Screen_Capture::ScreenCaptureManager> ScreenCaptureManager_;

        ServerDriver ServerDriver_;
        std::shared_ptr<Clipboard_Lite::Clipboard_Manager> Clipboard_;

        Server_Status Status_ = Server_Status::SERVER_STOPPED;
        std::shared_ptr<Server_Config> Config_;

        std::mutex ClientsLock;
        std::vector<std::shared_ptr<WS_LITE::IWSocket>> Clients;

        std::mutex ClientsThatNeedFullFramesLock;
        std::vector<NewClient> ClientsThatNeedFullFrames;

        ServerImpl(std::shared_ptr<Server_Config> config) : ServerDriver_(this, config), Config_(config)
        {
            Status_ = Server_Status::SERVER_RUNNING;

            Clipboard_ = Clipboard_Lite::CreateClipboard()
                             ->onText([&](const std::string &text) {
                                 if (Config_->Share_Clipboard) {
                                     SendtoAll(ServerDriver_.PrepareClipboardChanged(text));
                                 }
                             })
                             ->run();
            ScreenCaptureManager_ =
                Screen_Capture::CreateCaptureConfiguration([&]() {
                    auto monitors = Screen_Capture::GetMonitors();
                    SendtoAll(ServerDriver_.PrepareMonitorsChanged(monitors));
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
                            auto msg = ServerDriver_.PrepareNewFrame(img, monitor);
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
                            // erase any clients from the list that have no more monitors that they need
                            ClientsThatNeedFullFrames.erase(std::remove_if(begin(ClientsThatNeedFullFrames), end(ClientsThatNeedFullFrames),
                                                                           [&](const auto i) { return i.mids.empty(); }),
                                                            end(ClientsThatNeedFullFrames));
                        }
                    })
                    ->onFrameChanged([&](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor) {
                        SendtoAll(ServerDriver_.PrepareFrameChanged(img, monitor));
                    })
                    ->onMouseChanged([&](const SL::Screen_Capture::Image *img, const SL::Screen_Capture::Point &point) {
                        if (img) {
                            SendtoAll(ServerDriver_.PrepareMouseImageChanged(*img));
                        }
                        SendtoAll(ServerDriver_.PrepareMousePositionChanged(point));
                    })
                    ->start_capturing();

            ScreenCaptureManager_->setMouseChangeInterval(std::chrono::milliseconds(Config_->MousePositionCaptureRate));
            ScreenCaptureManager_->setFrameChangeInterval(std::chrono::milliseconds(Config_->ScreenImageCaptureRate));
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

        virtual void onConnection(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket) override
        {
            {
                std::lock_guard<std::mutex> lock(ClientsLock);
                Clients.push_back(socket);
            }
            std::vector<int> ids;
            auto p = Screen_Capture::GetMonitors();
            for (auto &a : p) {
                ids.push_back(Screen_Capture::Id(a));
            }
            socket->send(ServerDriver_.PrepareMonitorsChanged(p), false);
            {
                std::lock_guard<std::mutex> lock(ClientsThatNeedFullFramesLock);
                ClientsThatNeedFullFrames.push_back({socket, ids});
            }
            ScreenCaptureManager_->resume();
        }
        virtual void onMessage(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const WS_LITE::WSMessage &msg) override
        {
            UNUSED(socket);
            UNUSED(msg);
        }
        virtual void onDisconnection(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, unsigned short code, const std::string &msg) override
        {
            {
                std::lock_guard<std::mutex> lock(ClientsLock);
                Clients.erase(std::remove_if(std::begin(Clients), std::end(Clients), [&](auto &s) { return s == socket; }), std::end(Clients));
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
        }
        virtual void onKeyUp(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::KeyCodes key) override
        {
            if (!Config_->IgnoreIncomingKeyboardEvents) {
                Input_Lite::KeyEvent kevent;
                kevent.Key = key;
                kevent.Pressed = false;
                Input_Lite::SendInput(kevent);
            }
        }

        virtual void onKeyDown(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::KeyCodes key) override
        {
            if (!Config_->IgnoreIncomingKeyboardEvents) {
                Input_Lite::KeyEvent kevent;
                kevent.Key = key;
                kevent.Pressed = true;
                Input_Lite::SendInput(kevent);
            }
        }

        virtual void onMouseUp(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::MouseButtons button) override
        {
            if (!Config_->IgnoreIncomingMouseEvents) {
                Input_Lite::MouseButtonEvent mbevent;
                mbevent.Pressed = false;
                mbevent.Button = button;
                Input_Lite::SendInput(mbevent);
            }
        }
        virtual void onMouseDown(const std::shared_ptr<WS_LITE::IWSocket> &socket, Input_Lite::MouseButtons button) override
        {
            if (!Config_->IgnoreIncomingMouseEvents) {
                Input_Lite::MouseButtonEvent mbevent;
                mbevent.Pressed = true;
                mbevent.Button = button;
                Input_Lite::SendInput(mbevent);
            }
        }
        virtual void onMouseScroll(const std::shared_ptr<WS_LITE::IWSocket> &socket, int offset) override
        {
            if (!Config_->IgnoreIncomingMouseEvents) {
                Input_Lite::MouseScrollEvent mbevent;
                mbevent.Offset = offset;
                Input_Lite::SendInput(mbevent);
            }
        }
        virtual void onMousePosition(const std::shared_ptr<WS_LITE::IWSocket> &socket, const Point &pos) override
        {
            if (!Config_->IgnoreIncomingMouseEvents) {
                Input_Lite::MousePositionAbsoluteEvent mbevent;
                mbevent.X = pos.X;
                mbevent.Y = pos.Y;
                Input_Lite::SendInput(mbevent);
            }
        }
        virtual void onClipboardChanged(const std::string &text) override
        {
            SL_RAT_LOG(Logging_Levels::INFO_log_level, "onClipboardChanged " << text.size());
            Clipboard_->copy(text);
        }
    }; // namespace RAT

    Server::Server(std::shared_ptr<Server_Config> config) { ServerImpl_ = std::make_shared<ServerImpl>(config); }
    Server::~Server() {}
    void Server::Server::Run()
    {
        // ServerImpl_->ServerDriver_
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
