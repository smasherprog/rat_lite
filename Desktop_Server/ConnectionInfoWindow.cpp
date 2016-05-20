
#include "../Core/stdafx.h"
#include "ConnectionInfoWindow.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>

#include <memory>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <sstream>

#include "../Core/IBaseNetworkDriver.h"
#include "../Core/Server.h"
#include "../Core/Server_Config.h"
#include "../Core/ApplicationDirectory.h"
#include "../Core/ISocket.h"
#include "LogWindow.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class ConnectionInfoWindowImpl : public Network::IBaseNetworkDriver {
			public:

				Fl_Window* cWindow = nullptr;
				Fl_Button* StartStopBtn = nullptr;
				Fl_Menu_Bar *_MenuBar = nullptr;
				Fl_Check_Button* _GrayScaleImage = nullptr;

				Fl_File_Chooser* _FolderBrowser = nullptr;
				Fl_Check_Button* _IgnoreIncomingMouseEvents_Checkbox = nullptr;

				std::shared_ptr<Network::Server_Config> config;
				std::unique_ptr<LogWindow> _LogWindow;
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
					std::ostringstream os;
					os << "User Connected! Ip: " << socket->get_ip() << " port: " << socket->get_port();
					_LogWindow->AddMessage(os.str());
				}
				virtual void OnReceive(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Network::Packet>& pack) {

				}
				virtual void OnClose(const std::shared_ptr<Network::ISocket>& socket) {
					std::ostringstream os;
					os << "User Disconnected! Ip: " << socket->get_ip() << " port: " << socket->get_port();
					_LogWindow->AddMessage(os.str());
				}

				static void toggle_service(Fl_Widget* o, void* userdata) {

					auto ptr = ((ConnectionInfoWindowImpl*)userdata);

					auto shrd = ptr->_Server.lock();//elevate to shared ptr
					if (shrd) {
						//the thread will exit when the server stops
						shrd->Stop(false);
						ptr->StartStopBtn->label("Start");
						ptr->_LogWindow->AddMessage("Stopping Service");
					}
					else {
						auto config = ptr->config;
						if (ptr->Runner.joinable()) ptr->Runner.join();
						ptr->_LogWindow->AddMessage("Starting Service");
						ptr->Runner = std::thread([ptr, config]() {
							auto shrdptr = std::make_shared<Server>(config, ptr);
							ptr->_Server = shrdptr;//assign the weak ptr for tracking
							shrdptr->Run();
						});
						ptr->StartStopBtn->label("Stop");
					}
				}
				static void Menu_CB(Fl_Widget*w, void*data) {
					auto p = (ConnectionInfoWindowImpl*)data;
					p->Menu_CB2();
				}
				void Menu_CB2() {
					char picked[80];
					_MenuBar->item_pathname(picked, sizeof(picked) - 1);

					// How to handle callbacks..
					if (strcmp(picked, "File/Quit") == 0) Fl::delete_widget(cWindow);
					if (strcmp(picked, "File/Log") == 0)  _LogWindow->Show();
				}
				static void SGrayScaleImageCB(Fl_Widget*w, void*data) {
					auto p = (ConnectionInfoWindowImpl*)data;
					p->config->SendGrayScaleImages = p->_GrayScaleImage->value() == 1;
				}
				void Init() {
					auto workingy = 0;
					cWindow = new Fl_Window(400, 420, 400, 300, "Server Settings");
					_MenuBar = new Fl_Menu_Bar(0, 0, cWindow->w(), 30);
					_MenuBar->add("File/Quit", 0, Menu_CB, (void*)this);
					_MenuBar->add("File/Log", 0, Menu_CB, (void*)this);
					workingy += 30;

					_GrayScaleImage = new Fl_Check_Button(70, workingy, 100, 14, " GrayScale");
					_GrayScaleImage->align(FL_ALIGN_LEFT);
					_GrayScaleImage->callback(SGrayScaleImageCB, this);

					workingy += 30;
					StartStopBtn = new Fl_Button(200, cWindow->h()-30, 80, 30, "Start");
					StartStopBtn->callback(toggle_service, this);
					workingy = StartStopBtn->h() + 4;
					cWindow->resizable(_MenuBar);
					cWindow->end();
					cWindow->show();
					_LogWindow = std::make_unique<LogWindow>();
					_LogWindow->set_MaxLines(100);
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
