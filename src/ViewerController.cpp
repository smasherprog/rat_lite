
#include "ViewerController.h"
#include "ImageControl.h"
#include "Input.h"
#include "Logging.h"
#include "ImageControl.h"
#include "Configs.h"
#include "ISocket.h"

#include "IClientDriver.h"
#include "ClientNetworkDriver.h"
#include "Clipboard.h"
#include "Shapes.h"
#include "ScreenCapture.h"

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

		class ViewerControllerImpl : public Fl_Double_Window, IClientDriver {
		public:
			ImageControl* _ImageControl = nullptr;

			Fl_Scroll* _Fl_Scroll = nullptr;
			std::shared_ptr<Client_Config> _Config;

			std::unique_ptr<Clipboard> _Clipboard;
			ClientNetworkDriver _ClientNetworkDriver;

			std::chrono::time_point<std::chrono::steady_clock> _NetworkStatsTimer;
			std::chrono::time_point<std::chrono::steady_clock> _FrameTimer;
			float MaxFPS = 30.0f;
			int FPS = 0;
			int FrameCounter = 0;

			bool _BeingClosed = false;
			bool _CursorHidden = false;
			char _Title[255];
			bool _HasFocus = false;

			static void window_cb(Fl_Widget *widget, void *)
			{
				auto wnd = (ViewerControllerImpl*)widget;
				wnd->Close();
			}
			ViewerControllerImpl(std::shared_ptr<Client_Config> config, const char* dst_host) :
				Fl_Double_Window(900, 700, "Remote Host"),
				_Config(config),
				_ClientNetworkDriver(this, config, dst_host)
			{
				_FrameTimer = _NetworkStatsTimer = std::chrono::steady_clock::now();
				callback(window_cb);
				_Fl_Scroll = new Fl_Scroll(0, 0, 900, 700);

				ScreenImageInfo info;
				info.get_Height = [this]() {return this->h(); };
				info.get_Left = [this]() {return _Fl_Scroll->xposition(); };
				info.get_Top = [this]() {return _Fl_Scroll->yposition(); };
				info.OnKey = [this](int e, Press p) { this->handle_key(e, p); };
				info.OnMouse = [this](int e, int button, Press press, int x, int y) { this->handle_mouse(e, button, press, x, y); };

				_ImageControl = new ImageControl(0, 0, 900, 700, nullptr, std::move(info));
				_Fl_Scroll->end();
				end();
				resizable(this);
				show();

				_Clipboard = std::make_unique<Clipboard>(&config->Share_Clipboard, [&](const char* c, int len) { _ClientNetworkDriver.SendClipboardText(c, static_cast<unsigned int>(len)); });

			}
			virtual ~ViewerControllerImpl() {
				_Clipboard.reset();//need to manually do this to avoid a possible race condition with the captured reference to _ClientNetworkDriver
				_ClientNetworkDriver.Stop();
			}
			virtual void resize(int X, int Y, int W, int H) override {
				Fl_Double_Window::resize(X, Y, W, H);

				_ImageControl->OnResize(W, H, _Fl_Scroll->scrollbar_size());
			}
			void handle_key(int e, Press press) {
				UNUSED(e);
				auto key = Fl::event_key();
				auto text = Fl::event_text();
				auto len = Fl::event_length();
				auto t = *text;
				SL_RAT_LOG(INFO_log_level, "key: '" << key << "' text: '" << text << "' len: '" << len << "'");
				//make sure to map all keys to their lower case equivelents. 
				if (t >= 'A' && t < 'Z') key = static_cast<unsigned int>(t + ('a' - 'A'));

				KeyEvent k;
				k.Key = key;
				k.PressData = press;
				k.SpecialKey = Specials::NO_SPECIAL_DATA;
				_ClientNetworkDriver.SendKey(k);
			}
			void handle_mouse(int e, int button, Press press, int x, int y) {
				MouseEvent ev;
				ev.Pos = Point(x, y);
				if (e == FL_MOUSEWHEEL) {
					ev.ScrollDelta = Fl::event_dy();
					ev.EventData = Events::SCROLL;
				}
				else {
					ev.ScrollDelta = 0;
					switch (button) {
					case FL_LEFT_MOUSE:
						ev.EventData = Events::LEFT;
						break;
					case FL_MIDDLE_MOUSE:
						ev.EventData = Events::MIDDLE;
						break;
					case FL_RIGHT_MOUSE:
						ev.EventData = Events::RIGHT;
						break;
					default:
						ev.EventData = Events::NO_EVENTDATA;
						break;
					};
				}
				ev.PressData = press;
				_ClientNetworkDriver.SendMouse(ev);
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
			static void awakensettitle(void* data) {
				auto imp = ((ViewerControllerImpl*)data);
				if (imp->_BeingClosed) return;
				imp->label(imp->_Title);
			}
			static void awakenhidecursor(void* data) {
				auto imp = ((ViewerControllerImpl*)data);
				imp->cursor(Fl_Cursor::FL_CURSOR_NONE);
			}

			virtual void onConnection(const std::shared_ptr<ISocket>& socket) override {
				UNUSED(socket);
			}

			virtual void onDisconnection(const ISocket* socket) override {
				UNUSED(socket);
				Close();
			}

			virtual void onMessage(const std::shared_ptr<ISocket>& socket, const char* data, size_t len)  override {
				UNUSED(socket);
				UNUSED(data);
				UNUSED(len);
			}


			virtual void OnReceive_Image(std::shared_ptr<Screen_Capture::Image>& img) override
			{
				_ImageControl->set_ScreenImage(img);
				Fl::awake(awakenredraw, this);
			}

			virtual void OnReceive_ImageDif(Point pos, std::shared_ptr<Screen_Capture::Image>& img) override {
				_ImageControl->set_ImageDifference(pos, img);
				Fl::awake(awakenredraw, this);
				if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _NetworkStatsTimer).count() > 1000) {
					_NetworkStatsTimer = std::chrono::steady_clock::now();

			
					std::string st = "Client ";
				
					FPS = FrameCounter;
					FrameCounter = 0;
					st += " Fps: " + std::to_string(FPS);
				
					if (st.size() > sizeof(_Title) - 1) st = st.substr(0, sizeof(_Title) - 1);
					memcpy(_Title, st.c_str(), st.size() + 1);
					Fl::awake(awakensettitle, this);
				}

			}
			virtual void OnReceive_MouseImage(std::shared_ptr<Screen_Capture::Image>& img)override {
				_ImageControl->set_MouseImage(img);
			}

			virtual void OnReceive_MousePos(Point* pos)override {
				_ImageControl->set_MousePosition(pos);
			}
			virtual void  OnReceive_ClipboardText(const char* data, unsigned int len) override {
				_Clipboard->copy_to_clipboard(data, len);
			}


		};
	}
}


SL::Remote_Access_Library::ViewerController::ViewerController(std::shared_ptr<Client_Config> config, const char * dst_host) {

	_ViewerControllerImpl = new ViewerControllerImpl(config, dst_host);
	_ViewerControllerImpl->_ClientNetworkDriver.Start();
}

SL::Remote_Access_Library::ViewerController::~ViewerController() {

}
