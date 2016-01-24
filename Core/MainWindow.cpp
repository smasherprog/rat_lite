#include "stdafx.h"
#include "MainWindow.h"
#include "Image.h"
#include "ClientNetworkDriver.h"
#include "Commands.h"
#include "ISocket.h"
#include <mutex>
#include <wx/rawbmp.h>
#include "IClientDriver.h"

namespace SL {
	namespace Remote_Access_Library {

		namespace UI {
			class MainWindowImpl : public wxScrolledWindow, public Network::IClientDriver
			{
				std::unique_ptr<wxBitmap> _Image;
				std::unique_ptr<wxAlphaPixelData> ImageData;
				std::mutex _ImageLock;
				Network::ClientNetworkDriver _ClientNetworkDriver;

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
		
					if (_Image) {
						std::lock_guard<std::mutex> lock(_ImageLock);
						if (_Image) dc.DrawBitmap(*_Image, 0, 0, false);
						
					}
		
				}

				virtual void OnReceive_Image(Utilities::Rect * rect, std::shared_ptr<Utilities::Image>& img) override
				{
					/*if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _NetworkStatsTimer).count() > 1000) {
						_NetworkStatsTimer = std::chrono::steady_clock::now();
						auto stats = _ReceiverNetworkDriver.get_SocketStats();
						wxString st = "Client ";

						st += std::to_string((stats.NetworkBytesReceived - LastStats.NetworkBytesReceived) / 1000) + " kbs Received ";
						st += std::to_string((stats.NetworkBytesSent - LastStats.NetworkBytesSent) / 1000) + " kbs Sent";
						Parent->SetTitle(st);
					}*/
					auto stride = 32;
					auto gennewimg = false;
					if (_Image) {
						if (_Image->GetHeight() < static_cast<int>(img->Height()) || _Image->GetWidth() <= static_cast<int>(img->Width())) {
							gennewimg = true;
						}
					}
					else {//first image, just copy all the data
						gennewimg = true;
					}

					if (gennewimg) {
						//lock needed for image updates
						{
							std::lock_guard<std::mutex> lock(_ImageLock);
							_Image = std::make_unique<wxBitmap>();
							_Image->Create(img->Width(), img->Height(), stride);
							ImageData = std::make_unique<wxAlphaPixelData>(*_Image);

							auto dstrowdata = ImageData->GetPixels().m_ptr;
							auto imgrowstride = img->Stride()*img->Width();
							for (decltype(img->Height()) row = 0; row < img->Height(); row++) {
								memcpy(dstrowdata, img->data() + (row*imgrowstride), imgrowstride);
								dstrowdata += ImageData->GetRowStride();
							}
						}
						SetScrollbars(1, 1, img->Width(), img->Height(), 0, 0);
						Refresh(false);
					}
					else {//update part of the image
						 //lock needed for image updates
						{
							std::lock_guard<std::mutex> lock(_ImageLock);
							auto dstrowdata = ImageData->GetPixels().m_ptr;
							auto imgrowstride = img->Stride()*img->Width();
							auto dstrowstride = ImageData->GetRowStride();

							for (auto dstrow = rect->Origin.Y, srcrow = 0; dstrow < rect->bottom(); dstrow++, srcrow++) {
								auto dst = dstrowdata + (dstrow*dstrowstride) + (rect->Origin.X * img->Stride());//move pointer
								memcpy(dst, img->data() + (srcrow*imgrowstride), imgrowstride);
							}
						}
						RefreshRect(wxRect(rect->left(), rect->top(), rect->Width, rect->Height), false);

					}
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
	auto parentframe = new wxFrame(parent, -1, title, wxPoint(50, 50), wxSize(650, 650));

	auto _MainWindowImpl = new MainWindowImpl(parentframe, dst_host, dst_port);
	sizer->Add(_MainWindowImpl, 1, wxALL | wxEXPAND);
	parentframe->SetSizer(sizer);
	parentframe->Show();
	return _MainWindowImpl;
	//THE WXWIDGETS LIBRARY TAKES CARE OF DELETING WINDOWS... THATS WHY THERE IS NO DELETE !!!
}
