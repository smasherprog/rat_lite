#include "stdafx.h"
#include "Viewer.h"
#include "ConnectWindow.h"

SL::Remote_Access_Library::UI::Viewer::Viewer()
{

}


bool SL::Remote_Access_Library::UI::Viewer::OnInit()
{
	wxInitAllImageHandlers();
	auto _ConnectWindow = new ConnectWindow();
	_ConnectWindow->get_Frame()->Show();
	return true;
}
