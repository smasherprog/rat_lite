#include "stdafx.h"
#include "Server.h"
#include "Screen.h"
#include "Mouse.h"
#include "Image.h"
#include "ServerNetworkDriver.h"
#include "IServerDriver.h"
#include "IUserNetworkDriver.h"

using namespace std::literals;


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct Server_Config;
		}
		class ServerImpl : public Network::IServerDriver {
		public:
			std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> LastScreen;
			Network::ServerNetworkDriver _ServerNetworkDriver;
			Network::IUserNetworkDriver* _IUserNetworkDriver;
			bool _Keepgoing;

			ServerImpl(Network::Server_Config& config, Network::IUserNetworkDriver* parent) : _ServerNetworkDriver(this, config), _IUserNetworkDriver(parent)
			{
				_Keepgoing = true;
			}

			virtual ~ServerImpl() {
				_Keepgoing = false;
			}
			virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) override {
				auto newimg = SL::Remote_Access_Library::Capturing::CaptureDesktop();
				_ServerNetworkDriver.Send(socket.get(), Utilities::Rect(Utilities::Point(0, 0), newimg->Height(), newimg->Width()), *newimg);
				if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnConnect(socket);
			}
			virtual void OnClose(const Network::ISocket* socket)override {

				if (_IUserNetworkDriver != nullptr) _IUserNetworkDriver->OnClose(socket);
			}

			virtual void OnReceive(const Network::ISocket* socket, std::shared_ptr<Network::Packet>& packet)override {

			}

			void ProcessScreen()
			{
				if (!LastScreen) {//first screen send all!
					LastScreen = SL::Remote_Access_Library::Capturing::CaptureDesktop();
					_ServerNetworkDriver.Send(Utilities::Rect(Utilities::Point(0, 0), LastScreen->Height(), LastScreen->Width()), *LastScreen);
				}
				else {//compare and send all difs along
					auto newimg = SL::Remote_Access_Library::Capturing::CaptureDesktop();
					if (newimg->data() != LastScreen->data()) {

						for (auto r : SL::Remote_Access_Library::Utilities::Image::GetDifs(*LastScreen, *newimg)) {
							_ServerNetworkDriver.Send(r, *newimg);
						}

						LastScreen = newimg;//swap
					}
				}
			}

			int Run() {
				_ServerNetworkDriver.StartNetworkProcessing();
				while (_Keepgoing) {
					ProcessScreen();
					std::this_thread::sleep_for(100ms);
				}
				_ServerNetworkDriver.StopNetworkProcessing();
				return 0;
			}

		};

	}
}

SL::Remote_Access_Library::Server::Server(Network::Server_Config& config, Network::IUserNetworkDriver* parent)
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
