
#include "stdafx.h"

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Tooltip.H>
#include <Fl/Fl_Check_Button.H>
#include <FL/x.H>               // needed for fl_display
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Secret_Input.H>

#include <memory>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <sstream>

#include "ConnectionInfoWindow.h"
#include "IBaseNetworkDriver.h"
#include "RA_Server.h"
#include "Server_Config.h"
#include "ApplicationDirectory.h"
#include "ISocket.h"
#include "LogWindow.h"
#include "SliderInput.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Server {


			namespace UI {
				class ConnectionInfoWindowImpl : public Network::IBaseNetworkDriver<std::shared_ptr<Network::ISocket>, std::shared_ptr<Network::Packet>> {
				public:

					Fl_Window* cWindow = nullptr;
					Fl_Button* StartStopBtn = nullptr;
					Fl_Menu_Bar *_MenuBar = nullptr;
					Fl_Check_Button* _GrayScaleImage = nullptr;
					Fl_Check_Button* _IgnoreIncomingMouse = nullptr;
					Fl_Check_Button* _IgnoreIncomingKeyboard = nullptr;

					Fl_Input* _FullPathToCertificate = nullptr;

					Fl_Input* _FullPathToPrivateKey = nullptr;
					Fl_Secret_Input* _PasswordToPrivateKey = nullptr;

					Fl_Check_Button* _IgnoreIncomingMouseEvents_Checkbox = nullptr;

					std::shared_ptr<Network::Server_Config> config;
					std::unique_ptr<LogWindow> _LogWindow;
					SliderInput* _ImageQualitySlider = nullptr;
					SliderInput* _MouseCaptureRateSlider = nullptr;
					SliderInput* _MousePositionCaptureRate = nullptr;
					SliderInput* _ScreenCaptureRate = nullptr;

					std::weak_ptr<RA_Server> _Server;
					std::thread Runner;

					ConnectionInfoWindowImpl() {
						//init defaults
						config = std::make_shared<Network::Server_Config>();
						config->WebSocketTLSListenPort = 6001;// listen for websockets
						config->HttpTLSListenPort = 8080;
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

					virtual bool ValidateUntrustedCert(const std::shared_ptr<Network::ISocket>& socket) override {
						UNUSED(socket);

						return true;
					}
					virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) {
						std::ostringstream os;
						os << "User Connected! Ip: " << socket->get_ipv4_address() << " port: " << socket->get_port();
						_LogWindow->AddMessage(os.str());
					}
					virtual void OnReceive(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Network::Packet>& pack) {
						UNUSED(socket);
						UNUSED(pack);
					}
					virtual void OnClose(const std::shared_ptr<Network::ISocket>& socket) {
						std::ostringstream os;
						os << "User Disconnected! Ip: " << socket->get_ipv4_address() << " port: " << socket->get_port();
						_LogWindow->AddMessage(os.str());
					}

					static void toggle_service(Fl_Widget* o, void* userdata) {
						UNUSED(o);
						auto ptr = ((ConnectionInfoWindowImpl*)userdata);

						auto shrd = ptr->_Server.lock();//elevate to shared ptr
						if (shrd) {
							//the thread will exit when the server stops
							shrd->Stop(false);
							ptr->StartStopBtn->label("Start");
							ptr->_LogWindow->AddMessage("Stopping Service");
							ptr->StartStopBtn->color(FL_GREEN);
						}
						else {
							auto config = ptr->config;
							auto ret = RA_Server::Validate_Settings(config);//check settings
							if (!ret.empty()) {
								fl_alert("%s", ret.c_str());
							}
							else {
								if (ptr->Runner.joinable()) ptr->Runner.join();
								ptr->_LogWindow->AddMessage("Starting Service");
								ptr->Runner = std::thread([ptr, config]() {
									auto shrdptr = std::make_shared<RA_Server>(config, ptr);
									ptr->_Server = shrdptr;//assign the weak ptr for tracking
									shrdptr->Run();
								});
								ptr->StartStopBtn->label("Stop");
								ptr->StartStopBtn->color(FL_BLUE);
							}
						}
					}
					static void Menu_CB(Fl_Widget*w, void*data) {
						UNUSED(w);
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
					static void GenerateCerts(Fl_Widget*w, void*data) {
						UNUSED(w);
						auto p = (ConnectionInfoWindowImpl*)data;
						//GENERATE CERT STUFF MM KAY!!
					}
					static void SGrayScaleImageCB(Fl_Widget*w, void*data) {
						UNUSED(w);
						auto p = (ConnectionInfoWindowImpl*)data;
						p->config->SendGrayScaleImages = p->_GrayScaleImage->value() == 1;
					}
					static void SIgnoreIncomingMouseCB(Fl_Widget*w, void*data) {
						UNUSED(w);
						auto p = (ConnectionInfoWindowImpl*)data;
						p->config->IgnoreIncomingMouseEvents = p->_IgnoreIncomingMouse->value() == 1;
					}
					static void SIgnoreIncomingKeyboardCB(Fl_Widget*w, void*data) {
						UNUSED(w);
						auto p = (ConnectionInfoWindowImpl*)data;
						p->config->IgnoreIncomingKeyboardEvents = p->_IgnoreIncomingKeyboard->value() == 1;
					}
					static void _ImageQualitySliderCB(Fl_Widget*w, void*data) {
						UNUSED(w);
						auto p = (ConnectionInfoWindowImpl*)data;
						p->config->ImageCompressionSetting = p->_ImageQualitySlider->value();
					}
					static void _MouseCaptureRateSliderCB(Fl_Widget*w, void*data) {
						UNUSED(w);
						auto p = (ConnectionInfoWindowImpl*)data;
						p->config->MouseImageCaptureRate = p->_MouseCaptureRateSlider->value() * 1000;
					}
					static void _MousePositionCaptureRateCB(Fl_Widget*w, void*data) {
						UNUSED(w);
						auto p = (ConnectionInfoWindowImpl*)data;
						p->config->MousePositionCaptureRate = p->_MousePositionCaptureRate->value();
					}
					static void _ScreenCaptureRateCB(Fl_Widget*w, void*data) {
						UNUSED(w);
						auto p = (ConnectionInfoWindowImpl*)data;
						p->config->ScreenImageCaptureRate = p->_ScreenCaptureRate->value();
					}
					static void _FullPathToCertificateCB(Fl_Widget*w, void*data) {
						UNUSED(w);
						auto p = (ConnectionInfoWindowImpl*)data;
						Fl_File_Chooser chooser(".",                        // directory
							"Certificate Filec (*.crt,*.pem)",                        // filter
							Fl_File_Chooser::SINGLE,     // chooser type
							"Select the Certificate file");        // title
						chooser.preview(0);//no preview
						chooser.show();
						// Block until user picks something.
						while (chooser.shown())
						{
							Fl::wait();
						}
						if (chooser.value() == NULL) return;//cancel was hit
						p->_FullPathToCertificate->value(chooser.value());
						p->config->FullPathToCertificate = chooser.value();
					}
					static void _FullPathToPrivateKeyCB(Fl_Widget*w, void*data) {
						UNUSED(w);
						auto p = (ConnectionInfoWindowImpl*)data;
						Fl_File_Chooser chooser(".",                        // directory
							"Private Key Files (*.key,*.pem)",                        // filter
							Fl_File_Chooser::SINGLE,     // chooser type
							"Select the Private Key file");        // title
						chooser.preview(0);//no preview
						chooser.show();
						// Block until user picks something.
						while (chooser.shown())
						{
							Fl::wait();
						}
						if (chooser.value() == NULL) return;//cancel was hit
						p->_FullPathToPrivateKey->value(chooser.value());
						p->config->FullPathToPrivateKey = chooser.value();
					}

					static void _PasswordToPrivateKeyCB(Fl_Widget*w, void*data) {
						UNUSED(w);
						auto p = (ConnectionInfoWindowImpl*)data;
						p->config->PasswordToPrivateKey = p->_PasswordToPrivateKey->value();
					}

					


					void Init() {
						auto colwidth = 500;
						auto startleft = 200;
						auto workingy = 0;
						cWindow = new Fl_Window(400, 400, colwidth, 300, "Server Settings");
#ifdef WIN32
						cWindow->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
#endif

						_MenuBar = new Fl_Menu_Bar(0, 0, cWindow->w(), 30);
						_MenuBar->add("File/Quit", 0, Menu_CB, (void*)this);
						_MenuBar->add("Certificate/Generate", 0, GenerateCerts, (void*)this);
						_MenuBar->add("File/Log", 0, Menu_CB, (void*)this);
						workingy += 30;

						_GrayScaleImage = new Fl_Check_Button(startleft, workingy, colwidth - startleft, 20, " GrayScale");
						_GrayScaleImage->align(FL_ALIGN_LEFT);
						_GrayScaleImage->callback(SGrayScaleImageCB, this);
						_GrayScaleImage->value(config->SendGrayScaleImages == 1);
						workingy += 24;

						_IgnoreIncomingMouse = new Fl_Check_Button(startleft, workingy, colwidth - startleft, 20, " Ignore Incoming Mouse");
						_IgnoreIncomingMouse->tooltip("When this is checked, mouse commands send in will be ignored.");
						_IgnoreIncomingMouse->align(FL_ALIGN_LEFT);
						_IgnoreIncomingMouse->callback(SIgnoreIncomingMouseCB, this);
						_IgnoreIncomingMouse->value(config->IgnoreIncomingMouseEvents == 1);
						workingy += 24;

						_IgnoreIncomingKeyboard = new Fl_Check_Button(startleft, workingy, colwidth - startleft, 20, " Ignore Incoming Keyboard");
						_IgnoreIncomingKeyboard->tooltip("When this is checked, mouse commands send in will be ignored.");
						_IgnoreIncomingKeyboard->align(FL_ALIGN_LEFT);
						_IgnoreIncomingKeyboard->callback(SIgnoreIncomingKeyboardCB, this);
						_IgnoreIncomingKeyboard->value(config->IgnoreIncomingKeyboardEvents == 1);
						workingy += 24;

						_ImageQualitySlider = new SliderInput(startleft, workingy, colwidth - startleft, 20, " Image Quality Level");
						_ImageQualitySlider->tooltip("This is the quality level used by the system for images");
						_ImageQualitySlider->align(FL_ALIGN_LEFT);
						_ImageQualitySlider->bounds(10, 100);
						_ImageQualitySlider->callback(_ImageQualitySliderCB, this);
						_ImageQualitySlider->value(config->ImageCompressionSetting);
						workingy += 24;

						_MouseCaptureRateSlider = new SliderInput(startleft, workingy, colwidth - startleft, 20, " Mouse Capture Rate");
						_MouseCaptureRateSlider->tooltip("This controls the rate at which the mouse Image is captured. Measured in Seconds");
						_MouseCaptureRateSlider->align(FL_ALIGN_LEFT);
						_MouseCaptureRateSlider->bounds(1, 5);
						_MouseCaptureRateSlider->callback(_MouseCaptureRateSliderCB, this);
						_MouseCaptureRateSlider->value(config->MouseImageCaptureRate / 1000);
						workingy += 24;

						_MousePositionCaptureRate = new SliderInput(startleft, workingy, colwidth - startleft, 20, " Mouse Movement Capture");
						_MousePositionCaptureRate->tooltip("This controls how often the mouse is checked for movement. Measured in Milliseconds");
						_MousePositionCaptureRate->align(FL_ALIGN_LEFT);
						_MousePositionCaptureRate->bounds(50, 1000);
						_MousePositionCaptureRate->callback(_MousePositionCaptureRateCB, this);
						_MousePositionCaptureRate->value(config->MousePositionCaptureRate);
						workingy += 24;

						_ScreenCaptureRate = new SliderInput(startleft, workingy, colwidth - startleft, 20, " Screen Capture Rate");
						_ScreenCaptureRate->tooltip("This controls how often the screen is captured. Measured in milliseconds");
						_ScreenCaptureRate->align(FL_ALIGN_LEFT);
						_ScreenCaptureRate->bounds(100, 1000);
						_ScreenCaptureRate->callback(_ScreenCaptureRateCB, this);
						_ScreenCaptureRate->value(config->ScreenImageCaptureRate);
						workingy += 24;

					
						_FullPathToCertificate = new Fl_Input(startleft, workingy, colwidth - startleft, 20, "Path to Certificate: ");
						_FullPathToCertificate->tooltip("This is the full path to the certificate file");
						_FullPathToCertificate->align(FL_ALIGN_LEFT);
						_FullPathToCertificate->readonly(1);
						_FullPathToCertificate->callback(_FullPathToCertificateCB, this);
						workingy += 24;

						_FullPathToPrivateKey = new Fl_Input(startleft, workingy, colwidth - startleft, 20, "Path to Private Key: ");
						_FullPathToPrivateKey->tooltip("This is the full path to the private key file");
						_FullPathToPrivateKey->align(FL_ALIGN_LEFT);
						_FullPathToPrivateKey->readonly(1);
						_FullPathToPrivateKey->callback(_FullPathToPrivateKeyCB,this);
						workingy += 24;

						_PasswordToPrivateKey = new Fl_Secret_Input(startleft, workingy, colwidth - startleft, 20, "Private Key Password: ");
						_PasswordToPrivateKey->tooltip("This is the password needed to open the Private Keyfile");
						_PasswordToPrivateKey->align(FL_ALIGN_LEFT);
						_PasswordToPrivateKey->callback(_PasswordToPrivateKeyCB, this);
						workingy += 24;

						workingy += 30;
						StartStopBtn = new Fl_Button(0, cWindow->h() - 30, cWindow->w(), 30, "Start");
						StartStopBtn->callback(toggle_service, this);
						StartStopBtn->color(FL_GREEN);
						workingy = StartStopBtn->h() + 4;

						Fl_Tooltip::enable();
						cWindow->end();
						cWindow->show();
						_LogWindow = std::make_unique<LogWindow>();
						_LogWindow->set_MaxLines(100);
					}

				};
			}
		}
	}
}


SL::Remote_Access_Library::Server::UI::ConnectionInfoWindow::ConnectionInfoWindow()
{
	_ConnectWindowImpl = new ConnectionInfoWindowImpl();

}
SL::Remote_Access_Library::Server::UI::ConnectionInfoWindow::~ConnectionInfoWindow()
{
	delete _ConnectWindowImpl;
}
void SL::Remote_Access_Library::Server::UI::ConnectionInfoWindow::Init()
{
	if (_ConnectWindowImpl) _ConnectWindowImpl->Init();
}
