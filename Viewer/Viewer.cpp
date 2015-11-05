// Viewer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "..\Core\CommonNetwork.h"
#include "..\Core\Socket.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Toggle_Button.H>

class double_blink_window : public Fl_Double_Window {

public:
	double_blink_window(int x, int y, int w, int h, const char *l)
		: Fl_Double_Window(x, y, w, h, l) {
		resizable(this);
	}
};
SL::Remote_Access_Library::Network::NetworkEvents netevents;
std::shared_ptr<SL::Remote_Access_Library::Network::Socket> sock;

void button_cb(Fl_Widget *, void *) {
	sock = SL::Remote_Access_Library::Network::Socket::ConnectTo("127.0.0.1", "6000", netevents);
}


int main(int argc, char **argv) {
	netevents.OnConnect = [](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) { std::cout << "Connected" << std::endl; };
	netevents.OnReceive = [](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac) { std::cout << "OnReceive" << std::endl; };
	netevents.OnClose = [](const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) { std::cout << "Disconnected" << std::endl; };

	if (!Fl::visual(FL_DOUBLE))
		printf("Xdbe not supported, faking double buffer with pixmaps.\n");
	double_blink_window w2(10, 10, 400, 400, "Viewer Placeholder");
	auto overb = new Fl_Toggle_Button(25, 75, 100, 25, "text over");
	overb->callback(button_cb);
	w2.end();
	w2.show();
	return Fl::run();
}
