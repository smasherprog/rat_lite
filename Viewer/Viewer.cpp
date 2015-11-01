// Viewer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <FL/Fl.H>
#include <FL/Fl_Single_Window.H>
#include <FL/Fl_Double_Window.H>

class double_blink_window : public Fl_Double_Window {

public:
	double_blink_window(int x, int y, int w, int h, const char *l)
		: Fl_Double_Window(x, y, w, h, l) {
		resizable(this);
	}
};

int main(int argc, char **argv) {
	if (!Fl::visual(FL_DOUBLE))
		printf("Xdbe not supported, faking double buffer with pixmaps.\n");
	double_blink_window w2(10, 10, 400, 400, "Viewer Placeholder");
	w2.end();
	w2.show();
	return Fl::run();
}
