// Viewer.cpp : Defines the entry point for the console application.
//

#include <FL/Fl.H>
#include "ConnectWindow.h"

int main(int argc, char **argv) {
	Fl::args(argc, argv);
	Fl::get_system_colors();
	Fl::visual(FL_RGB);
	SL::Remote_Access_Library::UI::ConnectWindow c;
	c.Init();
	Fl::lock();
	int left(0), top(0), width(0), height(0);
	for (auto i = 0; i < Fl::screen_count(); i++) {
		auto l(0), t(0), w(0), h(0);
		Fl::screen_xywh(l, t, w, h, i);
		width += w;
	}
	printf(" out %d", width);
	while (Fl::wait() > 0) {
		if (Fl::thread_message()) {

		}
	}
	return 0;
}