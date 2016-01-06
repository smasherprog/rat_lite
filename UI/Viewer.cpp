#include "stdafx.h"
#include "Viewer.h"
#include "MainWindow.h"
#include "ConnectWindow.h"

SL::Remote_Access_Library::UI::Viewer::Viewer()
{

}

void SL::Remote_Access_Library::UI::Viewer::ConnectTo(std::string host)
{
	_ConnectWindow->get_Frame()->HideWithEffect(wxSHOW_EFFECT_BLEND);
	if (_MainWindow != nullptr) _MainWindow->Close();
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	_MainWindow = new wxFrame(NULL, -1, wxT("Scrolling an Image"), wxPoint(50, 50), wxSize(650, 650));

	auto child = new MainWindow(_MainWindow, "RDP Window", host, "6000", [this]() {
		_ConnectWindow->get_Frame()->ShowWithEffect(wxSHOW_EFFECT_BLEND);
	});
	sizer->Add(child->get_Frame(), 1, wxALL | wxEXPAND);
	_MainWindow->SetSizer(sizer);
	_MainWindow->Show();
}

bool SL::Remote_Access_Library::UI::Viewer::OnInit()
{
	wxInitAllImageHandlers();
	_ConnectWindow = new ConnectWindow(
		std::bind(&SL::Remote_Access_Library::UI::Viewer::ConnectTo, this, std::placeholders::_1),
		[this]() {
			if (_MainWindow != nullptr) _MainWindow->Close();
			if (_ConnectWindow != nullptr) _ConnectWindow->get_Frame()->Close();
		});
	_ConnectWindow->get_Frame()->Show();
	return true;
}
