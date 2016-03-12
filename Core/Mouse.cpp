#include "stdafx.h"
#include "Mouse.h"
#include "Image.h"
#include <assert.h>

namespace SL
{
namespace Remote_Access_Library
{
    namespace Capturing
    {

#if _WIN32

#define RAIIHDC(handle) std::unique_ptr<std::remove_pointer<HDC>::type, decltype(& ::DeleteDC)>(handle, &::DeleteDC)
#define RAIIHBITMAP(handle) \
    std::unique_ptr<std::remove_pointer<HBITMAP>::type, decltype(& ::DeleteObject)>(handle, &::DeleteObject)
#define RAIIHANDLE(handle) \
    std::unique_ptr<std::remove_pointer<HANDLE>::type, decltype(& ::CloseHandle)>(handle, &::CloseHandle)

	std::shared_ptr<Utilities::Image> CaptureMouseImage()
	{
	    auto desktopdc = RAIIHDC(GetDC(NULL));
	    auto capturedc = RAIIHDC(CreateCompatibleDC(desktopdc.get()));
	    auto capturebmp = RAIIHBITMAP(
	        CreateCompatibleBitmap(desktopdc.get(), 32, 32)); // 32 x 32 is the biggest allowed for windows...
	    if (!desktopdc || !capturedc || !capturebmp)
		return Utilities::Image::CreateImage(0, 0);

	    auto originalBmp = SelectObject(capturedc.get(), capturebmp.get());

	    CURSORINFO cursorInfo;
	    cursorInfo.cbSize = sizeof(cursorInfo);
	    if (GetCursorInfo(&cursorInfo) == FALSE)
		return Utilities::Image::CreateImage(0, 0);

	    ICONINFO ii = { 0 };
	    if (GetIconInfo(cursorInfo.hCursor, &ii) == FALSE)
		return Utilities::Image::CreateImage(0, 0);
	    auto colorbmp = RAIIHBITMAP(ii.hbmColor); // make sure this is cleaned up properly
	    auto maskbmp = RAIIHBITMAP(ii.hbmMask); // make sure this is cleaned up properly
	    if (DrawIcon(capturedc.get(), 0, 0, cursorInfo.hCursor) == FALSE)
		return Utilities::Image::CreateImage(0, 0);
	    BITMAP bm;
	    int height = 0;

	    if (ii.hbmColor != NULL) {
		GetObject(colorbmp.get(), sizeof(bm), &bm);
		height = bm.bmHeight;

	    } else if (ii.hbmMask != NULL) {
		GetObject(maskbmp.get(), sizeof(bm), &bm);
		height = bm.bmHeight / 2;
	    }
	    int width = bm.bmWidth;

	    BITMAPINFOHEADER bi;
	    memset(&bi, 0, sizeof(bi));

	    bi.biSize = sizeof(BITMAPINFOHEADER);
	    bi.biWidth = width;
	    bi.biHeight = -height;
	    bi.biPlanes = 1;
	    bi.biBitCount = 32; // always 32 bits damnit!!!
	    bi.biCompression = BI_RGB;
	    bi.biSizeImage = 0;
	    bi.biXPelsPerMeter = 0;
	    bi.biYPelsPerMeter = 0;
	    bi.biClrUsed = 0;
	    bi.biClrImportant = 0;
	    bi.biSizeImage = ((width * bi.biBitCount + 31) / 32) * 4 * height;

	    auto retimg(Utilities::Image::CreateImage(width, height));
	    GetDIBits(
	        desktopdc.get(), capturebmp.get(), 0, (UINT)height, retimg->data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	    SelectObject(capturedc.get(), originalBmp);
	    if (ii.hbmMask != NULL && ii.hbmColor == NULL) { // when its just a mask, some bit fiddling needs to happen
		unsigned int* ptr = (unsigned int*)retimg->data();
		for (auto i = 0; i < retimg->size() / 4; i++) {
		    if (ptr[i] != 0) {
			ptr[i] = 0xff000000;
		    }
		}
	    }
	    return retimg;
	}
	Utilities::Point GetCursorPos()
	{
	    Utilities::Point pos;
	    CURSORINFO cursorInfo;
	    cursorInfo.cbSize = sizeof(cursorInfo);
	    GetCursorInfo(&cursorInfo);

	    ICONINFOEX ii = { 0 };
	    ii.cbSize = sizeof(ii);
	    GetIconInfoEx(cursorInfo.hCursor, &ii);
	    auto colorbmp = RAIIHBITMAP(ii.hbmColor); // make sure this is cleaned up properly
	    auto maskbmp = RAIIHBITMAP(ii.hbmMask); // make sure this is cleaned up properly

	    pos.X = cursorInfo.ptScreenPos.x - ii.xHotspot;
	    pos.Y = cursorInfo.ptScreenPos.y - ii.yHotspot;

	    return pos;
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
#error "Unknown Apple platform"
#endif

#error Applie specific implementation of CaptureMouse has not been written yet. You can help out by writing it!
#elif __linux__
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
	std::shared_ptr<Utilities::Image> CaptureMouseImage()
	{ 
		auto display = XOpenDisplay(NULL);
		auto img = XFixesGetCursorImage(display);

		if (sizeof(img->pixels[0]) == 8) {
			auto pixels = (int *) img->pixels;
			for (auto i = 0; i < img->width * img->height; ++i) {
				pixels[i] = pixels[i*2];
			}
		}
	
		auto i = Utilities::Image::CreateImage(img->height, img->width, (char*)img->pixels, 4);	

		XFree(img);
		XCloseDisplay(display);
		return i;
	}

	Utilities::Point GetCursorPos()
	{
	    auto display = XOpenDisplay(NULL);
	    auto root = DefaultRootWindow(display);
		auto img = XFixesGetCursorImage(display);

	    // Get the mouse cursor position
	    int x, y, root_x, root_y = 0;
	    unsigned int mask = 0;
	    Window child_win, root_win;
	    XQueryPointer(display, root, &child_win, &root_win, &root_x, &root_y, &x, &y, &mask);
		x-= img->xhot;
		y-= img->yhot;
		XFree(img);
	    XCloseDisplay(display);

	    return Utilities::Point(x, y);
	}
#elif __ANDROID__
#error Andriod specific implementation  of CaptureMouse has not been written yet. You can help out by writing it!
#endif
	bool SetCursorPosition(Utilities::Point p)
	{

#if defined _WIN32
	    return SetCursorPos(p.X, p.Y) == TRUE;
#elif defined __APPLE__
	    CGPoint new_pos;
	    CGEventErr err;
	    new_pos.x = p.X;
	    new_pos.y = p.Y;
	    return !CGWarpMouseCursorPosition(new_pos);

#elif __linux__
#include <X11/Xlib.h>
	    auto display = XOpenDisplay(NULL);
	    auto root = DefaultRootWindow(display);

	    XWarpPointer(display, None, root, 0, 0, 0, 0, p.X, p.Y);

	    XCloseDisplay(display);
	    return true;
#endif

	    return false; // Fail
	}
    }
    namespace INTERNAL
    {
	struct MouseImpl
	{
	    std::thread _thread;
	    std::function<void(std::shared_ptr<Utilities::Image>)> _ImgCallBack;
	    std::function<void(Utilities::Point)> _PosCallBack;
	    int _img_Delay;
	    int _pos_Delay;
	    bool _Running;
	};
    }
}
}
void SL::Remote_Access_Library::Capturing::Mouse::_run()
{
    int totalwait = 0;
    while (_MouseImpl->_Running) {

	if (totalwait >= _MouseImpl->_img_Delay) {
	    _MouseImpl->_ImgCallBack(CaptureMouseImage());
	    totalwait = 0;
	}
	totalwait += _MouseImpl->_pos_Delay;
	_MouseImpl->_PosCallBack(GetCursorPos());
	std::this_thread::sleep_for(std::chrono::milliseconds(_MouseImpl->_pos_Delay));
    }
}

SL::Remote_Access_Library::Capturing::Mouse::Mouse(std::function<void(std::shared_ptr<Utilities::Image>)> img_func,
                                                   std::function<void(Utilities::Point)> pos_func,
                                                   int img_dely,
                                                   int pos_dely)
{
    assert(img_dely > pos_dely); // img delay must be longer than the pos delay.. mm kay?
    _MouseImpl = new INTERNAL::MouseImpl();
    _MouseImpl->_img_Delay = img_dely;
    _MouseImpl->_pos_Delay = pos_dely;
    _MouseImpl->_ImgCallBack = img_func;
    _MouseImpl->_PosCallBack = pos_func;

    _MouseImpl->_Running = true;
    _MouseImpl->_thread = std::thread(&SL::Remote_Access_Library::Capturing::Mouse::_run, this);
}

SL::Remote_Access_Library::Capturing::Mouse::~Mouse()
{
    _MouseImpl->_Running = false;
    _MouseImpl->_thread.join();
    delete _MouseImpl;
}
