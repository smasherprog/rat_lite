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
			std::unique_ptr<Capturing::Screen> _ScreenCapture;
			bool _Keepgoing;
			std::mutex _NewClientLock;
			std::vector<std::shared_ptr<Network::ISocket>> _NewClients;


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
				UNUSED(socket);
				UNUSED(packet);
			}

			void OnScreen(std::shared_ptr<Utilities::Image> img)
			{
				if (!LastScreen) {//first screen send all!
					LastScreen = img;
					_ServerNetworkDriver.Send(nullptr, *LastScreen);
					std::lock_guard<std::mutex> lock(_NewClientLock);
					_NewClients.clear();
				}
				else {//compare and send all difs along
					{
						//make sure to send the full screens to any new connects
						std::lock_guard<std::mutex> lock(_NewClientLock);
						for (auto& a : _NewClients) {
							_ServerNetworkDriver.Send(nullptr, *img);
						}
						_NewClients.clear();
					}
					if (img->data() != LastScreen->data()) {
						for (auto r : SL::Remote_Access_Library::Utilities::Image::GetDifs(*LastScreen, *img)) {
							_ServerNetworkDriver.Send(nullptr, r, *img);
						}
						LastScreen = img;//swap
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
				_ScreenCapture = std::make_unique<Capturing::Screen>([this](std::shared_ptr<Utilities::Image> img) {this->OnScreen(img); });
				while (_Keepgoing) {
					ProcessMouse();
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
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

