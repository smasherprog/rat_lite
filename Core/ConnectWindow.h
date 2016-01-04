#pragma once
#include <string>
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <functional>

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {


			class ConnectWindow : public wxFrame
			{
			public:
				ConnectWindow(std::function<void(std::string)> connfunc, std::function<void()> onquit);
				virtual ~ConnectWindow() {}
			private:
				void OnQuit(wxCommandEvent& event);
				void OnConnect(wxCommandEvent& event);
				std::function<void(std::string)> _ConnectTo;
				std::function<void()> _OnQuit;
				wxTextCtrl* m_serverEntry;
			};


		}
	}
}
