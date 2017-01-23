#include "Server.h"
#include <beast/websocket.hpp>
#include <beast/websocket/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <thread>
#include <mutex>
#include <string.h>

#include <assert.h>
#include "ScreenCapture.h"
#include "Input.h"
#include "ServerNetworkDriver.h"

#include "IServerDriver.h"
#include "Logging.h"
#include "Packet.h"
#include "Server_Config.h"


#include "Clipboard.h"




namespace SL {
	namespace Remote_Access_Library {

		class ServerImpl : public IServerDriver {
		public:

			SL::Screen_Capture::ScreenCaptureManager _ScreenCaptureManager;

			ServerNetworkDriver _ServerNetworkDriver;
			std::unique_ptr<Clipboard> _Clipboard;

			bool _Keepgoing = true;

			std::vector<uWS::WebSocket<uWS::CLIENT>> _Clients;
			std::mutex _ClientsLock;

			Server_Status _Status = Server_Status::SERVER_STOPPED;
			std::shared_ptr<Server_Config> _Config;
			//io_service ioservice;

			ServerImpl(std::shared_ptr<Server_Config> config) :
				_ServerNetworkDriver(this, config), _Config(config)
			{
				_Status = Server_Status::SERVER_RUNNING;
				_ServerNetworkDriver.Start();
				_Clipboard = std::make_unique<Clipboard>(&_Config->Share_Clipboard, [&](const char* c, int len) { _ServerNetworkDriver.SendClipboardText(nullptr, c, static_cast<unsigned int>(len)); });
		


			}

			virtual ~ServerImpl() {
				_Keepgoing = false;
				_ScreenCaptureManager.Stop();
				_Clipboard.reset();//make sure to prevent race conditions
				_Status = Server_Status::SERVER_STOPPED;
				_ServerNetworkDriver.Stop();

			}
			virtual void onConnection(uWS::WebSocket<uWS::CLIENT> ws, uWS::UpgradeInfo ui) override {
				UNUSED(ui);
				std::lock_guard<std::mutex> lock(_ClientsLock);
				_Clients.push_back(ws);
				//if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnConnect(socket);
			}

			virtual void onDisconnection(uWS::WebSocket<uWS::CLIENT> ws, int code, char *message, size_t length) override {
				std::lock_guard<std::mutex> lock(_ClientsLock);
				_Clients.erase(std::remove_if(begin(_Clients), end(_Clients), [&ws](const uWS::WebSocket<uWS::CLIENT>& p) { return p == ws; }), _Clients.end());
				//if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnClose(socket);
			}

			virtual void onMessage(uWS::WebSocket<uWS::CLIENT> ws, char *message, size_t length, uWS::OpCode opCode)  override {

			}

			virtual void OnReceive_ClipboardText(const char* data, unsigned int len) override {
				SL_RAT_LOG(Logging_Levels::INFO_log_level, "OnReceive_ClipboardText " << len);
				_Clipboard->copy_to_clipboard(data, static_cast<int>(len));
			}


			virtual void OnReceive_Mouse(MouseEvent* m) override {
				if (!_Config->IgnoreIncomingMouseEvents) SimulateMouseEvent(*m);
			}
			virtual void OnReceive_Key(KeyEvent* m)override {
				if (!_Config->IgnoreIncomingKeyboardEvents) SimulateKeyboardEvent(*m);
			}


			void OnMousePos(Point p) {
				_ServerNetworkDriver.SendMouse(nullptr, p);
			}
			//void OnMouseImg(std::shared_ptr<Utilities::Image> img) {
			//	if (!LastMouse) {
			//		_ServerNetworkDriver.SendMouse(nullptr, *img);
			//	}
			//	else {
			//		if (LastMouse->size() != img->size()) {
			//			_ServerNetworkDriver.SendMouse(nullptr, *img);
			//		}
			//		else if (memcmp(img->data(), LastMouse->data(), LastMouse->size()) != 0) {
			//			_ServerNetworkDriver.SendMouse(nullptr, *img);
			//		}
			//	}
			//	LastMouse = img;
			//}
			//void OnScreen(std::shared_ptr<Utilities::Image> img) {
			//	if (!_NewClients.empty()) {
			//		//make sure to send the full screens to any new connects
			//		std::lock_guard<std::mutex> lock(_NewClientLock);
			//		for (auto& a : _NewClients) {
			//			_ServerNetworkDriver.SendScreenFull(a.get(), *img);
			//		}
			//		_NewClients.clear();
			//	}
			//	if (LastScreen) {
			//		if (img->data() != LastScreen->data()) {
			//			for (auto r : SL::Remote_Access_Library::Utilities::Image::GetDifs(*LastScreen, *img)) {
			//				_ServerNetworkDriver.SendScreenDif(nullptr, r, *img);
			//			}
			//		}
			//	}
			//	LastScreen = img;//swap
			//}

	
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
