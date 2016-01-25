#include "stdafx.h"
#include "MainWindow.h"
#include "Image.h"
#include "ClientNetworkDriver.h"
#include "Commands.h"
#include "ISocket.h"
#include <wx/rawbmp.h>
#include "IClientDriver.h"


namespace SL {
	namespace Remote_Access_Library {

		namespace UI {

			class MainWindowImpl : public wxScrolledWindow, public Network::IClientDriver
			{

				std::mutex _ImageLock;
				Network::ClientNetworkDriver _ClientNetworkDriver;
				std::unique_ptr<wxBitmap> Image;
				std::chrono::time_point<std::chrono::steady_clock> _NetworkStatsTimer;
				SL::Remote_Access_Library::Network::SocketStats LastStats;

			public:

				MainWindowImpl(wxFrame* parent, const char*  dst_host, const char*  dst_port) : wxScrolledWindow(parent, wxID_ANY), _ClientNetworkDriver(this, dst_host, dst_port)
				{
				
				}
				virtual ~MainWindowImpl() {
					_ClientNetworkDriver.Close();
				}
				virtual void OnConnect(const std::shared_ptr<Network::ISocket>& socket) override
				{

				}

				virtual void OnReceive(const Network::ISocket * socket, std::shared_ptr<Network::Packet>& packet) override
				{

				}

				virtual void OnClose(const Network::ISocket * socket) override
				{
					if (!this->IsBeingDeleted()) {
						wxQueueEvent(GetParent(), new wxCloseEvent(wxEVT_CLOSE_WINDOW));//this will call delete on the window 
					}

				}



				void OnEraseBackGround(wxEraseEvent& ev)
				{

				}
				virtual void OnDraw(wxDC & dc) override
				{
					std::lock_guard<std::mutex> lock(_ImageLock);
					if (Image) dc.DrawBitmap(*Image, 0, 0, false);
				}

				virtual void OnReceive_Image(const Network::ISocket * socket, Utilities::Rect * rect, std::shared_ptr<Utilities::Image>& img) override
				{
					if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _NetworkStatsTimer).count() > 1000) {
						_NetworkStatsTimer = std::chrono::steady_clock::now();
						auto stats = socket->get_SocketStats();
						wxString st = "Client ";
						st += std::to_string((stats.NetworkBytesReceived - LastStats.NetworkBytesReceived) / 1000) + " kbs Received ";
						st += std::to_string((stats.NetworkBytesSent - LastStats.NetworkBytesSent) / 1000) + " kbs Sent";
						LastStats = stats;
						static_cast<wxFrame*>(GetParent())->SetTitle(st);
					}
					auto ptr(std::make_unique<wxBitmap>(img->data(), img->Width(), img->Height(), 32));
					if (!Image) {
						{
							std::lock_guard<std::mutex> lock(_ImageLock);
							Image = std::move(ptr);
						}
						SetScrollbars(1, 1, img->Width(), img->Height(), 0, 0);
					}
					else {

						wxMemoryDC memDC;
						std::lock_guard<std::mutex> lock(_ImageLock);
						memDC.SelectObject(*Image);
						memDC.DrawBitmap(*ptr, wxPoint(rect->left(), rect->top()));

					}
					Refresh(false);

				}
				DECLARE_EVENT_TABLE()
			};

		}
	}
}

BEGIN_EVENT_TABLE(SL::Remote_Access_Library::UI::MainWindowImpl, wxScrolledWindow)
EVT_ERASE_BACKGROUND(SL::Remote_Access_Library::UI::MainWindowImpl::OnEraseBackGround)
END_EVENT_TABLE()


wxScrolledWindow * SL::Remote_Access_Library::UI::CreateMainWindow(wxFrame * parent, const char * title, const char * dst_host, const char * dst_port)
{
	//THE WXWIDGETS LIBRARY TAKES CARE OF DELETING WINDOWS... THATS WHY THERE IS NO DELETE !!!
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	auto parentframe = new wxFrame(parent, -1, title, wxPoint(50, 50), wxSize(900, 900));

	auto _MainWindowImpl = new MainWindowImpl(parentframe, dst_host, dst_port);
	sizer->Add(_MainWindowImpl, 1, wxALL | wxEXPAND);
	parentframe->SetSizer(sizer);
	parentframe->Show();
	return _MainWindowImpl;
	//THE WXWIDGETS LIBRARY TAKES CARE OF DELETING WINDOWS... THATS WHY THERE IS NO DELETE !!!
}
