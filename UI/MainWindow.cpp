#include "stdafx.h"
#include "MainWindow.h"
#include "Image.h"
#include "ClientNetworkDriver.h"
#include "Commands.h"
#include "ISocket.h"
#include <mutex>
#include <wx/rawbmp.h>

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class MainWindowImpl : public wxScrolledWindow
			{
				std::unique_ptr<wxBitmap> _Image;
				std::unique_ptr<wxAlphaPixelData> ImageData;
				Network::ClientNetworkDriver<MainWindowImpl> _ReceiverNetworkDriver;
				std::mutex _ImageLock;
				wxFrame* Parent = nullptr;
			public:

				MainWindowImpl(wxFrame* frame, const wxString& title, std::string dst_host, std::string dst_port)
					: wxScrolledWindow(frame, wxID_ANY), _ReceiverNetworkDriver(this, dst_host, dst_port)
				{
					Parent = frame;
				}
				virtual ~MainWindowImpl() {
			
				}

				virtual void OnDraw(wxDC & dc) override
				{
					std::cout << "Beg DrawBitmap" << std::endl;
					if (_Image) {
						std::lock_guard<std::mutex> lock(_ImageLock);
						if(_Image) dc.DrawBitmap(*_Image, 0, 0, false);
					}
					std::cout << "End DrawBitmap" << std::endl;
				}

				void ImageDif(Utilities::Rect* rect, std::shared_ptr<Utilities::Image>& img)
				{
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
						_Image = std::make_unique<wxBitmap>();
						_Image->Create(img->Width(), img->Height(), stride);
						ImageData = std::make_unique<wxAlphaPixelData>(*_Image);

						auto dstrowdata = ImageData->GetPixels().m_ptr;
						auto imgrowstride = img->Stride()*img->Width();
						for (decltype(img->Height()) row = 0; row < img->Height(); row++) {
							memcpy(dstrowdata, img->data() + (row*imgrowstride), imgrowstride);
							dstrowdata += ImageData->GetRowStride();
						}
				

					} else {//update part of the image
						std::cout << "Updating Image" << std::endl;
						std::lock_guard<std::mutex> lock(_ImageLock);
						auto dstrowdata = ImageData->GetPixels().m_ptr;
						auto imgrowstride = img->Stride()*img->Width();
						auto dstrowstride = ImageData->GetRowStride();

						for (auto dstrow = rect->Origin.Y, srcrow = 0; dstrow < rect->bottom(); dstrow++, srcrow++) {
							auto dst = dstrowdata + (dstrow*dstrowstride) + (rect->Origin.X * img->Stride());//move pointer
							memcpy(dst, img->data() + (srcrow*imgrowstride), imgrowstride);
						}
					
					}

					SetScrollbars(1, 1, img->Width(), img->Height(), 0, 0);
					Refresh(false);
				}
				void OnClose(const Network::ISocket* socket) {
					wxQueueEvent(Parent, new wxCloseEvent(wxEVT_CLOSE_WINDOW));
				}
				void OnConnect(const std::shared_ptr<Network::ISocket>& socket) {
					int k = 0;
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

