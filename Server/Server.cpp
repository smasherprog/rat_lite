#include "Server.h"
#include <thread>
#include <string.h>
#include <assert.h>
#include <mutex>

#include "ScreenCapture.h"
#include "ServerDriver.h"
#include "IServerDriver.h"
#include "Clipboard.h"
#include "Configs.h"
#include "Shapes.h"
#include "Input.h"
#include "IWebSocket.h"
#include "Logging.h"

namespace SL {
	namespace RAT {
		struct NewClient {
			std::shared_ptr<IWebSocket> s;
			std::vector<int> mids;
		};
		class ServerImpl : public IServerDriver {
		public:

			SL::Screen_Capture::ScreenCaptureManager ScreenCaptureManager_;

			ServerDriver ServerDriver_;
			std::unique_ptr<Clipboard> Clipboard_;

			Server_Status _Status = Server_Status::SERVER_STOPPED;
			std::shared_ptr<Server_Config> Config_;

			std::mutex ClientsThatNeedFullFramesLock;
			std::vector<NewClient> ClientsThatNeedFullFrames;

			ServerImpl(std::shared_ptr<Server_Config> config) :
				ServerDriver_(), Config_(config)
			{
				_Status = Server_Status::SERVER_RUNNING;

				Clipboard_ = std::make_unique<Clipboard>();
				Clipboard_->shareClipboard(Config_->Share_Clipboard);
				Clipboard_->onChange([&](const char* c, int len) { ServerDriver_.SendClipboardText(nullptr, c, static_cast<unsigned int>(len)); });

				ScreenCaptureManager_.setMouseChangeInterval(Config_->MousePositionCaptureRate);
				ScreenCaptureManager_.setFrameChangeInterval(Config_->ScreenImageCaptureRate);
				ScreenCaptureManager_.setMonitorsToCapture([&]() {
					auto p = Screen_Capture::GetMonitors();

					ServerDriver_.SendMonitorInfo(nullptr, p);
					//rebuild any ids
					std::lock_guard<std::mutex> lock(ClientsThatNeedFullFramesLock);
					std::vector<NewClient> newclients;
					for (auto& old : ClientsThatNeedFullFrames) {
						std::vector<int> ids;
						for (auto& a : p) {
							ids.push_back(Screen_Capture::Id(*a));
						}
						newclients.push_back({old.s, ids});
					}
					ClientsThatNeedFullFrames = newclients;
					return p;
				});
				ScreenCaptureManager_.onMouseChanged([&](const SL::Screen_Capture::Image* img, int x, int y) {
					if (img) {
						ServerDriver_.SendMouse(nullptr, *img);
					}
					ServerDriver_.SendMouse(nullptr, Point(x, y));
				});
				ScreenCaptureManager_.onFrameChanged([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor) {
					ServerDriver_.SendFrameChange(nullptr, img, monitor);
				});
				ScreenCaptureManager_.onNewFrame([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor) {
					if (!ClientsThatNeedFullFrames.empty()) {
						std::lock_guard<std::mutex> lock(ClientsThatNeedFullFramesLock);
						for (auto& a : ClientsThatNeedFullFrames) {
							a.mids.erase(std::remove_if(begin(a.mids), end(a.mids), [&](const auto i) {  return i == Screen_Capture::Id(monitor);  }), end(a.mids));
							ServerDriver_.SendFrameChange(a.s.get(), img, monitor);
						}
						ClientsThatNeedFullFrames.erase(std::remove_if(begin(ClientsThatNeedFullFrames), end(ClientsThatNeedFullFrames), [&](const auto i) {  return i.mids.empty(); }), end(ClientsThatNeedFullFrames));
					}
				});
				ServerDriver_.Start(this, config);
				ScreenCaptureManager_.Start();

			}

			virtual ~ServerImpl() {
				ScreenCaptureManager_.Stop();
				Clipboard_.reset();//make sure to prevent race conditions
				_Status = Server_Status::SERVER_STOPPED;
				ServerDriver_.Stop();

			}
			virtual void onConnection(const std::shared_ptr<IWebSocket>& socket) override {
				UNUSED(socket);
				std::vector<int> ids;
				auto p = Screen_Capture::GetMonitors();
				for (auto& a : p) {
					ids.push_back(Screen_Capture::Id(*a));
				}
				ServerDriver_.SendMonitorInfo(socket.get(), p);

				std::lock_guard<std::mutex> lock(ClientsThatNeedFullFramesLock);
				ClientsThatNeedFullFrames.push_back({ socket, ids });
			}

			virtual void onDisconnection(const IWebSocket& socket, int code, char* message, size_t length) override {
				UNUSED(socket);
				UNUSED(code);
				UNUSED(message);
				UNUSED(length);
			}

			virtual void onMessage(const IWebSocket& socket, const char* data, size_t len)  override {
				UNUSED(socket);
				UNUSED(data);
				UNUSED(len);
			}

			virtual void onReceive_ClipboardText(const char* data, unsigned int len) override {
				SL_RAT_LOG(Logging_Levels::INFO_log_level, "OnReceiveClipboard_Text " << len);
				Clipboard_->updateClipbard(data, static_cast<int>(len));
			}


			virtual void onReceive_Mouse(const MouseEvent* m) override {
				if (!Config_->IgnoreIncomingMouseEvents) SimulateMouseEvent(*m);
			}
			virtual void onReceive_Key(const KeyEvent* m)override {
				if (!Config_->IgnoreIncomingKeyboardEvents) SimulateKeyboardEvent(*m);
			}


			void OnMousePos(Point p) {
				ServerDriver_.SendMouse(nullptr, p);
			}

			Server_Status get_Status() const {
				return _Status;
			}
		};

		Server::Server()
		{
		}

		Server::~Server()
		{
			Stop();
		}

		void Server::Server::Start(std::shared_ptr<Server_Config> config)
		{
			ServerImpl_ = std::make_shared<ServerImpl>(config);

		}

		void Server::Server::Stop()
		{
			ServerImpl_.reset();
		}

		Server_Status Server::Server::get_Status() const
		{
			return ServerImpl_->get_Status();
		}

#if __ANDROID__
		void Server::Server::OnImage(char* buf, int width, int height)
		{
			return _ServerImpl->OnScreen(Utilities::Image::CreateImage(height, width, buf, 4));
		}
#endif

	}
}
