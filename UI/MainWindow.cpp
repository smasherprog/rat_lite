#include "stdafx.h"
#include "MainWindow.h"
#include "Image.h"
#include "ClientNetworkDriver.h"
#include "Commands.h"
#include "Socket.h"
#include <mutex>
#include <wx/rawbmp.h>

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class MainWindowImpl : public wxScrolledWindow
			{
				std::shared_ptr<wxBitmap> _Image;
				Network::ClientNetworkDriver<MainWindowImpl> _ReceiverNetworkDriver;
				std::mutex _CloseLock;
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
					//need to make a tmp of the image because it might be replaced by another caller
					auto imgcopy = _Image;
					if (imgcopy) dc.DrawBitmap(*imgcopy, 0, 0, false);
				}

				void ImageDif(Utilities::Rect* rect, std::shared_ptr<Utilities::Image>& img)
				{
					if (!_Image) {//first image, just copy all the data
						_Image = std::make_shared<wxBitmap>(img->data(), img->Width(), img->Height(), 32);
					}
					else if (_Image->GetHeight() < img->Height() || _Image->GetWidth() <= img->Width()) {//resolution change.. make image bigger
						_Image = std::make_shared<wxBitmap>(img->data(), img->Width(), img->Height(), 32);
					}
					else {//update part of the image
						wxNativePixelData data(*_Image);
						if (!data) wxMessageBox("Could not access image data!");
						else if (data.GetWidth() < rect->Width || data.GetHeight() < rect->Height) wxMessageBox("Bitmap too small!");
						else {
							auto stride = 32;
							auto rawdata = (char*)_Image->GetRawData(data, stride);
#if _WIN32
							auto bytestrde = data.GetRowStride()*(data.GetHeight() - 1);
							rawdata += bytestrde;//the wxwidgets library advances the pointer in their GetRawData function 
#endif
							auto srcimgrowstride = img->Stride()*img->Width();
							
							for (unsigned int dstrow = rect->Origin.X, srcrow=0; dstrow < rect->Origin.X + rect->Width; dstrow++, srcrow++) {
								auto dst = rawdata + (rect->Origin.Y*data.GetRowStride()) + (dstrow + stride);//move pointer
								memcpy(dst, img->data() + (srcrow*srcimgrowstride), srcimgrowstride);

							}
							_Image->UngetRawData(data);
						}
					}

					SetScrollbars(1, 1, img->Width(), img->Height(), 0, 0);
					Refresh(false);
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

