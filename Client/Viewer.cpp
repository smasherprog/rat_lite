// Viewer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <functional>
#include "..\Core\ConnectWindow.h"

class Viewer : public wxApp {

public:

	Viewer() {}

	virtual ~Viewer() {}

	virtual bool OnInit() override
	{
		wxInitAllImageHandlers();
		auto _ConnectWindow = new SL::Remote_Access_Library::UI::ConnectWindow();
		_ConnectWindow->get_Frame()->Show();
		return true;
	}
};




//main entry point for this application, this is a wxWixgets implementation. The macro expands to the correct entry point for each platform. 
//On Windows it expands to int WinMain(){} It calls the Oninit Function defined in the class specified
wxIMPLEMENT_APP(Viewer);
