#include "Server.h"
#include <thread>
#include <string.h>
#include <assert.h>
#include <mutex>

#include "ScreenCapture.h"
#include "ServerDriver.h"
#include "IServerDriver.h"
#include "Configs.h"
#include "Shapes.h"
#include "Input_Lite.h"
#include "Clipboard_Lite.h"
#include "Logging.h"

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

            Screen_Capture::ScreenCaptureManager ScreenCaptureManager_;

            ServerDriver ServerDriver_;
            Clipboard_Lite::Clipboard_Manager Clipboard_;

            Server_Status Status_ = Server_Status::SERVER_STOPPED;
            std::shared_ptr<Server_Config> Config_;

            std::mutex ClientsThatNeedFullFramesLock;
            std::vector<NewClient> ClientsThatNeedFullFrames;

            ServerImpl(std::shared_ptr<Server_Config> config) :ServerDriver_(this, config), Config_(config)
            {
                Status_ = Server_Status::SERVER_RUNNING;

                Clipboard_ = Clipboard_Lite::CreateClipboard().onText([&](const std::string& text) {
                    if (Config_->Share_Clipboard) {
                        ServerDriver_.SendClipboardChanged(text);
                    }

                }).run();
                ScreenCaptureManager_ = Screen_Capture::CreateScreeCapture([&]() {
                    auto p = Screen_Capture::GetMonitors();

                    ServerDriver_.SendMonitorsChanged(p);
                    //rebuild any ids
                    std::lock_guard<std::mutex> lock(ClientsThatNeedFullFramesLock);
                    std::vector<NewClient> newclients;
                    for (auto& old : ClientsThatNeedFullFrames) {
                        std::vector<int> ids;
                        for (auto& a : p) {
                            ids.push_back(Screen_Capture::Id(a));
                        }
                        newclients.push_back({ old.s, ids });
                    }
                    ClientsThatNeedFullFrames = newclients;
                    return p;
                }).onNewFrame([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor) {
                    if (!ClientsThatNeedFullFrames.empty()) {
                        std::lock_guard<std::mutex> lock(ClientsThatNeedFullFramesLock);
                        for (auto& a : ClientsThatNeedFullFrames) {
                            a.mids.erase(std::remove_if(begin(a.mids), end(a.mids), [&](const auto i) {  return i == Screen_Capture::Id(monitor);  }), end(a.mids));
                            ServerDriver_.SendNewFrame(a.s, img, monitor);
                        }
                        ClientsThatNeedFullFrames.erase(std::remove_if(begin(ClientsThatNeedFullFrames), end(ClientsThatNeedFullFrames), [&](const auto i) {  return i.mids.empty(); }), end(ClientsThatNeedFullFrames));
                    }
                }).onFrameChanged([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor) {
                    ServerDriver_.SendFrameChanged(img, monitor);
                }).onMouseChanged([&](const SL::Screen_Capture::Image* img, int x, int y) {
                    if (img) {
                        ServerDriver_.SendMouseImageChanged(*img);
                    }
                    ServerDriver_.SendMousePositionChanged(Point(x, y));
                }).start_capturing();

                ScreenCaptureManager_.setMouseChangeInterval(std::chrono::milliseconds(Config_->MousePositionCaptureRate));
                ScreenCaptureManager_.setFrameChangeInterval(std::chrono::milliseconds(Config_->ScreenImageCaptureRate));

            }

            virtual ~ServerImpl() {
                ScreenCaptureManager_.destroy();
                Clipboard_.destroy();//make sure to prevent race conditions
                Status_ = Server_Status::SERVER_STOPPED;
            }

            virtual void onConnection(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket)override {
                UNUSED(socket);
                std::vector<int> ids;
                auto p = Screen_Capture::GetMonitors();
                for (auto& a : p) {
                    ids.push_back(Screen_Capture::Id(a));
                }
                ServerDriver_.SendMonitorsChanged(socket, p);

                std::lock_guard<std::mutex> lock(ClientsThatNeedFullFramesLock);
                ClientsThatNeedFullFrames.push_back({ socket, ids });
            }
            virtual void onMessage(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const WS_LITE::WSMessage& msg) override {
                UNUSED(socket);
                UNUSED(msg);
            }
            virtual void onDisconnection(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) override {
                UNUSED(socket);
                UNUSED(code);
                UNUSED(msg);
            }
            virtual void onKeyUp(const std::shared_ptr<WS_LITE::IWSocket>& socket, char key) override {
                if (!Config_->IgnoreIncomingKeyboardEvents) Input_Lite::SendKeyUp(key);
            }
            virtual void onKeyUp(const std::shared_ptr<WS_LITE::IWSocket>& socket, wchar_t key) override {
                if (!Config_->IgnoreIncomingKeyboardEvents) Input_Lite::SendKeyUp(key);
            }
            virtual void onKeyUp(const std::shared_ptr<WS_LITE::IWSocket>& socket, Input_Lite::SpecialKeyCodes key) override {
                if (!Config_->IgnoreIncomingKeyboardEvents) Input_Lite::SendKeyUp(key);
            }

            virtual void onKeyDown(const std::shared_ptr<WS_LITE::IWSocket>& socket, char key) override {
                if (!Config_->IgnoreIncomingKeyboardEvents) Input_Lite::SendKeyDown(key);
            }
            virtual void onKeyDown(const std::shared_ptr<WS_LITE::IWSocket>& socket, wchar_t key) override {
                if (!Config_->IgnoreIncomingKeyboardEvents) Input_Lite::SendKeyDown(key);
            }
            virtual void onKeyDown(const std::shared_ptr<WS_LITE::IWSocket>& socket, Input_Lite::SpecialKeyCodes key) override {
                if (!Config_->IgnoreIncomingKeyboardEvents) Input_Lite::SendKeyDown(key);
            }

            virtual void onMouseUp(const std::shared_ptr<WS_LITE::IWSocket>& socket, Input_Lite::MouseButtons button) override {
                if (!Config_->IgnoreIncomingMouseEvents) Input_Lite::SendMouseUp(button);
            }
            virtual void onMouseDown(const std::shared_ptr<WS_LITE::IWSocket>& socket, Input_Lite::MouseButtons button) override {
                if (!Config_->IgnoreIncomingMouseEvents) Input_Lite::SendMouseDown(button);
            }
            virtual void onMouseScroll(const std::shared_ptr<WS_LITE::IWSocket>& socket, int offset) override {
                if (!Config_->IgnoreIncomingMouseEvents) Input_Lite::SendMouseScroll(offset);
            }
            virtual void onMousePosition(const std::shared_ptr<WS_LITE::IWSocket>& socket, const Point& pos) override {
 
            }
            virtual void onClipboardChanged(const std::string& text) override {
                SL_RAT_LOG(Logging_Levels::INFO_log_level, "onClipboardChanged " << text.size());
                Clipboard_.copy(text);
            }
        };

        Server::Server(std::shared_ptr<Server_Config> config)
        {
            ServerImpl_ = std::make_shared<ServerImpl>(config);
        }
        Server::~Server()
        {

        }
        void Server::Server::Run()
        {
           // ServerImpl_->ServerDriver_
            while (ServerImpl_->Status_ != Server_Status::SERVER_RUNNING) {
                std::this_thread::sleep_for(50ms);
            }
        }

#if __ANDROID__
        void Server::Server::OnImage(char* buf, int width, int height)
        {
            return _ServerImpl->OnScreen(Utilities::Image::CreateImage(height, width, buf, 4));
        }
#endif

    }
}
