
#include <thread>
#include <FL\Fl.H>
int main() {

	Fl::event_length();
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	return 0;
}