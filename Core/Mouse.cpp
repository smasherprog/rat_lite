#include "stdafx.h"
#include "Image.h"
#include "Mouse.h"
#include "ThreadPool.h"
#include <FL/Fl.H>

#define MOUSECAPTURERATE 30
#define MOUSEWAITCAPTURERATE 10

namespace SL {
	namespace Remote_Access_Library {

#if _WIN32
#define RAIIHDC(handle) std::unique_ptr<std::remove_pointer<HDC>::type, decltype(&::DeleteDC)>(handle, &::DeleteDC)
#define RAIIHBITMAP(handle) std::unique_ptr<std::remove_pointer<HBITMAP>::type, decltype(&::DeleteObject)>(handle, &::DeleteObject)
#define RAIIHANDLE(handle) std::unique_ptr<std::remove_pointer<HANDLE>::type, decltype(&::CloseHandle)>(handle, &::CloseHandle)

		std::shared_ptr<Utilities::Image_Wrapper> CaptureMouse()
		{

			auto desktopdc = RAIIHDC(GetDC(NULL));
			if (desktopdc.get() == NULL) return Utilities::Image::CreateWrappedImage(0, 0);
			auto capturedc = RAIIHDC(CreateCompatibleDC(desktopdc.get()));
			if (capturedc.get() == NULL) return Utilities::Image::CreateWrappedImage(0, 0);
			auto capturebmp = RAIIHBITMAP(CreateCompatibleBitmap(desktopdc.get(), 32, 32));//32 x 32 is the biggest allowed for windows...
			if (capturebmp.get() == NULL)return Utilities::Image::CreateWrappedImage(0, 0);


			auto originalBmp = SelectObject(capturedc.get(), capturebmp.get());

			CURSORINFO cursorInfo;
			cursorInfo.cbSize = sizeof(cursorInfo);
			GetCursorInfo(&cursorInfo);

			ICONINFO ii = { 0 };
			GetIconInfo(cursorInfo.hCursor, &ii);
			auto colorbmp = RAIIHBITMAP(ii.hbmColor);//make sure this is cleaned up properly
			auto maskbmp = RAIIHBITMAP(ii.hbmMask);//make sure this is cleaned up properly
			DrawIcon(capturedc.get(), 0, 0, cursorInfo.hCursor);
			BITMAP bm;
			int height = 0;

			if (ii.hbmColor != NULL) {
				GetObject(colorbmp.get(), sizeof(bm), &bm);
				height = bm.bmHeight;
			}
			else if (ii.hbmMask != NULL) {
				GetObject(maskbmp.get(), sizeof(bm), &bm);
				height = bm.bmHeight / 2;
			}
			else return Utilities::Image::CreateWrappedImage(0, 0);
			int width = bm.bmWidth;

			BITMAPINFOHEADER   bi;
			memset(&bi, 0, sizeof(bi));

			bi.biSize = sizeof(BITMAPINFOHEADER);
			bi.biWidth = width;
			bi.biHeight = -height;
			bi.biPlanes = 1;
			bi.biBitCount = 32;//always 32 bits damnit!!!
			bi.biCompression = BI_RGB;
			bi.biSizeImage = 0;
			bi.biXPelsPerMeter = 0;
			bi.biYPelsPerMeter = 0;
			bi.biClrUsed = 0;
			bi.biClrImportant = 0;
			bi.biSizeImage = ((width * bi.biBitCount + 31) / 32) * 4 * height;

			auto retimg(Utilities::Image::CreateWrappedImage(width, height));
			GetDIBits(desktopdc.get(), capturebmp.get(), 0, (UINT)height, retimg->WrappedImage.data(), (BITMAPINFO *)&bi, DIB_RGB_COLORS);

			SelectObject(capturedc.get(), originalBmp);

			return retimg;
		}


#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
		// iOS Simulator
#elif TARGET_OS_IPHONE
		// iOS device
#elif TARGET_OS_MAC
		// Other kinds of Mac OS
#else
#   error "Unknown Apple platform"
#endif

#error Applie specific implementation of CaptureMouse has not been written yet. You can help out by writing it!
#elif __linux__
		std::shared_ptr<Utilities::Image_Wrapper> CaptureMouse()
		{
			return Utilities::Image::CreateWrappedImage(0, 0);
	}
#elif __ANDROID__
#error Andriod specific implementation  of CaptureMouse has not been written yet. You can help out by writing it!
#endif


		class MouseCache {
			SL::Remote_Access_Library::Utilities::ThreadPool _ThreadPool;
			std::shared_ptr<Utilities::Image_Wrapper> _Image;
			std::chrono::time_point<std::chrono::steady_clock> _Timer;
			std::mutex _TimerLock;
		public:
			MouseCache() : _ThreadPool(1), _Timer(std::chrono::steady_clock::now()) {
				//get an image on startup to seed
				_ThreadPool.Enqueue([this]() {
					_Image = CaptureMouse();
				});
			}

			std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> GetImage() {
				{
					std::lock_guard<std::mutex> lock(_TimerLock);
					if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _Timer).count() > MOUSECAPTURERATE && _ThreadPool.TaskCount() == 0) {
						_Timer = std::chrono::steady_clock::now();
						_ThreadPool.Enqueue([this]() {
							_Image.reset();
							_Image = CaptureMouse();
						});
					}
				}
				while (true) {
					auto tmpimage = _Image;
					if (tmpimage) {
						return std::shared_ptr<Utilities::Image>(tmpimage, &tmpimage->WrappedImage);
					}
					else { //wait for an image to become available. This should only happen on the first call to this function
						std::this_thread::sleep_for(std::chrono::milliseconds(MOUSEWAITCAPTURERATE));
					}
				}
			}
		};
	}
}




const std::shared_ptr<SL::Remote_Access_Library::Utilities::Image> SL::Remote_Access_Library::Capturing::CaptureMouse()
{
	static MouseCache _MouseCache;
	return _MouseCache.GetImage();
}

