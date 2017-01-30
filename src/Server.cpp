#include "Server.h"

#include <thread>
#include <string.h>

#include <assert.h>
#include "ScreenCapture.h"
#include "Input.h"
#include "ServerNetworkDriver.h"

#include "IServerDriver.h"
#include "Logging.h"
#include "Configs.h"


#include "Clipboard.h"




namespace SL {
	namespace Remote_Access_Library {

		class ServerImpl : public IServerDriver {
		public:

			SL::Screen_Capture::ScreenCaptureManager _ScreenCaptureManager;

			ServerNetworkDriver _ServerNetworkDriver;
			std::unique_ptr<Clipboard> _Clipboard;

			Server_Status _Status = Server_Status::SERVER_STOPPED;
			std::shared_ptr<Server_Config> _Config;

			ServerImpl(std::shared_ptr<Server_Config> config) :
				_ServerNetworkDriver(), _Config(config)
			{
				_Status = Server_Status::SERVER_RUNNING;
				_ServerNetworkDriver.Start(this, config);
				_Clipboard = std::make_unique<Clipboard>(&_Config->Share_Clipboard, [&](const char* c, int len) { _ServerNetworkDriver.SendClipboardText(nullptr, c, static_cast<unsigned int>(len)); });
				_ScreenCaptureManager.setMouseChangeInterval(_Config->MousePositionCaptureRate);
				_ScreenCaptureManager.setFrameChangeInterval(_Config->ScreenImageCaptureRate);
				_ScreenCaptureManager.setMonitorsToCapture([]() {
					return Screen_Capture::GetMonitors();
				});
				_ScreenCaptureManager.onMouseChanged([&](const SL::Screen_Capture::Image* img, int x, int y) {
					if (img) {
						_ServerNetworkDriver.SendMouse(nullptr, *img);
					}
					_ServerNetworkDriver.SendMouse(nullptr, Point(x, y));
				});
				_ScreenCaptureManager.onFrameChanged([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor) {
					_ServerNetworkDriver.SendFrameChange(nullptr, img);
				});
				_ScreenCaptureManager.onNewFrame([&](const SL::Screen_Capture::Image& img, const SL::Screen_Capture::Monitor& monitor) {
					_ServerNetworkDriver.SendFrame(nullptr, img);
				});
				_ScreenCaptureManager.Start();
			}

			virtual ~ServerImpl() {
				_ScreenCaptureManager.Stop();
				_Clipboard.reset();//make sure to prevent race conditions
				_Status = Server_Status::SERVER_STOPPED;
				_ServerNetworkDriver.Stop();

			}
			virtual void onConnection(const std::shared_ptr<ISocket>& socket) override {
				UNUSED(socket);
				//if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnConnect(socket);
			}

			virtual void onDisconnection(const ISocket* socket) override {
				UNUSED(socket);
				//if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnClose(socket);
			}

			virtual void onMessage(const std::shared_ptr<ISocket>& socket, const char* data, size_t len)  override {
				UNUSED(socket);
				UNUSED(data);
				UNUSED(len);
			}

			virtual void OnReceive_ClipboardText(const char* data, size_t len) override {
				SL_RAT_LOG(Logging_Levels::INFO_log_level, "OnReceive_ClipboardText " << len);
				_Clipboard->copy_to_clipboard(data, static_cast<int>(len));
			}


			virtual void OnReceive_Mouse(const MouseEvent* m) override {
				if (!_Config->IgnoreIncomingMouseEvents) SimulateMouseEvent(*m);
			}
			virtual void OnReceive_Key(const KeyEvent* m)override {
				if (!_Config->IgnoreIncomingKeyboardEvents) SimulateKeyboardEvent(*m);
			}


			void OnMousePos(Point p) {
				_ServerNetworkDriver.SendMouse(nullptr, p);
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
