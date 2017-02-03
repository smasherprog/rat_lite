#include "ConnectWindow.h"
#include "Configs.h"
#include "FileCryptoLoader.h"
#include "InMemoryCryptoLoader.h"
#include "ViewerController.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/fl_ask.H>
#undef CREATE
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>

#include <string>
#include <memory>
#include <thread>

namespace SL {
	namespace RAT {

		class ConnectWindowImpl : Fl_Window {
		public:

			Fl_Input* bInput = nullptr;
			Fl_Button* connectbtn = nullptr;
			//Fl_Check_Button* checkbx = nullptr;
			Fl_Menu_Bar *_MenuBar = nullptr;
	
			std::string Host;
			std::unique_ptr<ViewerController> _MainWindow;
			std::shared_ptr<Client_Config> _Config;
			ConnectWindowImpl(std::shared_ptr<Client_Config> c) :Fl_Window(400, 420, 350, 110, "Connect to Host"), _Config(c){
			
			}
			virtual ~ConnectWindowImpl() {

			}
			static void Failed_to_reach_host(void* userdata) {
				auto ptr = ((ConnectWindowImpl*)userdata);
				std::string msg = std::string(ptr->bInput->value()) + std::string(" could not be resolved");
				auto st = msg.c_str();
				fl_alert("%s", st);
				ptr->ActivateWidgets();
			}
			static void DoConnect(void* userdata) {
				auto ptr = ((ConnectWindowImpl*)userdata);
				ptr->ActivateWidgets();
				ptr->_MainWindow = std::make_unique<ViewerController>(ptr->_Config, ptr->Host.c_str());
			}
			static void try_connect(std::string host, ConnectWindowImpl* ptr) {
				auto portspecified = host.find_last_of(':');
				if (portspecified != host.npos) {
					auto port = host.substr(portspecified + 1, host.size() - (portspecified - 1));
					if (!port.empty()) {
						ptr->_Config->WebSocketTLSLPort = static_cast<unsigned short>(std::stoul(port));
					}
					ptr->Host = host.substr(0, portspecified);
				}
				else ptr->Host = host;

				Fl::awake(DoConnect, ptr);//make sure to switch to the GUI thread
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
				if (host.empty()) {
					fl_alert("You must enter a hostname");
					return;
				}
				if (host.size() < 2) {
					fl_alert("You must enter a hostname");
					return;
				}
				ptr->connectbtn->label("Connecting . . .");
				ptr->bInput->deactivate();
				ptr->connectbtn->deactivate();
				std::thread th([host, ptr]() { try_connect(host, ptr); });
				th.detach();
			}
			/*static void setscale(Fl_Widget* o, void* userdata) {
				UNUSED(o);
				auto ptr = ((ConnectWindowImpl*)userdata);
				if (ptr->checkbx) ptr->_Config->Scale_Image = ptr->checkbx->value() == 1;
			}*/
			static void _FullPathToCertifiateCB(Fl_Widget*w, void*data) {
				UNUSED(w);
				auto p = (ConnectWindowImpl*)data;
				Fl_Native_File_Chooser chooser;
				chooser.title("Select the file location to save the Cert and Key");
				chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);

				chooser.filter("Certificate Files\t*.{crt,pem}");
				auto action = chooser.show();
				if (action == -1 || action == 1) return;//cancel was hit

				p->_Config->Public_Certficate = std::static_pointer_cast<ICryptoLoader>(std::make_shared<FileCryptoLoader>(chooser.filename()));
			}
			static void Menu_CB(Fl_Widget*w, void*data) {
				UNUSED(w);
				auto p = (ConnectWindowImpl*)data;
				Fl::delete_widget(p);
			}


			void Init(const std::string& host) {
				auto workingy = 0;
				auto startleft = 80;


				_MenuBar = new Fl_Menu_Bar(0, 0, w(), 30);
				_MenuBar->add("File/Quit", 0, Menu_CB, (void*)this);
				_MenuBar->add("Certificate/Location", 0, _FullPathToCertifiateCB, (void*)this);
				workingy += 30;

				bInput = new Fl_Input(startleft, workingy, w() - startleft, 30, "Host: ");
				bInput->value(host.c_str());
				workingy += 24;

				//checkbx = new Fl_Check_Button(startleft, workingy, w() - startleft, 30, " Scaling");
				//checkbx->align(FL_ALIGN_LEFT);
				//checkbx->callback(setscale, this);
			//	workingy += 24;


				connectbtn = new Fl_Button(0, h() - 30, w(), 30, "Connect");
				connectbtn->callback(try_connect_frm, this);
				end();
				show();
				if (!host.empty()) {
					connectbtn->activate();
				}
			}
		};

	}
}


SL::RAT::ConnectWindow::ConnectWindow(const std::shared_ptr<Client_Config> config, const std::string& host)
{
	_ConnectWindowImpl = new ConnectWindowImpl(config);
	_ConnectWindowImpl->Init(host);

}
SL::RAT::ConnectWindow::~ConnectWindow()
{
	//DELETE IS NOT NEEDED HERE AS it is handled by the fltk library
	//delete _ConnectWindowImpl;
}
