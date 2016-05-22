// Viewer.cpp : Defines the entry point for the console application.
//

#include <FL/Fl.H>
#include "../Core/ConnectWindow.h"

int main(int argc, char **argv) {
	Fl::args(argc, argv);
	Fl::get_system_colors();
	Fl::visual(FL_RGB);
	SL::Remote_Access_Library::UI::ConnectWindow c;
	c.Init();
	Fl::lock();
	while (Fl::wait() > 0) {
		if (Fl::thread_message()) {

		}
	}
	return 0;
}