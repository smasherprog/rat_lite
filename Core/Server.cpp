#include "stdafx.h"
#include "Server.h"
#include "Screen.h"
#include "Mouse.h"
#include "Image.h"
#include "ServerNetworkDriver.h"
#include "IServerDriver.h"
#include <FL/Fl.H>

using namespace std::literals;


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct Server_Config;
		}
		class ServerImpl : public Network::IServerDriver {
		public:
			std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> LastScreen;
			unsigned int _LastMouse = 0xffffffff;
			Utilities::Point LastMousePosition;
			Network::ServerNetworkDriver _ServerNetworkDriver;
			Network::IBaseNetworkDriver* _IUserNetworkDriver;
			bool _Keepgoing;

			ServerImpl(Network::Server_Config& config, Network::IBaseNetworkDriver* parent) : _ServerNetworkDriver(this, config), _IUserNetworkDriver(parent), LastMousePosition(0, 0)
			{

				_Keepgoing = true;
			}

			virtual ~ServerImpl() {
				_Keepgoing = false;
			}
			virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) override {
				auto newimg = SL::Remote_Access_Library::Capturing::CaptureDesktop();
				_ServerNetworkDriver.Send(socket.get(), *newimg);
				_LastMouse = SL::Remote_Access_Library::Capturing::GetShownMouseCursor();
				_ServerNetworkDriver.SendMouse(socket.get(), _LastMouse);
				if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnConnect(socket);
			}
			virtual void OnClose(const std::shared_ptr<Network::ISocket>& socket)override {

				if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnClose(socket);
			}

			virtual void OnReceive(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Network::Packet>& packet)override {

			}

			void ProcessScreen()
			{
				if (!LastScreen) {//first screen send all!
					LastScreen = SL::Remote_Access_Library::Capturing::CaptureDesktop();
					_ServerNetworkDriver.Send(nullptr, *LastScreen);
				}
				else {//compare and send all difs along
					auto newimg = SL::Remote_Access_Library::Capturing::CaptureDesktop();
					if (newimg->data() != LastScreen->data()) {

						for (auto r : SL::Remote_Access_Library::Utilities::Image::GetDifs(*LastScreen, *newimg)) {
							_ServerNetworkDriver.Send(nullptr, r, *newimg);
						}
						LastScreen = newimg;//swap
					}
				}
			}
			void ProcessMouse()
			{

				auto newimg = SL::Remote_Access_Library::Capturing::GetShownMouseCursor();
				if (newimg != _LastMouse) {
					_ServerNetworkDriver.SendMouse(nullptr, newimg);
					_LastMouse = newimg;
				}

				Utilities::Point p1;
				Fl::get_mouse(p1.X, p1.Y);
				if (p1 != LastMousePosition) {
					_ServerNetworkDriver.SendMouse(nullptr, p1);
					LastMousePosition = p1;
				}
			}
			int Run() {
				_ServerNetworkDriver.Start();
				while (_Keepgoing) {
					ProcessScreen();
					ProcessMouse();
					std::this_thread::sleep_for(100ms);
				}
				_ServerNetworkDriver.Stop();
				return 0;
			}

		};

	}
}

SL::Remote_Access_Library::Server::Server(Network::Server_Config& config, Network::IBaseNetworkDriver* parent)
{
	_ServerImpl = std::make_shared<ServerImpl>(config, parent);
}

SL::Remote_Access_Library::Server::~Server()
{

}

int SL::Remote_Access_Library::Server::Run()
{
	return _ServerImpl->Run();

}
