#include "Server.h"

#include "ScreenCapture.h"
#include "Mouse.h"
#include "ServerNetworkDriver.h"
#include "HttpsServerNetworkDriver.h"
#include "IServerDriver.h"
#include "Logging.h"
#include "Packet.h"
#include "Server_Config.h"
#include "Keyboard.h"
#include "IBaseNetworkDriver.h"
#include "crypto.h"
#include "Clipboard.h"

#include <thread>
#include <mutex>
#include <string.h>
#include <assert.h>


namespace SL {
	namespace Remote_Access_Library {
		namespace Server {

			class ServerImpl : public Network::IServerDriver {
			public:

				SL::Screen_Capture::ScreenCaptureManager _ScreenCaptureManager;

				Network::ServerNetworkDriver _ServerNetworkDriver;
				std::unique_ptr<Capturing::Clipboard> _Clipboard;

				bool _Keepgoing = true;

				std::vector<uWS::WebSocket<uWS::CLIENT>> _Clients;
				std::mutex _ClientsLock;

				Server_Status _Status = Server_Status::SERVER_STOPPED;
				std::shared_ptr<Network::Server_Config> _Config;

				ServerImpl(std::shared_ptr<Network::Server_Config> config, Network::IBaseNetworkDriver* parent) :
					_ServerNetworkDriver(this, config), _Config(config)
				{

				}

				virtual ~ServerImpl() {
					_ScreenCaptureManager.StopCapturing();
					_Clipboard.reset();//make sure to prevent race conditions
					_Status = Server_Status::SERVER_STOPPED;
					_Keepgoing = false;
				}
				virtual void onConnection(uWS::WebSocket<uWS::CLIENT> ws, uWS::UpgradeInfo ui) override {
					std::lock_guard<std::mutex> lock(_ClientsLock);
					_Clients.push_back(ws);
					//if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnConnect(socket);
				}

				virtual void onDisconnection(uWS::WebSocket<uWS::CLIENT> ws, int code, char *message, size_t length) override {
					std::lock_guard<std::mutex> lock(_ClientsLock);
					_Clients.erase(std::remove_if(begin(_Clients), end(_Clients), [&ws](const uWS::WebSocket<uWS::CLIENT>& p) { return p == ws; }), _Clients.end());
					//if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnClose(socket);
				}

				virtual void OnReceive(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Network::Packet>& packet)override {
					if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnReceive(socket, packet);
					else SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "OnReceive Unknown Packet " << packet->Packet_Type);
				}

				virtual void OnReceive_ClipboardText(const char* data, unsigned int len) override {
					SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "OnReceive_ClipboardText " << len);
					_Clipboard->copy_to_clipboard(data, static_cast<int>(len));
				}


				virtual void OnReceive_Mouse(Input::MouseEvent* m) override {
					if (!_Config->IgnoreIncomingMouseEvents) Input::SimulateMouseEvent(*m);
				}
				virtual void OnReceive_Key(Input::KeyEvent* m)override {
					if (!_Config->IgnoreIncomingKeyboardEvents) Input::SimulateKeyboardEvent(*m);
				}


				void OnMousePos(Utilities::Point p) {
					_ServerNetworkDriver.SendMouse(nullptr, p);
				}
				void OnMouseImg(std::shared_ptr<Utilities::Image> img) {
					if (!LastMouse) {
						_ServerNetworkDriver.SendMouse(nullptr, *img);
					}
					else {
						if (LastMouse->size() != img->size()) {
							_ServerNetworkDriver.SendMouse(nullptr, *img);
						}
						else if (memcmp(img->data(), LastMouse->data(), LastMouse->size()) != 0) {
							_ServerNetworkDriver.SendMouse(nullptr, *img);
						}
					}
					LastMouse = img;
				}
				void OnScreen(std::shared_ptr<Utilities::Image> img) {
					if (!_NewClients.empty()) {
						//make sure to send the full screens to any new connects
						std::lock_guard<std::mutex> lock(_NewClientLock);
						for (auto& a : _NewClients) {
							_ServerNetworkDriver.SendScreenFull(a.get(), *img);
						}
						_NewClients.clear();
					}
					if (LastScreen) {
						if (img->data() != LastScreen->data()) {
							for (auto r : SL::Remote_Access_Library::Utilities::Image::GetDifs(*LastScreen, *img)) {
								_ServerNetworkDriver.SendScreenDif(nullptr, r, *img);
							}
						}
					}
					LastScreen = img;//swap
				}

				int Run() {
					_Status = Server_Status::SERVER_RUNNING;
					_ServerNetworkDriver.Start();
					_HttpsServerNetworkDriver.Start();
					_Clipboard = std::make_unique<Capturing::Clipboard>(&_Config->Share_Clipboard, [&](const char* c, int len) { _ServerNetworkDriver.SendClipboardText(nullptr, c, static_cast<unsigned int>(len)); });



					std::this_thread::sleep_for(std::chrono::milliseconds(50));

					_HttpsServerNetworkDriver.Stop();
					_ServerNetworkDriver.Stop();

					_Status = Server_Status::SERVER_STOPPED;
					return 0;
				}
				void Stop(bool block) {
					_Keepgoing = false;
					if (block) {
						while (_Status != Server_Status::SERVER_STOPPED) {
							std::this_thread::sleep_for(std::chrono::milliseconds(20));
						}
					}
				}
				Server_Status get_Status() const {
					return _Status;
				}
			};
		}
	}
}

SL::Remote_Access_Library::Server::RA_Server::RA_Server(std::shared_ptr<Network::Server_Config> config, Network::IBaseNetworkDriver* parent)
{
	_ServerImpl = std::make_shared<ServerImpl>(config, parent);
}

SL::Remote_Access_Library::Server::RA_Server::~RA_Server()
{
	_ServerImpl->Stop(true);
}

int SL::Remote_Access_Library::Server::RA_Server::Run()
{
	return _ServerImpl->Run();
}

void SL::Remote_Access_Library::Server::RA_Server::Stop(bool block)
{
	_ServerImpl->Stop(block);
}

SL::Remote_Access_Library::Server_Status SL::Remote_Access_Library::Server::RA_Server::get_Status() const
{
	return _ServerImpl->get_Status();
}

std::string SL::Remote_Access_Library::Server::RA_Server::Validate_Settings(std::shared_ptr<Network::Server_Config> config)
{
	std::string ret;
	assert(config.get() != nullptr);
	ret += Crypto::ValidateCertificate(config->Public_Certficate.get());
	ret += Crypto::ValidatePrivateKey(config->Private_Key.get(), config->PasswordToPrivateKey);
	if (!SL::Directory_Exists(config->WWWRoot)) ret += "You must supply a valid folder for wwwroot!\n";

	return ret;
}
#if __ANDROID__
void SL::Remote_Access_Library::Server::RA_Server::OnImage(char* buf, int width, int height)
{
	return _ServerImpl->OnScreen(Utilities::Image::CreateImage(height, width, buf, 4));
}
#endif
