#include "stdafx.h"
#include "Screen.h"
#include "Image.h"
#include <thread>
#include <assert.h>

#define DESKTOPCAPTURERATE 100
#define DESKTOPWAITCAPTURERATE 20

namespace SL {
	namespace Remote_Access_Library {

#if _WIN32
#include <FL/Fl.H>
		//RAII Objects to ensure proper destruction
#define RAIIHDC(handle) std::unique_ptr<std::remove_pointer<HDC>::type, decltype(&::DeleteDC)>(handle, &::DeleteDC)
#define RAIIHBITMAP(handle) std::unique_ptr<std::remove_pointer<HBITMAP>::type, decltype(&::DeleteObject)>(handle, &::DeleteObject)
#define RAIIHANDLE(handle) std::unique_ptr<std::remove_pointer<HANDLE>::type, decltype(&::CloseHandle)>(handle, &::CloseHandle)

		std::shared_ptr<Utilities::Image> CaptureDesktopImage()
		{
			int left(0), top(0), width(0), height(0);
			for (auto i = 0; i < Fl::screen_count(); i++) {
				auto l(0), t(0), w(0), h(0);
				Fl::screen_xywh(l, t, w, h, i);
				width += w;
				height = std::max(height, h);
			}

			static auto desktopdc(RAIIHDC(CreateDCA("DISPLAY", NULL, NULL, NULL)));
			static auto capturedc(RAIIHDC(CreateCompatibleDC(desktopdc.get())));
			static auto capturebmp(RAIIHBITMAP(CreateCompatibleBitmap(desktopdc.get(), width, height)));

			if (!desktopdc || !capturedc || !capturebmp) return Utilities::Image::CreateImage(0, 0);

			// Selecting an object into the specified DC 
			auto originalBmp = SelectObject(capturedc.get(), capturebmp.get());

			auto retimg(Utilities::Image::CreateImage(height, width));

			if (BitBlt(capturedc.get(), 0, 0, width, height, desktopdc.get(), left, top, SRCCOPY | CAPTUREBLT) == FALSE) {
				//if the screen cannot be captured, set everything to 1 and return 
				memset(retimg->data(), 1, retimg->size());
				SelectObject(capturedc.get(), originalBmp);
				return retimg;
			}
			BITMAPINFO bmpInfo;
			memset(&bmpInfo, 0, sizeof(BITMAPINFO));

			bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmpInfo.bmiHeader.biWidth = width;
			bmpInfo.bmiHeader.biHeight = -height;
			bmpInfo.bmiHeader.biPlanes = 1;
			bmpInfo.bmiHeader.biBitCount = 32;//always 32 bits damnit!!!
			bmpInfo.bmiHeader.biCompression = BI_RGB;
			bmpInfo.bmiHeader.biSizeImage = ((width * bmpInfo.bmiHeader.biBitCount + 31) / 32) * 4 * height;

			GetDIBits(desktopdc.get(), capturebmp.get(), 0, (UINT)height, retimg->data(), (BITMAPINFO *)&bmpInfo, DIB_RGB_COLORS);

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

#error Applie specific implementation of CaptureDesktopImage has not been written yet. You can help out by writing it!
#elif __ANDROID__
		std::shared_ptr<Utilities::Image> CaptureDesktopImage()
		{
			return Utilities::Image::CreateImage(0, 0);
		}
#elif __linux__



#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xutil.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

		std::shared_ptr<Utilities::Image> CaptureDesktopImage()
		{
			auto display = XOpenDisplay(NULL);
			auto root = DefaultRootWindow(display);
			auto screen = XDefaultScreen(display);
			auto visual = DefaultVisual(display, screen);
			auto depth = DefaultDepth(display, screen);

			XWindowAttributes gwa;
			XGetWindowAttributes(display, root, &gwa);
			auto width = gwa.width;
			auto height = gwa.height;

			XShmSegmentInfo shminfo;
			auto image = XShmCreateImage(display, visual, depth, ZPixmap, NULL, &shminfo, width, height);
			shminfo.shmid = shmget(IPC_PRIVATE, image->bytes_per_line * image->height, IPC_CREAT | 0777);

			shminfo.readOnly = False;
			shminfo.shmaddr = image->data = (char*)shmat(shminfo.shmid, 0, 0);

			XShmAttach(display, &shminfo);

			XShmGetImage(display, root, image, 0, 0, AllPlanes);

			XShmDetach(display, &shminfo);

			auto px = Utilities::Image::CreateImage(height, width, (char*)shminfo.shmaddr, image->bits_per_pixel / 8);
			assert(image->bits_per_pixel == 32);//this should always be true... Ill write a case where it isnt, but for now it should be

			XDestroyImage(image);
			shmdt(shminfo.shmaddr);
			shmctl(shminfo.shmid, IPC_RMID, 0);
			XCloseDisplay(display);

			return px;

		}
#endif
		namespace INTERNAL {
			struct ScreenImpl {
				std::thread _thread;
				std::function<void(std::shared_ptr<Utilities::Image>)> _CallBack;
				int _ms_Delay;
				bool _Running;
			};
		}
	}
}

void SL::Remote_Access_Library::Capturing::Screen::_run()
{
	while (_ScreenImpl->_Running) {

		auto start = std::chrono::steady_clock::now();
		auto i = CaptureDesktopImage();
		_ScreenImpl->_CallBack(i);
		auto mstaken = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
		if (mstaken <= _ScreenImpl->_ms_Delay) {
			std::this_thread::sleep_for(std::chrono::milliseconds(_ScreenImpl->_ms_Delay - mstaken));
		}
	}
}

SL::Remote_Access_Library::Capturing::Screen::Screen(std::function<void(std::shared_ptr<Utilities::Image>)> func, int ms_dely)
{
#if !__ANDROID__
	_ScreenImpl = std::make_unique<INTERNAL::ScreenImpl>();
	_ScreenImpl->_ms_Delay = ms_dely;
	_ScreenImpl->_CallBack = func;
	_ScreenImpl->_Running = true;
	_ScreenImpl->_thread = std::thread(&SL::Remote_Access_Library::Capturing::Screen::_run, this);
#endif
}

SL::Remote_Access_Library::Capturing::Screen::~Screen()
{
	if (_ScreenImpl) {
		_ScreenImpl->_Running = false;
		_ScreenImpl->_thread.join();
	}
}
