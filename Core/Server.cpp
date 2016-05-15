#include "stdafx.h"
#include "Server.h"
#include "Screen.h"
#include "Mouse.h"
#include "Image.h"
#include "ServerNetworkDriver.h"
#include "IServerDriver.h"
#include <thread>
#include <mutex>
#include "Logging.h"
#include "Packet.h"
#include "Server_Config.h"
#include <string.h>
#include "Keyboard.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct Server_Config;
		}
		class ServerImpl : public Network::IServerDriver {
		public:
			std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> LastScreen;
			std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> LastMouse;

			Utilities::Point LastMousePos;
			Network::ServerNetworkDriver _ServerNetworkDriver;
			Network::IBaseNetworkDriver* _IUserNetworkDriver;

			bool _Keepgoing;
			std::mutex _NewClientLock;
			std::vector<std::shared_ptr<Network::ISocket>> _NewClients;
			Server_Status Status;
			std::shared_ptr<Network::Server_Config> _Config;

			ServerImpl(std::shared_ptr<Network::Server_Config> config, Network::IBaseNetworkDriver* parent) : _ServerNetworkDriver(this, config), _IUserNetworkDriver(parent), _Config(config)
			{
				LastMousePos = Utilities::Point(0xffffffff, 0xffffffff);
				_Keepgoing = true;
				Status = Server_Status::SERVER_STOPPED;
			}

			virtual ~ServerImpl() {
				Status = Server_Status::SERVER_STOPPED;
				_Keepgoing = false;
			}
			virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) override {
				{
					std::lock_guard<std::mutex> lock(_NewClientLock);
					_NewClients.push_back(socket);

				}
				if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnConnect(socket);
			}

			virtual void OnClose(const std::shared_ptr<Network::ISocket>& socket)override {
				if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnClose(socket);
			}

			virtual void OnReceive(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Network::Packet>& packet)override {
				if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnReceive(socket, packet);
				else SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "OnReceive Unknown Packet "<<packet->Packet_Type);
			}

			void OnScreen(std::shared_ptr<Utilities::Image> img)
			{
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


			void OnMouseImg(std::shared_ptr<Utilities::Image> img)
			{
				if (!LastMouse) {
					_ServerNetworkDriver.SendMouse(nullptr, *img);
				}
				else {
					if (memcmp(img->data(), LastMouse->data(), std::min(LastMouse->size(), img->size())) != 0) {
						_ServerNetworkDriver.SendMouse(nullptr, *img);
					}
				}
				LastMouse = img;
			}
			void OnMousePos(Utilities::Point p) {
				_ServerNetworkDriver.SendMouse(nullptr, p);
			}


			virtual void OnMouse(Input::MouseEvent* m) override {
				if (!_Config->IgnoreIncomingMouseEvents) Input::SimulateMouseEvent(*m);
			}
			virtual void OnKey(Input::KeyEvent* m)override {
				if (!_Config->IgnoreIncomingMouseEvents) Input::SimulateKeyboardEvent(*m);
			}

			int Run() {
				Status = Server_Status::SERVER_RUNNING;
				_ServerNetworkDriver.Start();

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
					if (_ServerNetworkDriver.ClientCount()>0) {
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
					}
#endif
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
				}
				_ServerNetworkDriver.Stop();

				Status = Server_Status::SERVER_STOPPED;
				return 0;
			}
			void Stop(bool block) {
				_Keepgoing = false;
				if (block) {
					while (Status != Server_Status::SERVER_STOPPED) {
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
					}
				}
			}
			Server_Status get_Status() const {
				return Status;
			}
		};
	}
}

SL::Remote_Access_Library::Server::Server(std::shared_ptr<Network::Server_Config> config, Network::IBaseNetworkDriver* parent)
{
	_ServerImpl = std::make_shared<ServerImpl>(config, parent);
}

SL::Remote_Access_Library::Server::~Server()
{
	_ServerImpl->Stop(true);
}

int SL::Remote_Access_Library::Server::Run()
{
	return _ServerImpl->Run();

}

void SL::Remote_Access_Library::Server::Stop(bool block)
{
	_ServerImpl->Stop(block);
}

SL::Remote_Access_Library::Server_Status SL::Remote_Access_Library::Server::get_Status() const
{
	return _ServerImpl->get_Status();
}
#if __ANDROID__
void SL::Remote_Access_Library::Server::OnImage(char* buf, int width, int height)
{
	return _ServerImpl->OnScreen(Utilities::Image::CreateImage(height, width, buf, 4));
}
#endif
