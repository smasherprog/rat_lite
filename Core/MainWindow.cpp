#include "stdafx.h"
#include "MainWindow.h"
#include "Image.h"


void button_cb(Fl_Widget *w, void *d) {
	auto wnd = (SL::Remote_Access_Library::UI::MainWindow*)d;
	wnd->Connect();
}

SL::Remote_Access_Library::UI::MainWindow::MainWindow(int x, int y, int w, int h, const char * l) : Fl_Double_Window(x, y, w, h, l)
{
	resizable(this);
	netevents.OnReceive = std::bind(&SL::Remote_Access_Library::UI::MainWindow::onreceive, this, std::placeholders::_1, std::placeholders::_2);
	scroller = new Fl_Scroll(0, 0, w, h);
	auto overb = new Fl_Button(5, 5, 100, 25, "Connect");
	overb->callback(button_cb, this);

	mypicturebox = new Fl_Box(20, 20, w-30, h-30);

	resizable(mypicturebox);
	end();
}

void SL::Remote_Access_Library::UI::MainWindow::Update(Network::Commands::ImageChange * info, char * data, unsigned int len)
{
	auto img = Remote_Access_Library::Utilities::Image::CreateImage(info->height, info->width, data, len, true);
	img->decompress();

	/*auto src = (rgba*)p->data();

	need to swizzle the data
	for (size_t i = 0; i < curimage.size(); i++)
	{
		curimage[i].b = src[i].r;
		curimage[i].g = src[i].g;
		curimage[i].r = src[i].b;
		curimage[i].a = 0;
	}*/
	curimage.resize(img->Height()*img->Width());
	memcpy(curimage.data(), img->data(), img->size());
	
	if (myimage != nullptr) {
		delete myimage;
	}
	myimage = new Fl_RGB_Image((unsigned char*)curimage.data(), info->width, info->height, 4);
	mypicturebox->size(info->width, info->height);
	mypicturebox->image(myimage);

	mypicturebox->redraw();

}
void SL::Remote_Access_Library::UI::MainWindow::Connect()
{
	if (sock)sock->close();
	sock = SL::Remote_Access_Library::Network::Socket::ConnectTo("127.0.0.1", "6000", netevents);
}

void SL::Remote_Access_Library::UI::MainWindow::onreceive(const SL::Remote_Access_Library::Network::Socket* s, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& p) {
	if (p->header()->Packet_Type == SL::Remote_Access_Library::Network::Commands::PACKET_TYPES::IMAGECHANGE) {
		Update((SL::Remote_Access_Library::Network::Commands::ImageChange*)p->data(), p->data() + sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange), p->header()->PayloadLen - sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange));
	}
}

