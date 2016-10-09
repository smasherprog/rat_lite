#include "stdafx.h"
#include "Mouse.h"
#include "Image.h"
#include <assert.h>
#include "Logging.h"

#if __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

namespace SL
{
	namespace Remote_Access_Library
	{
		namespace Capturing
		{

#if _WIN32

#define RAIIHDC(handle) std::unique_ptr<std::remove_pointer<HDC>::type, decltype(& ::DeleteDC)>(handle, &::DeleteDC)
#define RAIIHBITMAP(handle) std::unique_ptr<std::remove_pointer<HBITMAP>::type, decltype(& ::DeleteObject)>(handle, &::DeleteObject)
#define RAIIHANDLE(handle) std::unique_ptr<std::remove_pointer<HANDLE>::type, decltype(& ::CloseHandle)>(handle, &::CloseHandle)

			std::shared_ptr<Utilities::Image> CaptureMouseImage()
			{
				auto desktopdc = RAIIHDC(GetDC(NULL));
				auto capturedc = RAIIHDC(CreateCompatibleDC(desktopdc.get()));
				auto capturebmp = RAIIHBITMAP(CreateCompatibleBitmap(desktopdc.get(), 32, 32)); // 32 x 32 is the biggest allowed for windows...
				if (!desktopdc || !capturedc || !capturebmp) {
					SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "Couldnt get Init DC! LastError:" << GetLastError());
					return Utilities::Image::CreateImage(0, 0);
				}
					

				auto originalBmp = SelectObject(capturedc.get(), capturebmp.get());

