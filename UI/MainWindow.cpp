#include "stdafx.h"
#include "MainWindow.h"
#include "Image.h"
#include "ReceiverNetworkDriver.h"
#include "Commands.h"
#include "Socket.h"
#include <mutex>

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class MainWindowImpl : public wxScrolledWindow
			{
				std::shared_ptr<wxBitmap> _Image;
				std::unique_ptr<Network::ReceiverNetworkDriver<MainWindowImpl>> _ReceiverNetworkDriver;
				std::mutex _CloseLock;
				wxFrame* Parent = nullptr;
			public:

				MainWindowImpl(wxFrame* frame, const wxString& title, std::string dst_host, std::string dst_port)
					: wxScrolledWindow(frame, wxID_ANY)
				{
					_ReceiverNetworkDriver = std::make_unique<Network::ReceiverNetworkDriver<MainWindowImpl>>(this, dst_host, dst_port);
					Parent = frame;
				}
				virtual ~MainWindowImpl() {
	
				}

				void Update(Network::Commands::ImageChange* info, std::shared_ptr<Utilities::Image>& img)
				{
					auto tmpImage = std::make_shared<wxBitmap>(img->data(), img->Width(), img->Height(), 32);
					//tmpImage->SaveFile("c:\\users\\scott\\desktop\\somefile.bmp", wxBitmapType::wxBITMAP_TYPE_BMP);

					_Image = tmpImage;
					SetScrollbars(1, 1, img->Width(), img->Height(), 0, 0);
					Refresh(false);
				}

				virtual void OnDraw(wxDC & dc) override
				{
					auto imgcopy = _Image;
					if (imgcopy) dc.DrawBitmap(*imgcopy, 0, 0, false);
				}

				void NewImage(Network::Commands::ImageChange* info, std::shared_ptr<Utilities::Image>& img)
				{
					Update(info, img);
				}
				void OnClose(const Network::Socket* socket) {
					wxQueueEvent(Parent, new wxCloseEvent(wxEVT_CLOSE_WINDOW));
				}
				void OnConnect(const std::shared_ptr<Network::Socket>& socket) {

				}
			
			};
		}
	}
}







SL::Remote_Access_Library::UI::MainWindow::MainWindow(wxFrame* frame, const std::string title, std::string dst_host, std::string dst_port)
{
	_MainWindowImpl = new MainWindowImpl(frame, title, dst_host, dst_port);
}

SL::Remote_Access_Library::UI::MainWindow::~MainWindow()
{//specifc order
	if (!_MainWindowImpl->IsBeingDeleted()) {
		_MainWindowImpl->Destroy();
	}
}

wxScrolledWindow * SL::Remote_Access_Library::UI::MainWindow::get_Frame()
{
	return _MainWindowImpl;
}

