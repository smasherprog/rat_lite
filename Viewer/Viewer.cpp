// Viewer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "..\Core\MainWindow.h"

int main(int argc, char **argv) {

	SL::Remote_Access_Library::UI::MainWindow wnd(10, 10, 400, 400, "Viewer Placeholder");
	wnd.show();
	return Fl::run();
}
