
#include "../Core/stdafx.h"

#include "ConnectWindow.h"
#include "ViewerWindow.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/fl_ask.H>
#include <string>
#include <memory>
#include <thread>

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class ConnectWindowImpl {
			public:
				Fl_Window* cWindow = nullptr;
				Fl_Input* bInput = nullptr;
				Fl_Button* connectbtn = nullptr;
				Fl_Check_Button* checkbx = nullptr;
				std::string Host, Port;
				std::unique_ptr<ViewerWindow> _MainWindow;
				ConnectWindowImpl() {
		
				}

				static void Failed_to_reach_host(void* userdata) {
					auto ptr = ((ConnectWindowImpl*)userdata);
					std::string msg = std::string(ptr->bInput->value()) + std::string(" could not be resolved");
					auto st = msg.c_str();
                    fl_alert(st);
					ptr->ActivateWidgets();
				}
				static void DoConnect(void* userdata) {
					auto ptr = ((ConnectWindowImpl*)userdata);
					ptr->ActivateWidgets();
					ptr->_MainWindow = std::make_unique<ViewerWindow>(ptr->Host.c_str(), ptr->Port.c_str());
				}
				static void try_connect(std::string host, ConnectWindowImpl* ptr) {
					auto portspecified = host.find_last_of(':');
					ptr->Port = "6001";
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
					
					auto ptr = ((ConnectWindowImpl*)userdata);
					std::string host = ptr->bInput->value();
					ptr->connectbtn->label("Connecting . . .");
					ptr->bInput->deactivate();
					ptr->connectbtn->deactivate();
					std::thread th([host, ptr]() { try_connect(host, ptr); });
					th.detach();
				}
				static void setscale(Fl_Widget* o, void* userdata) {
					auto ptr = ((ConnectWindowImpl*)userdata);
					if (ptr->checkbx && ptr->_MainWindow) ptr->_MainWindow->ScaleView(ptr->checkbx->value() == 1);
				}
				void Init() {

					cWindow = new Fl_Window(400, 420, 300, 70, "Connect to Host");
					bInput = new Fl_Input(50, 0, 150, 30, "Host: ");
					checkbx = new Fl_Check_Button(60, 40, 100, 14, " Scaling");
					checkbx->align(FL_ALIGN_LEFT);
					checkbx->callback(setscale, this);
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
