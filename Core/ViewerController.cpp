#include "stdafx.h"
#include "ViewerController.h"
#include "Image.h"
#include "ClientNetworkDriver.h"
#include "Packet.h"
#include "ISocket.h"
#include "IClientDriver.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Logging.h"
#include "ImageControl.h"
#include "Client_Config.h"
#include "Clipboard.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_PNG_Image.H>

#include <chrono>
#include <mutex>
#include <assert.h>


namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class ViewerControllerImpl : public Fl_Double_Window, public Network::IClientDriver
			{
			public:
				ImageControl* _ImageControl = nullptr;

				Fl_Scroll* _Fl_Scroll = nullptr;
                
                std::shared_ptr<Network::Client_Config> _Config;
				Network::ClientNetworkDriver _ClientNetworkDriver;
				std::unique_ptr<Capturing::Clipboard> Clipboard;

				std::chrono::time_point<std::chrono::steady_clock> _NetworkStatsTimer;
				std::chrono::time_point<std::chrono::steady_clock> _FrameTimer;
				float MaxFPS = 30.0f;
				int FPS = 0;
				int FrameCounter = 0;
				Network::SocketStats LastStats;
				bool _BeingClosed = false;
				bool _CursorHidden = false;
				char _Title[255];
				bool _HasFocus = false;

				static void window_cb(Fl_Widget *widget, void *)
				{
					auto wnd = (ViewerControllerImpl*)widget;
					wnd->Close();
				}
				ViewerControllerImpl(std::shared_ptr<Network::Client_Config> config, const char* dst_host) : 
					Fl_Double_Window(900, 700, "Remote Host"), _Config(config), _ClientNetworkDriver(this, config, dst_host)
				{
					_FrameTimer = _NetworkStatsTimer = std::chrono::steady_clock::now();
					callback(window_cb);
					_Fl_Scroll = new Fl_Scroll(0, 0, 900, 700);
					ImageControlInfo info;
					info._Scroller = _Fl_Scroll;
					info._KeyCallback = [this](int e, Input::Keyboard::Press p) { this->handle_key(e, p); };
					info._MouseCallback = [this](int e, int button, Input::Mouse::Press press, int x, int y) { this->handle_mouse(e, button, press, x, y); };

					_ImageControl = new ImageControl(0, 0, 900, 700, nullptr, std::move(info));
					_Fl_Scroll->end();
					end();
					resizable(this);
					show();
					Clipboard = std::make_unique<Capturing::Clipboard>(&config->Share_Clipboard, [&](const char* c, int len) { _ClientNetworkDriver.SendClipboardText(c, static_cast<unsigned int>(len)); });
				
				}
				virtual ~ViewerControllerImpl() {
					Clipboard.reset();//need to manually do this to avoid a possible race condition with the captured reference to _ClientNetworkDriver
					_ClientNetworkDriver.Stop();
				}
				
				void handle_key(int e, Input::Keyboard::Press press) {
                    UNUSED(e);
					auto key = Fl::event_key();
					auto text = Fl::event_text();
					auto len = Fl::event_length();
					auto t = *text;
					SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "key: '" << key << "' text: '" << text << "' len: '" << len << "'");
					//make sure to map all keys to their lower case equivelents. 
					if (t >= 'A' && t < 'Z') key = static_cast<unsigned int>(t + ('a' - 'A'));					

					Input::KeyEvent k;
					k.Key = key;
					k.PressData = press;
					k.SpecialKey = Input::Keyboard::Specials::NO_PRESS_DATA;
					_ClientNetworkDriver.SendKey(k);
				}
				void handle_mouse(int e, int button, Input::Mouse::Press press, int x, int y) {
	
					auto scale = _ImageControl->GetScaleFactor();

					Input::MouseEvent ev;
					ev.Pos = Utilities::Point(static_cast<int>(static_cast<float>(x) / scale), static_cast<int>(static_cast<float>(y) / scale));
					if (e == FL_MOUSEWHEEL) {
						ev.ScrollDelta = Fl::event_dy();
						ev.EventData = Input::Mouse::Events::SCROLL;
					}
					else {
						ev.ScrollDelta = 0;
						switch (button) {
						case FL_LEFT_MOUSE:
							ev.EventData = Input::Mouse::Events::LEFT;
							break;
						case FL_MIDDLE_MOUSE:
							ev.EventData = Input::Mouse::Events::MIDDLE;
							break;
						case FL_RIGHT_MOUSE:
							ev.EventData = Input::Mouse::Events::RIGHT;
							break;
						default:
							ev.EventData = Input::Mouse::Events::NO_EVENTDATA;
							break;
						};
					}
					ev.PressData = press;
					if (!_ClientNetworkDriver.ConnectedToSelf()) {
						_ClientNetworkDriver.SendMouse(ev);//sending input to yourself will lead to an infinite loop...
					}
				}
				virtual bool ValidateUntrustedCert(const std::shared_ptr<Network::ISocket>& socket) override
				{
					UNUSED(socket);
					return true;
				}

				virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) override
				{
					UNUSED(socket);
				}

				virtual void OnReceive(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Network::Packet>& packet) override
				{
					UNUSED(socket);
					UNUSED(packet);
				}

				virtual void OnClose(const Network::ISocket* socket) override
				{
					UNUSED(socket);
					Close();
				}
		
				virtual void OnReceive_Image(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Utilities::Image>& img) override
				{
					UNUSED(socket);
					_ImageControl->SetImage(img);
					Fl::awake(awakenredraw, this);

				}

				virtual void OnReceive_ImageDif(const std::shared_ptr<Network::ISocket>& socket, Utilities::Point pos, std::shared_ptr<Utilities::Image>& img) override {
					_ImageControl->SetImageDif(pos, img);
					Fl::awake(awakenredraw, this);
					if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _NetworkStatsTimer).count() > 1000) {
						_NetworkStatsTimer = std::chrono::steady_clock::now();
						auto stats = socket->get_SocketStats();
						std::string st = "Client ";
						st += std::to_string((stats.NetworkBytesReceived - LastStats.NetworkBytesReceived) / 1000) + " Kbs Received ";
						st += std::to_string((stats.NetworkBytesSent - LastStats.NetworkBytesSent) / 1000) + " Kbs Sent";
						FPS = FrameCounter;
						FrameCounter = 0;
						st += " Fps: " + std::to_string(FPS);
						LastStats = stats;
						if (st.size() > sizeof(_Title) - 1) st = st.substr(0, sizeof(_Title) - 1);
						memcpy(_Title, st.c_str(), st.size() + 1);
						Fl::awake(awakensettitle, this);
					}

				}
				virtual void OnReceive_MouseImage(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Utilities::Image>& img)override {
					UNUSED(socket);
					if (_HasFocus && !_CursorHidden) {
						Fl::awake(awakenhidecursor, this);
						_CursorHidden = true;
					}
					_ImageControl->SetMouseImage(img);
					Fl::awake(awakenredraw, this);
				}

				virtual void OnReceive_MousePos(const std::shared_ptr<Network::ISocket>& socket, Utilities::Point* pos)override {
					UNUSED(socket);
					_ImageControl->SetMousePosition(*pos);
				}
				virtual void  OnReceive_ClipboardText(const std::shared_ptr<Network::ISocket>& socket, const char* data, unsigned int len) override {
					UNUSED(socket);
					SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "OnReceive_ClipboardText " << len);
					Fl::copy(data, static_cast<int>(len), 1);
				}


				void Close() {
					if (!_BeingClosed) {
						Fl::delete_widget(this);
						//this->hide();
					}
					_BeingClosed = true;
				}
				static void awakenredraw(void* data) {
					auto imp = ((ViewerControllerImpl*)data);
					if (imp->_BeingClosed) return;
					if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - imp->_FrameTimer).count() > imp->MaxFPS / 1000.0f) {
						imp->_FrameTimer = std::chrono::steady_clock::now();
						imp->FrameCounter += 1;
						if (!imp->_BeingClosed) imp->redraw();
					}
				}
				void ScaleView(bool b)
				{
					_ImageControl->ScaleImage(b);
				}

				static void awakensettitle(void* data) {
					auto imp = ((ViewerControllerImpl*)data);
					if (imp->_BeingClosed) return;
					imp->label(imp->_Title);
				}
				static void awakenhidecursor(void* data) {
					auto imp = ((ViewerControllerImpl*)data);
					imp->cursor(Fl_Cursor::FL_CURSOR_NONE);
				}
		
			};

		}
	}
}


SL::Remote_Access_Library::UI::ViewerController::ViewerController(std::shared_ptr<Network::Client_Config> config, const char * dst_host) {

	_ViewerControllerImpl = new ViewerControllerImpl(config, dst_host);
	_ViewerControllerImpl->_ClientNetworkDriver.Start();
}

SL::Remote_Access_Library::UI::ViewerController::~ViewerController() {

}

void SL::Remote_Access_Library::UI::ViewerController::ScaleView(bool b)
{
	_ViewerControllerImpl->ScaleView(b);
}
