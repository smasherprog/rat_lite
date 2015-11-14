// Viewer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "..\Core\CommonNetwork.h"
#include "..\Core\Socket.h"
#include "..\Core\Packet.h"
#include "..\Core\ThreadPool.h"

#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/FL_Box.H>
#include <FL/Fl_RGB_Image.H>

SL::Remote_Access_Library::Utilities::ThreadPool THPool;

class double_blink_window : public Fl_Double_Window {

public:
	double_blink_window(int x, int y, int w, int h, const char *l)
		: Fl_Double_Window(x, y, w, h, l) {
		resizable(this);
	}
};
SL::Remote_Access_Library::Network::NetworkEvents netevents;
std::shared_ptr<SL::Remote_Access_Library::Network::Socket> sock;
Fl_Box* mypicturebox = nullptr;
Fl_RGB_Image* myimage = nullptr;
struct rgba {
	unsigned char r, g, b, a;
};
std::vector<rgba> curimage;
void button_cb(Fl_Widget *, void *) {
	if (sock)sock->close();
	sock = SL::Remote_Access_Library::Network::Socket::ConnectTo("127.0.0.1", "6000", netevents);
}

void processimage(std::shared_ptr<SL::Remote_Access_Library::Network::Packet> p) {


	curimage.resize(3840 * 1080);

	auto src = (rgba*)p->data();

	//need to swizzle the data
	for (size_t i = 0; i < curimage.size(); i++)
	{
		curimage[i].b = src[i].r;
		curimage[i].g = src[i].g;
		curimage[i].r = src[i].b;
		curimage[i].a = 0;
	}

	if (myimage == nullptr) delete myimage;
	myimage = new Fl_RGB_Image((unsigned char*)curimage.data(), 3840, 1080, 4);

	mypicturebox->image(myimage);
	mypicturebox->redraw();


}

void onreceive(const SL::Remote_Access_Library::Network::Socket* s, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& p) {

	THPool.Enqueue([p]() { processimage(p); });

}

int main(int argc, char **argv) {

	netevents.OnReceive = onreceive;
	double_blink_window w2(10, 10, 400, 400, "Viewer Placeholder");
	auto overb = new Fl_Button(5, 5, 100, 25, "Connect");
	overb->callback(button_cb);
	mypicturebox = new Fl_Box(20, 20, 370, 370);

	w2.resizable(mypicturebox);
	w2.end();
	w2.show();
	return Fl::run();
}
