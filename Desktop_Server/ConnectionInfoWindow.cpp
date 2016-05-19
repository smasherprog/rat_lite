
#include "../Core/stdafx.h"
#include "ConnectionInfoWindow.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Multi_Browser.H>
#include <memory>
#include <string>
#include <assert.h>
#include <stdio.h>

#include "../Core/IBaseNetworkDriver.h"
#include "../Core/Server.h"
#include "../Core/Server_Config.h"
#include "../Core/ApplicationDirectory.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class ConnectionInfoWindowImpl: Network::IBaseNetworkDriver {
			public:

				Fl_Window* cWindow = nullptr;
				Fl_Button* StartStopBtn = nullptr;
				Fl_Multi_Browser* Log = nullptr;
				//Fl_Text_Buffer* Logbuff = nullptr;


				std::shared_ptr<Network::Server_Config> config;
				std::weak_ptr<Server> _Server;
				std::thread Runner;

				ConnectionInfoWindowImpl() {
					//init defaults
					config = std::make_shared<Network::Server_Config>();
					config->WebSocketListenPort = 6001;// listen for websockets
					config->HttpListenPort = 8080;
					auto searchpath = executable_path(nullptr);
					auto exeindex = searchpath.find_last_of('\\');
					if (exeindex == searchpath.npos) exeindex = searchpath.find_last_of('/');
					if (exeindex != searchpath.npos) {
						config->WWWRoot = searchpath.substr(0, exeindex) + "/wwwroot/";
					}
					assert(exeindex != std::string::npos);
				}
				virtual ~ConnectionInfoWindowImpl() {
					auto shrd = _Server.lock();//elevate to shared ptr
					if (shrd) shrd->Stop(true);
					if (Runner.joinable()) Runner.join();
				}
				virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) {

				}
				virtual void OnReceive(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Network::Packet>& pack) {

				}
				virtual void OnClose(const std::shared_ptr<Network::ISocket>& socket) {

				}

				static void toggle_service(Fl_Widget* o, void* userdata) {

					auto ptr = ((ConnectionInfoWindowImpl*)userdata);
				
					auto shrd = ptr->_Server.lock();//elevate to shared ptr
					if (shrd) {
						//the thread will exit when the server stops
						shrd->Stop(false);
						ptr->StartStopBtn->label("Start");
						ptr->Log->add("Stopping Service");
					}
					else {
						auto config = ptr->config;
						if (ptr->Runner.joinable()) ptr->Runner.join();
						ptr->Log->add("Starting Service");
						ptr->Runner = std::thread([ptr, config]() {
							auto shrdptr = std::make_shared<Server>(config);
							ptr->_Server = shrdptr;//assign the weak ptr for tracking
							shrdptr->Run();
						});
						ptr->StartStopBtn->label("Stop");
					}
				}

				void Init() {
					auto workingy = 0;
					cWindow = new Fl_Window(400, 420, 300, 500, "Server Settings");
					StartStopBtn = new Fl_Button(200, workingy, 80, 30, "Start");
					StartStopBtn->callback(toggle_service, this);
					workingy = StartStopBtn->h() + 4;
					
					workingy += 20;
					Log = new Fl_Multi_Browser(5, workingy, cWindow->w() - 10, 200, "Events");
					Log->align(FL_ALIGN_TOP);
					cWindow->resizable(Log);
					cWindow->end();
					cWindow->show();

				}

			};

		}
	}
}


SL::Remote_Access_Library::UI::ConnectionInfoWindow::ConnectionInfoWindow()
{
	_ConnectWindowImpl = new ConnectionInfoWindowImpl();

}
SL::Remote_Access_Library::UI::ConnectionInfoWindow::~ConnectionInfoWindow()
{
	delete _ConnectWindowImpl;
}
void SL::Remote_Access_Library::UI::ConnectionInfoWindow::Init()
{
	if (_ConnectWindowImpl) _ConnectWindowImpl->Init();
}
