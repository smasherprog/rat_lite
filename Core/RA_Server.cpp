#include "stdafx.h"
#include "RA_Server.h"
#include "Screen.h"
#include "Mouse.h"
#include "Image.h"
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
				std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> LastScreen;
				std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> LastMouse;

				Utilities::Point LastMousePos = Utilities::Point(0xffffffff, 0xffffffff);
				Network::ServerNetworkDriver _ServerNetworkDriver;
				Network::HttpsServerNetworkDriver _HttpsServerNetworkDriver;
				Network::IBaseNetworkDriver* _IUserNetworkDriver;
				std::unique_ptr<Capturing::Clipboard> _Clipboard;

				bool _Keepgoing = true;
				std::mutex _NewClientLock;
				std::vector<std::shared_ptr<Network::ISocket>> _NewClients;
				Server_Status _Status = Server_Status::SERVER_STOPPED;
				std::shared_ptr<Network::Server_Config> _Config;

				ServerImpl(std::shared_ptr<Network::Server_Config> config, Network::IBaseNetworkDriver* parent) :
					_ServerNetworkDriver(this, config), _HttpsServerNetworkDriver(nullptr, config), _IUserNetworkDriver(parent), _Config(config)
				{
					
				}

				virtual ~ServerImpl() {
					_Clipboard.reset();//make sure to prevent race conditions
					_Status = Server_Status::SERVER_STOPPED;
					_Keepgoing = false;
				}

				virtual bool ValidateUntrustedCert(const std::shared_ptr<Network::ISocket>& socket) override {
					UNUSED(socket);
					return true;
				}
				virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) override {
					{
						std::lock_guard<std::mutex> lock(_NewClientLock);
						_NewClients.push_back(socket);
					}
					if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnConnect(socket);
				}

				virtual void OnClose(const Network::ISocket* socket)override {
					if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnClose(socket);
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
                        if(LastMouse->size() != img->size()) {
                            _ServerNetworkDriver.SendMouse(nullptr, *img);
                        } else if (memcmp(img->data(), LastMouse->data(), LastMouse->size()) != 0) {
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

#if !__ANDROID__
					auto mouseimg(Input::get_MouseImage());
					auto mousepos(Input::get_MousePosition());
					auto mouseimgtimer = std::chrono::steady_clock::now();
					auto mousepostimer = std::chrono::steady_clock::now();

					auto screenimg(Capturing::get_ScreenImage());
					auto screenimgtimer = std::chrono::steady_clock::now();
				
#endif


					while (_Keepgoing) {
#if !__ANDROID__
						auto curtime = std::chrono::steady_clock::now();
						//if (_ServerNetworkDriver.ClientCount() > 0) {
							//check mouse img first
							if (std::chrono::duration_cast<std::chrono::milliseconds>(curtime - mouseimgtimer).count() > _Config->MouseImageCaptureRate && is_ready(mouseimg)) {
								OnMouseImg(mouseimg.get());
								mouseimg = Input::get_MouseImage();
								mouseimgtimer = curtime;
							}
							//check mouse pos next
							if (std::chrono::duration_cast<std::chrono::milliseconds>(curtime - mousepostimer).count() > _Config->MousePositionCaptureRate && is_ready(mousepos)) {
								OnMousePos(mousepos.get());
								mousepos = Input::get_MousePosition();
								mouseimgtimer = curtime;
							}

							//check screen next
							if (std::chrono::duration_cast<std::chrono::milliseconds>(curtime - screenimgtimer).count() > _Config->ScreenImageCaptureRate && is_ready(screenimg)) {
								OnScreen(screenimg.get());
								screenimg = Capturing::get_ScreenImage();
								screenimgtimer = curtime;
							}
						//}
#endif
						std::this_thread::sleep_for(std::chrono::milliseconds(50));
					}
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
