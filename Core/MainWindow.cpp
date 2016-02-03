#include "stdafx.h"
#include "MainWindow.h"
#include "Image.h"
#include "ClientNetworkDriver.h"
#include "Commands.h"
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
			class MainWindowImpl : public Fl_Double_Window, public Network::IClientDriver
			{
			public:
				MyCanvas* _MyCanvas;
				//Fl_RGB_Image* _Fl_RGB_Image = nullptr;
				Fl_Scroll* _Fl_Scroll = nullptr;
				std::mutex _ImageLock;
				Network::ClientNetworkDriver _ClientNetworkDriver;


				std::chrono::time_point<std::chrono::steady_clock> _NetworkStatsTimer;
				SL::Remote_Access_Library::Network::SocketStats LastStats;
				bool _BeingClosed = false;


				MainWindowImpl(const char*  dst_host, const char*  dst_port) :Fl_Double_Window(900, 700, "Remote Host"), _ClientNetworkDriver(this, dst_host, dst_port)
				{
					_Fl_Scroll = new Fl_Scroll(0, 0, 900, 700);

					_MyCanvas = new MyCanvas(0, 0, 900, 700, nullptr);
					_Fl_Scroll->end();
					end();
					resizable(_MyCanvas);
					show();
				}
				virtual ~MainWindowImpl() {
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
					if (!_BeingClosed) Fl::delete_widget(this);
					_BeingClosed = true;
				}

				static void awakenredraw(void* data) {
					((MainWindowImpl*)data)->redraw();
				}
				virtual void OnReceive_Image(const std::shared_ptr<Network::ISocket>& socket, Utilities::Rect * rect, std::shared_ptr<Utilities::Image>& img) override
				{
					if (!_MyCanvas->_Image) {
						_MyCanvas->_Image = img;
						_MyCanvas->resize(0, 0, img->Width(), img->Height());
					}
					else {
						Utilities::Image::Copy(*img, Utilities::Rect(Utilities::Point(0, 0), (int)img->Height(), (int)img->Width()), *_MyCanvas->_Image, *rect);
					}

					Fl::awake(awakenredraw, this);

					/*			if (std::chrno::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _NetworkStatsTimer).count() > 1000) {
									_NetworkStatsTimer = std::chrono::steady_clock::now();
									auto stats = socket->get_SocketStats();
									wxString st = "Client ";
									st += std::to_string((stats.NetworkBytesReceived - LastStats.NetworkBytesReceived) / 1000) + " kbs Received ";
									st += std::to_string((stats.NetworkBytesSent - LastStats.NetworkBytesSent) / 1000) + " kbs Sent";
									LastStats = stats;
									static_cast<wxFrame*>(GetParent())->SetTitle(st);
								}*/

				}
			};

		}
	}
}


SL::Remote_Access_Library::UI::MainWindow::MainWindow(const char * dst_host, const char * dst_port) {
	_MainWindowImpl = new MainWindowImpl(dst_host, dst_port);
	_MainWindowImpl->_ClientNetworkDriver.Start();
}

SL::Remote_Access_Library::UI::MainWindow::~MainWindow() {
}
