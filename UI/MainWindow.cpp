#include "stdafx.h"
#include "MainWindow.h"
#include "Image.h"
#include "Socket.h"
#include "Packet.h"


namespace SL {
	namespace Remote_Access_Library {
		namespace UI {

			class MainWindowImpl : public wxScrolledWindow
			{
			public:

				MainWindowImpl(wxFrame* frame, const wxString& title, std::string dst_host, std::string dst_port, std::function<void()> ondisconnect)
					: wxScrolledWindow(frame, wxID_ANY), _OnDisconnect(ondisconnect)
				{
					//Network::NetworkEvents netevents;
					//netevents.OnClose = [this](const Network::Socket* sock) { _OnDisconnect(); };
					//netevents.OnReceive = std::bind(&SL::Remote_Access_Library::UI::MainWindow::OnReceive, this, std::placeholders::_1, std::placeholders::_2);
					//_Socket = SL::Remote_Access_Library::Network::Socket::ConnectTo(dst_host.c_str(), dst_port.c_str(), netevents);
				}
				virtual ~MainWindowImpl() {
					_Socket->close();
				}

	

				std::function<void()> _OnDisconnect;
				std::shared_ptr<Network::Socket> _Socket;
				std::shared_ptr<wxBitmap> _Image;



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



				void OnReceive(const SL::Remote_Access_Library::Network::Socket * s, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& p)
				{
					if (p->header()->Packet_Type == SL::Remote_Access_Library::Network::Commands::PACKET_TYPES::IMAGECHANGE) {
						auto imginfo = (SL::Remote_Access_Library::Network::Commands::ImageChange*)p->data();
						auto img = Remote_Access_Library::Utilities::Image::CreateImage(imginfo->height, imginfo->width, p->data() + sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange), p->header()->PayloadLen - sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange));

						//	Update(imginfo, img);
					}
					else if (p->header()->Packet_Type == SL::Remote_Access_Library::Network::Commands::PACKET_TYPES::RESOLUTIONCHANGE) {
						auto imginfo = (SL::Remote_Access_Library::Network::Commands::ImageChange*)p->data();
						auto img = Remote_Access_Library::Utilities::Image::CreateImage(imginfo->height, imginfo->width, p->data() + sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange), p->header()->PayloadLen - sizeof(SL::Remote_Access_Library::Network::Commands::ImageChange));

						NewImage(imginfo, img);
					}
				}

			};


		}
	}
}




SL::Remote_Access_Library::UI::MainWindow::MainWindow(wxFrame* frame, const std::string title, std::string dst_host, std::string dst_port, std::function<void()> ondisconnect)
{
	_MainWindowImpl = std::make_unique<MainWindowImpl>(frame, title, dst_host, dst_port, ondisconnect);
}

wxScrolledWindow * SL::Remote_Access_Library::UI::MainWindow::get_Frame()
{
	return _MainWindowImpl.get();
}

