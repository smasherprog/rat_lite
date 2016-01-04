#include "stdafx.h"
#include "Screen.h"
#include "Image.h"
#include "wx/display.h"
#include "wx/rawbmp.h"
#include "wx/bitmap.h"
#include "ThreadPool.h"


#define DESKTOPCAPTURERATE 100
#define DESKTOPWAITCAPTURERATE 20ms

using namespace std::literals;

std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> GetOne2() {

	auto start = std::chrono::steady_clock::now();

	//its faster to make a copy of the entire screen area rather than a call per display
	int h = 0, w = 0;
	for (auto i = 0; i < wxDisplay::GetCount(); i++) {
		wxDisplay d(i);
		h = std::max(h, d.GetGeometry().height);
		w += d.GetGeometry().width;
	}


	wxScreenDC dcScreen;
	wxBitmap bitmap;
	//32 bit depth is best for performance
	bitmap.Create(w, h, 32);

	// Create a memory DC that will be used for actually taking the screenshot
	wxMemoryDC memDC;
	memDC.SelectObject(bitmap);
	memDC.Clear();

	// Blit (in this case copy) the actual screen on the memory DC
	// and thus the Bitmap
	memDC.Blit(0, // Copy to this X coordinate
		0, // Copy to this Y coordinate
		w, // Copy this width
		h, // Copy this height
		&dcScreen, // From where do we copy?
		0, // What's the X offset in the original DC?
		0  // What's the Y offset in the original DC?
		);

	// Select the Bitmap out of the memory DC by selecting a new
	// uninitialized Bitmap
	memDC.SelectObject(wxNullBitmap);
	//bitmap.SaveFile("c:\\users\\scott\\desktop\\somefile.png", wxBitmapType::wxBITMAP_TYPE_PNG);
	wxAlphaPixelData data(bitmap);

	auto rawdata = (char*)bitmap.GetRawData(data, 32);

#if _WIN32
	auto bytestrde = data.GetRowStride()*(data.GetHeight() - 1);
	rawdata += bytestrde;//the wxwidgets library advances the pointer in their GetRawData function 
#endif
	auto retimg = SL::Remote_Access_Library::Utilities::Image::CreateImage(h, w, (const char*)rawdata, h*w * 4);
	bitmap.UngetRawData(data);
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
	std::cout << "It took " << elapsed.count() << " milliseconds to complete a screen grab" << std::endl;
	return retimg;
}




class DesktopCache {
	SL::Remote_Access_Library::Utilities::ThreadPool _ThreadPool;
	std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> _Image;
	std::chrono::time_point<std::chrono::steady_clock> _Timer;
	std::mutex _TimerLock;
public:
	DesktopCache() : _ThreadPool(1), _Timer(std::chrono::steady_clock::now()) {
		//get an image on startup to seed
		_ThreadPool.Enqueue([this]() {
			_Image = GetOne2();
		});
	}

	std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> GetImage() {
		{
			std::lock_guard<std::mutex> lock(_TimerLock);
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _Timer).count() > DESKTOPCAPTURERATE && _ThreadPool.TaskCount()==0) {
				_Timer = std::chrono::steady_clock::now();
				_ThreadPool.Enqueue([this]() {
					_Image = GetOne2();
				});
			}
		}
		while (true) {
			auto tmpimage = _Image;
			if (tmpimage) return tmpimage;
			else { //wait for an image to become available. This should only happen on the first call to this function
				std::this_thread::sleep_for(DESKTOPWAITCAPTURERATE);
			}
		}
	}
};
const std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> SL::Remote_Access_Library::Capturing::CaptureDesktop() {
	static DesktopCache _DesktopCache;
	return _DesktopCache.GetImage();
}