				CURSORINFO cursorInfo;
				cursorInfo.cbSize = sizeof(cursorInfo);
				if (GetCursorInfo(&cursorInfo) == FALSE) {
					SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "GetCursorInfo == FALSE LastError:" << GetLastError());
					return Utilities::Image::CreateImage(0, 0);
				}
				ICONINFOEXA ii = { 0 };
				ii.cbSize = sizeof(ii);
				if (GetIconInfoExA(cursorInfo.hCursor, &ii) == FALSE) {
					//this tends to fail on hyper-v enviornments generating alot of noise. so lower its level to Info..
					SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "GetIconInfoEx == FALSE LastError:" << GetLastError());
					return Utilities::Image::CreateImage(0, 0);
				}
				auto colorbmp = RAIIHBITMAP(ii.hbmColor); // make sure this is cleaned up properly
				auto maskbmp = RAIIHBITMAP(ii.hbmMask); // make sure this is cleaned up properly
				if (DrawIcon(capturedc.get(), 0, 0, cursorInfo.hCursor) == FALSE) {
					SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "DrawIcon == FALSE LastError:" << GetLastError());
					return Utilities::Image::CreateImage(0, 0);
				}
				BITMAP bm;
				int height = 0;
				int width = 0;
				if (ii.hbmColor != NULL) {
					if (GetObject(colorbmp.get(), sizeof(bm), &bm) != NULL) {
						height = bm.bmHeight;
						width = bm.bmWidth;
					}
				}
				else if (ii.hbmMask != NULL) {
					if (GetObject(maskbmp.get(), sizeof(bm), &bm) != NULL) {
						height = bm.bmHeight / 2;
						width = bm.bmWidth;
					}
				}
				if (height <= 0 || width <= 0 || height > 32 || width > 32) {
					SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "Cursor dimensions are outside normal sizes, could be an error");
					return Utilities::Image::CreateImage(0, 0);
				}


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

				GetDIBits(desktopdc.get(), capturebmp.get(), 0, (UINT)height, retimg->data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

				SelectObject(capturedc.get(), originalBmp);
				if (ii.wResID == 32513) { // when its just the i beam
					unsigned int* ptr = (unsigned int*)retimg->data();
					for (decltype(retimg->size()) i = 0; i < retimg->size() / 4; i++) {
						if (ptr[i] != 0) {
							ptr[i] = 0xff000000;
						}
					}
				}
				else if (ii.hbmMask != nullptr && ii.hbmColor == nullptr) {// just 
					auto ptr = (unsigned int*)retimg->data();
					for (decltype(retimg->size()) i = 0; i < retimg->size() / 4; i++) {
						if (ptr[i] != 0) {
							ptr[i] = ptr[i] | 0xffffffff;
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
				if(GetCursorInfo(&cursorInfo) == FALSE) {
					SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "GetCursorInfo == FALSE LastError:"<< GetLastError());
					return pos;
				}

				ICONINFOEX ii = { 0 };
				ii.cbSize = sizeof(ii);
				auto res = GetIconInfoEx(cursorInfo.hCursor, &ii);
				auto colorbmp = RAIIHBITMAP(ii.hbmColor); // make sure this is cleaned up properly
				auto maskbmp = RAIIHBITMAP(ii.hbmMask); // make sure this is cleaned up properly
				auto lasterror = GetLastError();
				if(res == FALSE) {
					if (lasterror == ERROR_INVALID_CURSOR_HANDLE) return pos;// this happens sometimes... Its not an error, and can create some noise
					SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "GetIconInfoEx == FALSE LastError:" << lasterror);
					return pos;
				}
			
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

            std::shared_ptr<Utilities::Image> CaptureMouseImage()
			{
				return Utilities::Image::CreateImage(0, 0);
			}
           
            Utilities::Point GetCursorPos()
			{
                auto ev = CGEventCreate(NULL);
                auto loc = CGEventGetLocation(ev);
                CFRelease(ev);
                return Utilities::Point(loc.x, loc.y);
            }
#elif __ANDROID__

			std::shared_ptr<Utilities::Image> CaptureMouseImage()
			{
				//this should never be ran
				assert(true);
				return Utilities::Image::CreateImage(0, 0);
			}
			Utilities::Point GetCursorPos()
			{
				//this should never be ran
				assert(true);
				return Utilities::Point(0, 0);
			}
#elif __linux__

			std::shared_ptr<Utilities::Image> CaptureMouseImage()
			{
				auto display = XOpenDisplay(NULL);
				auto img = XFixesGetCursorImage(display);

				if (sizeof(img->pixels[0]) == 8) {
					auto pixels = (int *)img->pixels;
					for (auto i = 0; i < img->width * img->height; ++i) {
						pixels[i] = pixels[i * 2];
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
				x -= img->xhot;
				y -= img->yhot;
				XFree(img);
				XCloseDisplay(display);

				return Utilities::Point(x, y);
			}
#endif

		}
	}
}

std::future<std::shared_ptr<SL::Remote_Access_Library::Utilities::Image>> SL::Remote_Access_Library::Input::get_MouseImage()
{
	return std::async(std::launch::async, [] {return Capturing::CaptureMouseImage();});
}

std::future<SL::Remote_Access_Library::Utilities::Point> SL::Remote_Access_Library::Input::get_MousePosition()
{
	return std::async(std::launch::async, [] {return Capturing::GetCursorPos(); });
}
   
void SL::Remote_Access_Library::Input::SimulateMouseEvent(const Input::MouseEvent & m)
{

	SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level,"SetMouseEvent EventData:"<<m.EventData<<" ScrollDelta: "<<m.ScrollDelta<<" PressData: "<<m.PressData);
	
#if _WIN32

	INPUT input;
	memset(&input, 0, sizeof(input));
	input.type = INPUT_MOUSE;
	input.mi.mouseData = m.ScrollDelta * 120;
	input.mi.dx = static_cast<LONG>(static_cast<float>(m.Pos.X)*(65536.0f / static_cast<float>(GetSystemMetrics(SM_CXSCREEN))));//x being coord in pixels
	input.mi.dy = static_cast<LONG>(static_cast<float>(m.Pos.Y)*(65536.0f / static_cast<float>(GetSystemMetrics(SM_CYSCREEN))));//y being coord in pixels
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

	switch (m.EventData) {
	case Input::Mouse::Events::LEFT:
		if (m.PressData == Input::Mouse::Press::UP) input.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
		else if (m.PressData == Input::Mouse::Press::DOWN) input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
		else input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
		break;
	case Input::Mouse::Events::MIDDLE:
		if (m.PressData == Input::Mouse::Press::UP) input.mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
		else if (m.PressData == Input::Mouse::Press::DOWN) input.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
		else input.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP;
		break;
	case Input::Mouse::Events::RIGHT:
		if (m.PressData == Input::Mouse::Press::UP) input.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
		else if (m.PressData == Input::Mouse::Press::DOWN) input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
		else input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP;
		break;
	case Input::Mouse::Events::SCROLL:
		input.mi.dwFlags |= MOUSEEVENTF_WHEEL;
		break;
	default:
		break;
	}

	SendInput(1, &input, sizeof(input));
	if (m.PressData == Input::Mouse::Press::DBLCLICK) SendInput(1, &input, sizeof(input));
#elif __APPLE__

	CGPoint new_pos;
	new_pos.x = m.Pos.X;
	new_pos.y = m.Pos.Y;
	
    std::vector<CGEventRef> evnts;
    switch (m.EventData) {
	case Input::Mouse::Events::LEFT:
		if (m.PressData == Input::Mouse::Press::UP) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, new_pos, kCGMouseButtonLeft));
		else if (m.PressData == Input::Mouse::Press::DOWN) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, new_pos, kCGMouseButtonLeft));
		else {//double click
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, new_pos, kCGMouseButtonLeft));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, new_pos, kCGMouseButtonLeft));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, new_pos, kCGMouseButtonLeft));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, new_pos, kCGMouseButtonLeft));
        }
		break;
	case Input::Mouse::Events::MIDDLE:
		if (m.PressData == Input::Mouse::Press::UP) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseUp, new_pos, kCGMouseButtonCenter));
		else if (m.PressData == Input::Mouse::Press::DOWN) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseDown, new_pos, kCGMouseButtonCenter));
		else {//double click
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseDown, new_pos, kCGMouseButtonCenter));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseUp, new_pos, kCGMouseButtonCenter));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseDown, new_pos, kCGMouseButtonCenter));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventOtherMouseUp, new_pos, kCGMouseButtonCenter));
        }
		break;
	case Input::Mouse::Events::RIGHT:
		if (m.PressData == Input::Mouse::Press::UP) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, new_pos, kCGMouseButtonRight));
		else if (m.PressData == Input::Mouse::Press::DOWN) evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown, new_pos, kCGMouseButtonRight));
		else {//double click
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown, new_pos, kCGMouseButtonRight));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, new_pos, kCGMouseButtonRight));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseDown, new_pos, kCGMouseButtonRight));
            evnts.push_back(CGEventCreateMouseEvent(NULL, kCGEventRightMouseUp, new_pos, kCGMouseButtonRight));
        }
		break;
	case Input::Mouse::Events::SCROLL:
        evnts.push_back(CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitLine, 1, -m.ScrollDelta));
		break;
	default:
		CGWarpMouseCursorPosition(new_pos);
	}
    for(auto& a: evnts){
        CGEventPost(kCGHIDEventTap, a);
        CFRelease(a);
    }
    
    
    
