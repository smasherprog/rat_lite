// Viewer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "..\Core\Viewer.h"
#include <FL\Fl.H>

int main(int argc, char **argv) {

	SL::Remote_Access_Library::UI::Viewer wnd;
	return Fl::run();
}
