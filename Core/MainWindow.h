#pragma once

#include "Commands.h"
#include <vector>
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <mutex>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image;
		}
		namespace Network {
			class Socket;
			class Packet;
		};
		namespace UI {

			class MainWindow : public wxScrolledWindow
			{
			public:

				MainWindow(wxFrame* frame, const wxString& title, std::string dst_host, std::string dst_port, std::function<void()> ondisconnect);
				virtual ~MainWindow();

			private:


				std::function<void()> _OnDisconnect;
				std::shared_ptr<Network::Socket> _Socket;
				std::shared_ptr<wxBitmap> _Image;

				virtual void OnDraw(wxDC& dc) override;
				void NewImage(Network::Commands::ImageChange* info, std::shared_ptr<Utilities::Image>& img);
				void Update(Network::Commands::ImageChange* info, std::shared_ptr<Utilities::Image>& img);
				void OnReceive(const Network::Socket* s, std::shared_ptr<Network::Packet>& p);
			};


		}
	}
}
