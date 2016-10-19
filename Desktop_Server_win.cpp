
#include <FL/Fl.H>
#include "../Core/ConnectionInfoWindow.h"
#include "../Core/crypto.h"

int main(int argc, char **argv) {
	Fl::args(argc, argv);
	Fl::get_system_colors();
	Fl::visual(FL_RGB);
	SL::Remote_Access_Library::Crypto::Initer encryption_startup;//will ensure proper cleanup when it is destroyed RAII . . . 
	SL::Remote_Access_Library::Server::UI::ConnectionInfoWindow c;
	c.Init();
	Fl::lock();

	
	while (Fl::wait() > 0) {
		if (Fl::thread_message()) {

		}
	}
	return 0;
}