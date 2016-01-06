// Viewer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "..\UI\Viewer.h"

//main entry point for this application, this is a wxWixgets implementation. The macro expands to the correct entry point for each platform. 
//On Windows it expands to int WinMain(){} It calls the Oninit Function defined in the class below
wxIMPLEMENT_APP(SL::Remote_Access_Library::UI::Viewer);
