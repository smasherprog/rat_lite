#include "stdafx.h"
#include "Server.h"
#include "Screen.h"
#include "Mouse.h"
#include "Image.h"
#include "ServerNetworkDriver.h"
#include "IServerDriver.h"


using namespace std::literals;


namespace SL {
	namespace Remote_Access_Library {
		class ServerImpl: public Network::IServerDriver {
		public:
			std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> LastScreen;
			Network::ServerNetworkDriver _ServerNetworkDriver;
			Network::IBaseNetworkDriver* _IBaseNetworkDriver;
			bool _Keepgoing;


			ServerImpl(unsigned short port, Network::IBaseNetworkDriver* parent = nullptr) : _ServerNetworkDriver(this, port), _IBaseNetworkDriver(parent)
			{
				_Keepgoing = true;
			}

			virtual ~ServerImpl() {
				_Keepgoing = false;
			}
			virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) {
				auto newimg = SL::Remote_Access_Library::Capturing::CaptureDesktop();
				_ServerNetworkDriver.Send(socket.get(), Utilities::Rect(Utilities::Point(0, 0), newimg->Height(), newimg->Width()), *newimg);
				if (_IBaseNetworkDriver != nullptr) _IBaseNetworkDriver->OnConnect(socket);
			}
			virtual void OnClose(const Network::ISocket* socket) {

				if (_IBaseNetworkDriver != nullptr) _IBaseNetworkDriver->OnClose(socket);
			}

			virtual void OnReceive(const Network::ISocket* socket, std::shared_ptr<Network::Packet>& packet) {

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
					std::this_thread::sleep_for(1s);

				}
				return 0;
			}
	
		};

	}
}

SL::Remote_Access_Library::Server::Server(unsigned short port)
{
	wxInitAllImageHandlers();
	_ServerImpl = std::make_shared<ServerImpl>(port);
}

SL::Remote_Access_Library::Server::~Server()
{

}

int SL::Remote_Access_Library::Server::Run()
{
	return _ServerImpl->Run();
	
}