#elif __ANDROID__

#elif __linux__
    
	auto display = XOpenDisplay(NULL);
    XTestFakeMotionEvent(display, 0, m.Pos.X, m.Pos.Y,0);
    
	
    
    switch (m.EventData) {
	case Input::Mouse::Events::LEFT:
		if (m.PressData == Input::Mouse::Press::UP) XTestFakeButtonEvent(display, Button1,False, CurrentTime);
		else if (m.PressData == Input::Mouse::Press::DOWN) XTestFakeButtonEvent(display, Button1,True,CurrentTime);
		else {//double click
            XTestFakeButtonEvent(display, Button1,True, CurrentTime);
            XTestFakeButtonEvent(display, Button1,False, CurrentTime);
            XTestFakeButtonEvent(display, Button1,True, CurrentTime );
            XTestFakeButtonEvent(display, Button1,False, CurrentTime );
        }
		break;
	case Input::Mouse::Events::MIDDLE:
		if (m.PressData == Input::Mouse::Press::UP) XTestFakeButtonEvent(display, Button2,False, CurrentTime);
		else if (m.PressData == Input::Mouse::Press::DOWN) XTestFakeButtonEvent(display, Button2,True, CurrentTime);
		else {//double click
            XTestFakeButtonEvent(display, Button2,True, CurrentTime);
            XTestFakeButtonEvent(display, Button2,False, CurrentTime);
            XTestFakeButtonEvent(display, Button2,True, CurrentTime);
            XTestFakeButtonEvent(display, Button2,False, CurrentTime);
        }
		break;
	case Input::Mouse::Events::RIGHT:
		if (m.PressData == Input::Mouse::Press::UP) XTestFakeButtonEvent(display, Button3,False, CurrentTime );
		else if (m.PressData == Input::Mouse::Press::DOWN) XTestFakeButtonEvent(display, Button3,True, CurrentTime);
		else {//double click
            XTestFakeButtonEvent(display, Button3,True, CurrentTime);
            XTestFakeButtonEvent(display, Button3,False, CurrentTime);
            XTestFakeButtonEvent(display, Button3,True,CurrentTime );
            XTestFakeButtonEvent(display, Button3,False, CurrentTime);
        }
		break;
	case Input::Mouse::Events::SCROLL:
        if(m.ScrollDelta <0){
            for(auto i=0;i< abs(m.ScrollDelta) && i< 5; i++){///cap at 5
                XTestFakeButtonEvent(display, Button4,True, CurrentTime );  
                XTestFakeButtonEvent(display, Button4,False, CurrentTime); 
            }   
        } else if(m.ScrollDelta>0) {
             for(auto i=0;i< m.ScrollDelta && i< 5; i++){///cap at 5
                XTestFakeButtonEvent(display, Button5,True, CurrentTime );  
                XTestFakeButtonEvent(display, Button5,False, CurrentTime); 
            }     
        }
        
		break;
	default:
		break;
	}
    
	XCloseDisplay(display);

#endif



}

