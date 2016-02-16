#include "stdafx.h"
#include "ViewerWindow.h"
#include "Image.h"
#include "ClientNetworkDriver.h"
#include "Packet.h"
#include "ISocket.h"
#include "IClientDriver.h"
#include <FL\Fl.H>
#include <FL\Fl_Double_Window.H>
#include <FL\fl_draw.H>
#include <FL\Fl_Box.H>
#include <FL\Fl_Scroll.H>
#include <FL\Fl_RGB_Image.H>


namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class MyCanvas : public Fl_Widget {
			public:
				std::shared_ptr<Utilities::Image> _Image;
				MyCanvas(int X, int Y, int W, int H, const char*L = 0) : Fl_Widget(X, Y, W, H, L) {
				}
				virtual void draw() override {
					if (_Image) {
						fl_draw_image((uchar*)_Image->data(), x(), y(), _Image->Width(), _Image->Height(), 4);
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
				Network::SocketStats LastStats;
				bool _BeingClosed = false;
				static void window_cb(Fl_Widget *widget, void *)
				{
					auto wnd = (ViewerWindowImpl*)widget;
					wnd->Close();
				}
				ViewerWindowImpl(const char*  dst_host, const char*  dst_port) :Fl_Double_Window(900, 700, "Remote Host"), _ClientNetworkDriver(this, dst_host, dst_port)
				{
					callback(window_cb);
					_Fl_Scroll = new Fl_Scroll(0, 0, 900, 700);

					_MyCanvas = new MyCanvas(0, 0, 900, 700, nullptr);
					_Fl_Scroll->end();
					end();
					resizable(_MyCanvas);
					show();

				}
				virtual ~ViewerWindowImpl() {
					std::cout << "~MainWindowImpl() " << std::endl;
					_ClientNetworkDriver.Stop();
				}
				virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) override
				{
					std::cout << "MainWindowImpl::OnConnect " << std::endl;
				}

				virtual void OnReceive(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Network::Packet>& packet) override
				{

				}

				virtual void OnClose(const std::shared_ptr<Network::ISocket>& socket) override
				{
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
					((ViewerWindowImpl*)data)->redraw();
				}
				virtual void OnReceive_Image(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Utilities::Image>& img) override
				{
					_MyCanvas->_Image = img;
					_MyCanvas->resize(0, 0, img->Width(), img->Height());
					Fl::awake(awakenredraw, this);

				}
				virtual void OnReceive_ImageDif(const std::shared_ptr<Network::ISocket>& socket, Utilities::Rect* rect, std::shared_ptr<Utilities::Image>& img) override {
					Utilities::Image::Copy(*img, Utilities::Rect(Utilities::Point(0, 0), (int)img->Height(), (int)img->Width()), *_MyCanvas->_Image, *rect);
					Fl::awake(awakenredraw, this);

					if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _NetworkStatsTimer).count() > 1000) {
						_NetworkStatsTimer = std::chrono::steady_clock::now();
						auto stats = socket->get_SocketStats();
						std::string st = "Client ";
						st += std::to_string((stats.NetworkBytesReceived - LastStats.NetworkBytesReceived) / 1000) + " Kbs Received ";
						st += std::to_string((stats.NetworkBytesSent - LastStats.NetworkBytesSent) / 1000) + " Kbs Sent";
						LastStats = stats;
						label(st.c_str());
					}

				}

				// Inherited via IClientDriver
				virtual void OnReceive_MouseLocation(const std::shared_ptr<Network::ISocket>& socket, Utilities::Point & pos) override
				{

				}
				virtual void OnReceive_MouseImage(const std::shared_ptr<Network::ISocket>& socket, std::shared_ptr<Utilities::Image>& img) override
				{

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
