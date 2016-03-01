#include "stdafx.h"
#include "ConnectWindow.h"
#include "ViewerWindow.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include "TCPSocket.h"
#include <FL/fl_ask.H>


namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class ConnectWindowImpl {
			public:
				Fl_Window* cWindow = nullptr;
				Fl_Input* bInput = nullptr;
				Fl_Button* connectbtn = nullptr;
				std::string Host, Port;
				std::unique_ptr<ViewerWindow> _MainWindow;
				ConnectWindowImpl() {
		
				}

				static void Failed_to_reach_host(void* userdata) {
					auto ptr = ((ConnectWindowImpl*)userdata);
					std::string msg = std::string(ptr->bInput->value()) + std::string(" could not be resolved");
					fl_alert(msg.c_str());
					ptr->ActivateWidgets();
				}
				static void DoConnect(void* userdata) {
					auto ptr = ((ConnectWindowImpl*)userdata);
					ptr->ActivateWidgets();
					ptr->_MainWindow = std::make_unique<ViewerWindow>(ptr->Host.c_str(), ptr->Port.c_str());
				}
				static void try_connect(std::string host, ConnectWindowImpl* ptr) {
					auto portspecified = host.find_last_of(':');
					ptr->Port = "6000";
					if (portspecified != host.npos) {
						ptr->Port = host.substr(portspecified + 1, host.size() - (portspecified - 1));
						ptr->Host = host.substr(0, portspecified);
					}
					else ptr->Host = host;
					Fl::awake(DoConnect, ptr);
				}

				void ActivateWidgets() {
					bInput->activate();
					connectbtn->activate();
					connectbtn->label("Connect");
				}
				static void try_connect_frm(Fl_Widget* o, void* userdata) {
					UNUSED(o);
					auto ptr = ((ConnectWindowImpl*)userdata);
					std::string host = ptr->bInput->value();
					ptr->connectbtn->label("Connecting . . .");
					ptr->bInput->deactivate();
					ptr->connectbtn->deactivate();
					std::thread th([host, ptr]() { try_connect(host, ptr); });
					th.detach();
				}

				void Init() {

					cWindow = new Fl_Window(400, 420, 300, 50, "Connect to Host");
					bInput = new Fl_Input(50, 0, 150, 30, "Host: ");
					//bInput->callback(try_connect_frm, this);
					connectbtn = new Fl_Button(200, 0, 80, 30, "Connect");
					connectbtn->callback(try_connect_frm, this);
					cWindow->end();
					cWindow->show();

				}

			};

		}
	}
}


SL::Remote_Access_Library::UI::ConnectWindow::ConnectWindow()
{
	_ConnectWindowImpl = new ConnectWindowImpl();

}
SL::Remote_Access_Library::UI::ConnectWindow::~ConnectWindow()
{
	delete _ConnectWindowImpl;
}
void SL::Remote_Access_Library::UI::ConnectWindow::Init()
{
	if (_ConnectWindowImpl) _ConnectWindowImpl->Init();
}
