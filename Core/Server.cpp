#include "stdafx.h"
#include "Server.h"
#include "Screen.h"
#include "Mouse.h"
#include "Image.h"
#include "ServerNetworkDriver.h"
#include <atomic>

using namespace std::literals;


namespace SL {
	namespace Remote_Access_Library {
		class ServerImpl {
		public:
			std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> LastScreen;

			ServerImpl(unsigned short port) : _ServerNetworkDriver(this, port)
			{
				_Keepgoing = true;
				_Runner = std::thread(&SL::Remote_Access_Library::ServerImpl::Run, this);
			}

			~ServerImpl() {
				_Keepgoing = false;
				_Runner.join();
			}
			void OnConnect(const std::shared_ptr<Network::Socket>& socket) {
				auto newimg = SL::Remote_Access_Library::Capturing::CaptureDesktop();
				_ServerNetworkDriver.Send(socket.get(), Utilities::Rect(Utilities::Point(0, 0), newimg->Height(), newimg->Width()), *newimg);
			}
			void OnClose(const Network::Socket* socket) {

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

						auto start = std::chrono::steady_clock::now();
						auto retdif = SL::Remote_Access_Library::Utilities::Image::GetDifs(*LastScreen, *newimg, 64);
						auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);

						std::cout << "It took " << elapsed.count() << " milliseconds to compare run ProcessScreen " << retdif.size()<< std::endl;


						for (auto r : SL::Remote_Access_Library::Utilities::Image::GetDifs(*LastScreen, *newimg, 64)) {
							_ServerNetworkDriver.Send(r, *newimg);
						}

						LastScreen = newimg;//swap
					}
				}
			}

			void Run() {

				while (_Keepgoing) {
					auto start = std::chrono::steady_clock::now();
					ProcessScreen();
					auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);

					std::cout << "It took " << elapsed.count() << " milliseconds to compare run ProcessScreen " << std::endl;
					std::this_thread::sleep_for(1s);

				}
			}
			Network::ServerNetworkDriver<ServerImpl> _ServerNetworkDriver;
			bool _Keepgoing;
			std::thread _Runner;
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
