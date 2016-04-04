#include "stdafx.h"
#include "ViewerWindow.h"
#include "../Core/Image.h"
#include "../Core/ClientNetworkDriver.h"
#include "../Core/Packet.h"
#include "../Core/ISocket.h"
#include "../Core/IClientDriver.h"
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_PNG_Image.H>
#include "../Core/Mouse.h"
#include "../Core/stdafx.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class MyCanvas : public Fl_Widget {
			public:
				std::shared_ptr<Utilities::Image> _Image;
				Utilities::Point _MousePos;
				std::shared_ptr<Utilities::Image> _MouseImageData;
				std::shared_ptr<Fl_RGB_Image> _MouseImage;
				MyCanvas(int X, int Y, int W, int H, const char*L = 0) : Fl_Widget(X, Y, W, H, L) {
				}
				virtual void draw() override {
					if (_Image) {
						fl_draw_image((uchar*)_Image->data(), x(), y(), _Image->Width(), _Image->Height(), 4);
					}
					if (_MouseImage) {
						_MouseImage->draw( _MousePos.X, _MousePos.Y);
					}
				}
			};
			class ViewerWindowImpl : public Fl_Double_Window, public Network::IClientDriver
			{
			public:
				MyCanvas* _MyCanvas;
				//Fl_RGB_Image* _Fl_RGB_Image = nullptr;
				Fl_Scroll* _Fl_Scroll = nullptr;
				std::mutex _ImageLock;
				Network::ClientNetworkDriver _ClientNetworkDriver;


				std::chrono::time_point<std::chrono::steady_clock> _NetworkStatsTimer;
				std::chrono::time_point<std::chrono::steady_clock> _FrameTimer;
				float MaxFPS = 30.0f;
				int FPS = 0;
				int FrameCounter = 0;
				Network::SocketStats LastStats;
				bool _BeingClosed = false;
				bool _HasFocus = false;
				bool _CursorHidden = false;
				char _Title[255];
				
				static void window_cb(Fl_Widget *widget, void *)
				{
					auto wnd = (ViewerWindowImpl*)widget;
					wnd->Close();
				}
				ViewerWindowImpl(const char*  dst_host, const char*  dst_port) :Fl_Double_Window(900, 700, "Remote Host"), _ClientNetworkDriver(this, dst_host, dst_port)
				{
					_FrameTimer = _NetworkStatsTimer = std::chrono::steady_clock::now();
					callback(window_cb);
					_Fl_Scroll = new Fl_Scroll(0, 0, 900, 700);

					_MyCanvas = new MyCanvas(0, 0, 900, 700, nullptr);
					_Fl_Scroll->end();
					end();
					resizable(_MyCanvas);
					show();

				}

				virtual int handle(int e) override {
					switch (e) {

					case FL_PUSH:
					case FL_RELEASE:
					case FL_DRAG:
					case FL_MOVE:
						handle_mouse(e, Fl::event_button(), Fl::event_x(), Fl::event_y());
						break;
					case FL_FOCUS:
						_HasFocus = true;
						break;
					case FL_UNFOCUS:
						_HasFocus = false;
						break;
					};
					return Fl_Window::handle(e);
				}
				void handle_mouse(int event, int button, int x, int y) {
					if (!_HasFocus && _CursorHidden) {
						this->cursor(Fl_Cursor::FL_CURSOR_ARROW);
						_CursorHidden = false;
					}
				}
				virtual ~ViewerWindowImpl() {
					std::cout << "~MainWindowImpl() " << std::endl;
					_ClientNetworkDriver.Stop();
				}
				virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) override
				{
					UNUSED(socket);
					std::cout << "MainWindowImpl::OnConnect " << std::endl;
				}

				virtual void OnReceive(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Network::Packet>& packet) override
				{
					UNUSED(socket);
					UNUSED(packet);
				}

				virtual void OnClose(const std::shared_ptr<Network::ISocket>& socket) override
				{
					UNUSED(socket);
					Close();
				}
				void Close() {
					if (!_BeingClosed) {
						Fl::delete_widget(this);
						//this->hide();
					}
					_BeingClosed = true;
				}
				static void awakenredraw(void* data) {
					auto imp = ((ViewerWindowImpl*)data);
					if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - imp->_FrameTimer).count() > imp->MaxFPS / 1000.0f) {
						imp->_FrameTimer = std::chrono::steady_clock::now();
						imp->FrameCounter += 1;
						if (!imp->_BeingClosed) imp->redraw();
					}
				}
			
				virtual void OnReceive_Image(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Utilities::Image>& img) override
				{
					UNUSED(socket);
					_MyCanvas->_Image = img;
					_MyCanvas->resize(0, 0, img->Width(), img->Height());
					Fl::awake(awakenredraw, this);

				}
				static void awakensettitle(void* data) {
					auto imp = ((ViewerWindowImpl*)data);
					imp->label(imp->_Title);
				}
				virtual void OnReceive_ImageDif(const std::shared_ptr<Network::ISocket>& socket, Utilities::Rect* rect, std::shared_ptr<Utilities::Image>& img) override {
					if (!_MyCanvas->_Image) return;
					Utilities::Image::Copy(*img, Utilities::Rect(Utilities::Point(0, 0), (int)img->Height(), (int)img->Width()), *_MyCanvas->_Image, *rect);
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
						if(st.size()>sizeof(_Title)-1) st = st.substr(0, sizeof(_Title)-1);
						memcpy(_Title, st.c_str(), st.size()+1);
						Fl::awake(awakensettitle, this);
					}

				}
				static void awakenhidecursor(void* data) {
					auto imp = ((ViewerWindowImpl*)data);
					imp->cursor(Fl_Cursor::FL_CURSOR_NONE);
				}
				virtual void OnReceive_MouseImage(const std::shared_ptr<Network::ISocket>& socket, Utilities::Point* point, std::shared_ptr<Utilities::Image>& img)override {
					if (_HasFocus && !_CursorHidden) {
						Fl::awake(awakenhidecursor, this);
						_CursorHidden = true;
					}
					_MyCanvas->_MouseImageData = img;
					_MyCanvas->_MouseImage = std::make_shared<Fl_RGB_Image>((uchar*)img->data(), point->X, point->Y, 4);
					Fl::awake(awakenredraw, this);
				}
				virtual void OnReceive_MousePos(const std::shared_ptr<Network::ISocket>& socket, Utilities::Point* pos)override {
					_MyCanvas->_MousePos = *pos;
				}
			};

		}
	}
}


SL::Remote_Access_Library::UI::ViewerWindow::ViewerWindow(const char * dst_host, const char * dst_port) {
	_ViewerWindowImpl = new ViewerWindowImpl(dst_host, dst_port);
	_ViewerWindowImpl->_ClientNetworkDriver.Start();
}

SL::Remote_Access_Library::UI::ViewerWindow::~ViewerWindow() {
}
