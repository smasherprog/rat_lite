#include "stdafx.h"
#include "Mouse.h"
#include <FL/Fl.H>

namespace SL {
	namespace Remote_Access_Library {
		namespace Capturing {

#if _WIN32

#define RAIIHBITMAP(handle) std::unique_ptr<std::remove_pointer<HBITMAP>::type, decltype(&::DeleteObject)>(handle, &::DeleteObject)

			unsigned int GetShownMouseCursor()
			{


				CURSORINFO cursorInfo;
				cursorInfo.cbSize = sizeof(cursorInfo);
				GetCursorInfo(&cursorInfo);

				ICONINFOEX ii = { 0 };
				ii.cbSize = sizeof(ii);
				GetIconInfoEx(cursorInfo.hCursor, &ii);
				auto colorbmp = RAIIHBITMAP(ii.hbmColor);//make sure this is cleaned up properly
				auto maskbmp = RAIIHBITMAP(ii.hbmMask);//make sure this is cleaned up properly
				//icons listed at https://msdn.microsoft.com/en-us/library/windows/desktop/ms648071(v=vs.85).aspx
				switch (ii.wResID) {
				case 32650:
				case 32514:
					return Fl_Cursor::FL_CURSOR_WAIT;
				case 32512:
					return Fl_Cursor::FL_CURSOR_ARROW;
				case 32515:
				case 32646:
					return Fl_Cursor::FL_CURSOR_CROSS;
				case 32649:
					return Fl_Cursor::FL_CURSOR_HAND;
				case 32651:
					return Fl_Cursor::FL_CURSOR_HELP;
				case 32513:
					return Fl_Cursor::FL_CURSOR_INSERT;
				case 32643:
					return Fl_Cursor::FL_CURSOR_NESW;
				case 32645:
					return Fl_Cursor::FL_CURSOR_NS;
				case 32642:
					return Fl_Cursor::FL_CURSOR_NWSE;
				case 32644:
					return Fl_Cursor::FL_CURSOR_WE;
				case 32516:
					return Fl_Cursor::FL_CURSOR_N;
				default:
					return Fl_Cursor::FL_CURSOR_ARROW;
				}
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
				return Fl_Cursor::FL_CURSOR_ARROW;
			}
#elif __ANDROID__
#error Andriod specific implementation  of CaptureMouse has not been written yet. You can help out by writing it!
#endif
		}
	}
}


