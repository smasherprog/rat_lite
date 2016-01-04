#include "stdafx.h"
#include "ConnectWindow.h"

SL::Remote_Access_Library::UI::ConnectWindow::ConnectWindow(std::function<void(std::string)> connfunc, std::function<void()> onquit)
	: wxFrame(NULL, wxID_ANY, "Connect to host", wxPoint(wxID_ANY, wxID_ANY), wxSize(340, 150)),
	_ConnectTo(connfunc),
	_OnQuit(onquit)
{
	auto panel = new wxPanel(this, wxID_ANY);

	auto vbox = new wxBoxSizer(wxVERTICAL);
	auto hbox1 = new wxBoxSizer(wxHORIZONTAL);
	auto m_serverLabel = new wxStaticText(panel, wxID_ANY, wxT("Server: "));
	hbox1->Add(m_serverLabel, 0);

	m_serverEntry = new wxTextCtrl(panel, wxID_ANY);
	hbox1->Add(m_serverEntry, 1);
	vbox->Add(hbox1, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);


	wxBoxSizer *hbox3 = new wxBoxSizer(wxHORIZONTAL);
	auto newid = wxWindow::NewControlId();
	auto m_buttonLogin = new wxButton(panel, newid, wxT("Connect"));
	hbox3->Add(m_buttonLogin);
	Connect(newid, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SL::Remote_Access_Library::UI::ConnectWindow::OnConnect));

	auto m_buttonQuit = new wxButton(panel, wxID_EXIT, ("Quit"));
	hbox3->Add(m_buttonQuit);
	vbox->Add(hbox3, 0, wxALIGN_RIGHT | wxTOP | wxRIGHT | wxBOTTOM, 10);
	Connect(wxID_EXIT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SL::Remote_Access_Library::UI::ConnectWindow::OnQuit));

	panel->SetSizer(vbox);
	Centre();
}

void SL::Remote_Access_Library::UI::ConnectWindow::OnQuit(wxCommandEvent & event)
{
	_OnQuit();
}

void SL::Remote_Access_Library::UI::ConnectWindow::OnConnect(wxCommandEvent & event)
{
	if (m_serverEntry->IsEmpty()) wxMessageBox(wxT("Server must not be emmpty!"), wxT("Warning!"), wxICON_WARNING);
	else {
		_ConnectTo(m_serverEntry->GetValue().ToStdString());
	}

}
