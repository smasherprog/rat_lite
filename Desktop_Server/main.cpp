
#include <FL/Fl.H>
#include "ConnectionInfoWindow.h"

int main(int argc, char **argv) {
	Fl::args(argc, argv);
	Fl::get_system_colors();
	Fl::visual(FL_RGB);
	SL::Remote_Access_Library::UI::ConnectionInfoWindow c;
	c.Init();
	Fl::lock();

	
	while (Fl::wait() > 0) {
		if (Fl::thread_message()) {

		}
	}
	return 0;
}