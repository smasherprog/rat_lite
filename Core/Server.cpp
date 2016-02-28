#include "stdafx.h"
#include "Server.h"
#include "Screen.h"
#include "Mouse.h"
#include "Image.h"
#include "ServerNetworkDriver.h"
#include "IServerDriver.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct Server_Config;
		}
		class ServerImpl : public Network::IServerDriver {
		public:
			std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> LastScreen;
			Capturing::MouseInfo _LastMouseInfo;
			Network::ServerNetworkDriver _ServerNetworkDriver;
			Network::IBaseNetworkDriver* _IUserNetworkDriver;
			bool _Keepgoing;

			ServerImpl(Network::Server_Config& config, Network::IBaseNetworkDriver* parent) : _ServerNetworkDriver(this, config), _IUserNetworkDriver(parent)
			{
				_LastMouseInfo.MouseType = 0xffffffff;
				_LastMouseInfo.Pos = Utilities::Point(0xffffffff, 0xffffffff);
				_Keepgoing = true;
			}

			virtual ~ServerImpl() {
				_Keepgoing = false;
			}
			virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) override {
				auto newimg = SL::Remote_Access_Library::Capturing::CaptureDesktop();
				_ServerNetworkDriver.Send(socket.get(), *newimg);
				_LastMouseInfo = Capturing::GetCursorInfo();
				_ServerNetworkDriver.Send(socket.get(), _LastMouseInfo);
				if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnConnect(socket);
			}
			virtual void OnClose(const std::shared_ptr<Network::ISocket>& socket)override {

				if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnClose(socket);
			}

			virtual void OnReceive(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Network::Packet>& packet)override {
				UNUSED(socket);
				UNUSED(packet);
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

				auto nmouse = SL::Remote_Access_Library::Capturing::GetCursorInfo();
				if (nmouse.MouseType != _LastMouseInfo.MouseType || nmouse.Pos != _LastMouseInfo.Pos) {
					_ServerNetworkDriver.Send(nullptr, nmouse);
					_LastMouseInfo = nmouse;
				}
			}
			int Run() {
				_ServerNetworkDriver.Start();
				while (_Keepgoing) {
					ProcessScreen();
					ProcessMouse();;
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
