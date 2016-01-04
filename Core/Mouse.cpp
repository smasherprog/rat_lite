#include "stdafx.h"
#include "Image.h"
#include "Mouse.h"
#include "wx/rawbmp.h"
#include "wx/bitmap.h"
#include "ThreadPool.h"

using namespace std::literals;
#define MOUSECAPTURERATE 30
#define MOUSEWAITCAPTURERATE 10ms

SL::Remote_Access_Library::Capturing::MousePoint SL::Remote_Access_Library::Capturing::GetMousePosition()
{
	auto pt = wxGetMousePosition();
	return MousePoint{ pt.x, pt.y };
}
std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> GetOne()
{
	auto cur1 = wxCursor(wxStockCursor::wxCURSOR_ARROW);

	auto w = cur1.GetWidth();
	wxBitmap sdsd;
	sdsd.CopyFromCursor(cur1);
	wxAlphaPixelData data(sdsd);
	auto rawdata = (char*)sdsd.GetRawData(data, 32);
#if _WIN32
	auto bytestrde = data.GetRowStride()*(data.GetHeight() - 1);
	rawdata += bytestrde;//the wxwidgets advances the pointer in their GetRawData function
#endif
	auto retimg = SL::Remote_Access_Library::Utilities::Image::CreateImage(sdsd.GetHeight(), sdsd.GetWidth(), (const char*)rawdata, sdsd.GetHeight()* sdsd.GetWidth() * 4);
	sdsd.UngetRawData(data);
	return retimg;

}






class MouseCache {
	SL::Remote_Access_Library::Utilities::ThreadPool _ThreadPool;
	std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> _Image;
	std::chrono::time_point<std::chrono::steady_clock> _Timer;
	std::mutex _TimerLock;
public:
	MouseCache() : _ThreadPool(1), _Timer(std::chrono::steady_clock::now()) {
		//get an image on startup to seed
		_ThreadPool.Enqueue([this]() {
			_Image = GetOne();
		});
	}

	std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> GetImage() {
		{
			std::lock_guard<std::mutex> lock(_TimerLock);
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _Timer).count() > MOUSECAPTURERATE && _ThreadPool.TaskCount() == 0) {
				_Timer = std::chrono::steady_clock::now();
				_ThreadPool.Enqueue([this]() {
					_Image = GetOne();
				});
			}
		}
		while (true) {
			auto tmpimage = _Image;
			if (tmpimage) return tmpimage;
			else { //wait for an image to become available. This should only happen on the first call to this function
				std::this_thread::sleep_for(MOUSEWAITCAPTURERATE);
			}
		}
	}
};

const std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> SL::Remote_Access_Library::Capturing::CaptureMouse()
{
	static MouseCache _MouseCache;
	return _MouseCache.GetImage();
}

