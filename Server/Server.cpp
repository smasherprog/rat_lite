#include "Server.h"
#include <thread>
#include <string.h>
#include <assert.h>

#include "ScreenCapture.h"
#include "ServerNetworkDriver.h"
#include "IServerDriver.h"
#include "Clipboard.h"
#include "Configs.h"
#include "Shapes.h"
#include "Input.h"


namespace SL {
	namespace RAT {

		class ServerImpl : public IServerDriver {
		public:

			SL::Screen_Capture::ScreenCaptureManager _ScreenCaptureManager;

			ServerNetworkDriver ServerNetworkDriver_;
			std::unique_ptr<Clipboard> Clipboard_;

			Server_Status _Status = Server_Status::SERVER_STOPPED;
			std::shared_ptr<Server_Config> _Config;

			ServerImpl(std::shared_ptr<Server_Config> config) :
				ServerNetworkDriver_(), _Config(config)
			{
				_Status = Server_Status::SERVER_RUNNING;
		
				Clipboard_ = std::make_unique<Clipboard>();
				Clipboard_->shareClipboard(_Config->Share_Clipboard);
				Clipboard_->onChange([&](const char* c, int len) { ServerNetworkDriver_.SendClipboardText(nullptr, c, static_cast<unsigned int>(len)); });

				_ScreenCaptureManager.setMouseChangeInterval(_Config->MousePositionCaptureRate);
				_ScreenCaptureManager.setFrameChangeInterval(_Config->ScreenImageCaptureRate);
				_ScreenCaptureManager.setMonitorsToCapture([]() {
					return Screen_Capture::GetMonitors();
				});
				_ScreenCaptureManager.onMouseChanged([&](const SL::Screen_Capture::Image* img, int x, int y) {
					if (img) {
						ServerNetworkDriver_.SendMouse(nullptr, *img);
					}
					ServerNetworkDriver_.SendMouse(nullptr, Point(x, y));
				});
				_ScreenCaptureManager.onFrameChanged([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor) {
					ServerNetworkDriver_.SendFrameChange(nullptr, img);
				});
				_ScreenCaptureManager.onNewFrame([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor) {
					ServerNetworkDriver_.SendFrame(nullptr, img);
				});		
				ServerNetworkDriver_.Start(this, config);
				_ScreenCaptureManager.Start();

			}

			virtual ~ServerImpl() {
				_ScreenCaptureManager.Stop();
				Clipboard_.reset();//make sure to prevent race conditions
				_Status = Server_Status::SERVER_STOPPED;
				ServerNetworkDriver_.Stop();

			}
			virtual void onConnection(const std::shared_ptr<IWebSocket>& socket) override {
				UNUSED(socket);
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

			virtual void onReceive_ClipboardText(const char* data, size_t len) override {
				SL_RAT_LOG(Logging_Levels::INFO_log_level, "OnReceiveClipboard_Text " << len);
				Clipboard_->updateClipbard(data, static_cast<int>(len));
			}


			virtual void onReceive_Mouse(const MouseEvent* m) override {
				if (!_Config->IgnoreIncomingMouseEvents) SimulateMouseEvent(*m);
			}
			virtual void onReceive_Key(const KeyEvent* m)override {
				if (!_Config->IgnoreIncomingKeyboardEvents) SimulateKeyboardEvent(*m);
			}


			void OnMousePos(Point p) {
				ServerNetworkDriver_.SendMouse(nullptr, p);
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
			_ServerImpl = std::make_shared<ServerImpl>(config);
			
		}

		void Server::Server::Stop()
		{
			_ServerImpl.reset();
		}

		Server_Status Server::Server::get_Status() const
		{
			return _ServerImpl->get_Status();
		}

		std::string Server::Server::Validate_Settings(std::shared_ptr<Server_Config> config)
		{
			std::string ret;
			//assert(config.get() != nullptr);
			//ret += Crypto::ValidateCertificate(config->Public_Certficate.get());
			//ret += Crypto::ValidatePrivateKey(config->Private_Key.get(), config->PasswordToPrivateKey);
			//if (!SL::Directory_Exists(config->WWWRoot)) ret += "You must supply a valid folder for wwwroot!\n";

			return ret;
		}
#if __ANDROID__
		void Server::Server::OnImage(char* buf, int width, int height)
		{
			return _ServerImpl->OnScreen(Utilities::Image::CreateImage(height, width, buf, 4));
		}
#endif

	}
}
