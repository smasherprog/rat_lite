#include "stdafx.h"
#include "MainWindow.h"
#include "Image.h"
#include <FL/fl_draw.H>

void button_cb(Fl_Widget *w, void *d) {
	auto wnd = (SL::Remote_Access_Library::UI::MainWindow*)d;
	wnd->Connect();
}

SL::Remote_Access_Library::UI::MainWindow::MainWindow(int x, int y, int w, int h, const char * l) : Fl_Double_Window(x, y, w, h, l)
{
	resizable(this);
	netevents.OnReceive = std::bind(&SL::Remote_Access_Library::UI::MainWindow::onreceive, this, std::placeholders::_1, std::placeholders::_2);
	scroller = new Fl_Scroll(0, 0, w, h);
	overb = new Fl_Button(5, 5, 100, 25, "Connect");
	overb->callback(button_cb, this);
	end();
	Fl::visual(FL_RGB);
}
void SL::Remote_Access_Library::UI::MainWindow::draw() {
	
	if(Img)	fl_draw_image((const uchar*)Img->data(), 0, 0, Img->Width(), Img->Height(), 3, Img->Width()*3);
	else Fl_Window::draw();
}


void SL::Remote_Access_Library::UI::MainWindow::Update(Network::Commands::ImageChange * info, char * data, unsigned int len)
{
	Img = Remote_Access_Library::Utilities::Image::CreateImage(info->height, info->width, data, len, true);
	Img->decompress();
	redraw();
}
void SL::Remote_Access_Library::UI::MainWindow::Connect()
{
	if (sock) {
		sock->close();
	}
	else {
		sock = SL::Remote_Access_Library::Network::Socket::ConnectTo("127.0.0.1", "6000", netevents);
	}
}

void SL::Remote_Access_Library::UI::MainWindow::onreceive(const SL::Remote_Access_Library::Network::Socket* s, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& p) {
	if (p->header()->Packet_Type == SL::Remote_Access_Library::Network::Commands::PACKET_TYPES::IMAGECHANGE) {
		Update((SL::Remote_Access_Library::Network::Commands::ImageChange*)p->data(), p->data() + sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange), p->header()->PayloadLen - sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange));
	}
}

