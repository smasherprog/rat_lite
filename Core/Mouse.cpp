#include "stdafx.h"
#include "Mouse.h"
#include <FL/Fl.H>

namespace SL {
	namespace Remote_Access_Library {
		namespace Capturing {

#if _WIN32

#define RAIIHBITMAP(handle) std::unique_ptr<std::remove_pointer<HBITMAP>::type, decltype(&::DeleteObject)>(handle, &::DeleteObject)

			MouseInfo GetCursorInfo()
			{
				MouseInfo info;
				CURSORINFO cursorInfo;
				cursorInfo.cbSize = sizeof(cursorInfo);
				GetCursorInfo(&cursorInfo);

				ICONINFOEX ii = { 0 };
				ii.cbSize = sizeof(ii);
				GetIconInfoEx(cursorInfo.hCursor, &ii);
				auto colorbmp = RAIIHBITMAP(ii.hbmColor);//make sure this is cleaned up properly
				auto maskbmp = RAIIHBITMAP(ii.hbmMask);//make sure this is cleaned up properly

				info.Pos.X = cursorInfo.ptScreenPos.x - ii.xHotspot;
				info.Pos.Y = cursorInfo.ptScreenPos.y - ii.yHotspot;
				//icons listed at https://msdn.microsoft.com/en-us/library/windows/desktop/ms648071(v=vs.85).aspx
				switch (ii.wResID) {
				case 32650:
				case 32514:
					info.MouseType = Fl_Cursor::FL_CURSOR_WAIT;
					break;
				case 32512:
					info.MouseType = Fl_Cursor::FL_CURSOR_ARROW;
					break;
				case 32515:
				case 32646:
					info.MouseType = Fl_Cursor::FL_CURSOR_CROSS;
					break;
				case 32649:
					info.MouseType = Fl_Cursor::FL_CURSOR_HAND;
					break;
				case 32651:
					info.MouseType = Fl_Cursor::FL_CURSOR_HELP;
					break;
				case 32513:
					info.MouseType = Fl_Cursor::FL_CURSOR_INSERT;
					break;
				case 32643:
					info.MouseType = Fl_Cursor::FL_CURSOR_NESW;
					break;
				case 32645:
					info.MouseType = Fl_Cursor::FL_CURSOR_NS;
					break;
				case 32642:
					info.MouseType = Fl_Cursor::FL_CURSOR_NWSE;
					break;
				case 32644:
					info.MouseType = Fl_Cursor::FL_CURSOR_WE;
					break;
				case 32516:
					info.MouseType = Fl_Cursor::FL_CURSOR_N;
					break;
				default:
					info.MouseType = Fl_Cursor::FL_CURSOR_ARROW;
					break;
				}

				return info;
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
#include <gtk/gtk.h>
			MouseInfo GetCursorInfo()
			{

				info.MouseType = Fl_Cursor::FL_CURSOR_ARROW;
				info.Pos = Utilities::Point(0, 0);
				auto rootwnd = gdk_get_default_root_window();
				if (rootwnd == NULL) return info;
				auto m_cursor = gdk_window_get_cursor(rootwnd);
				if (m_cursor == NULL) return info;
				switch (m_cursor->cursor - type) {
				case GDK_CIRCLE:
					info.MouseType = Fl_Cursor::FL_CURSOR_WAIT;
					break;
				case GDK_ARROW:
					info.MouseType = Fl_Cursor::FL_CURSOR_ARROW;
					break;
				case GDK_CROSS:
					info.MouseType = Fl_Cursor::FL_CURSOR_CROSS;
					break;
				case GDK_HAND1:
				case GDK_HAND2:
					info.MouseType = Fl_Cursor::FL_CURSOR_HAND;
					break;
				case 32651:
					info.MouseType = Fl_Cursor::FL_CURSOR_HELP;
					break;
				case 32513:
					info.MouseType = Fl_Cursor::FL_CURSOR_INSERT;
					break;
				case 32643:
					info.MouseType = Fl_Cursor::FL_CURSOR_NESW;
					break;
				case 32645:
					info.MouseType = Fl_Cursor::FL_CURSOR_NS;
					break;
				case 32642:
					info.MouseType = Fl_Cursor::FL_CURSOR_NWSE;
					break;
				case 32644:
					info.MouseType = Fl_Cursor::FL_CURSOR_WE;
					break;
				case 32516:
					info.MouseType = Fl_Cursor::FL_CURSOR_N;
					break;
				default:
					info.MouseType = Fl_Cursor::FL_CURSOR_ARROW;
					break;
				}



				return info;
			}
			}
#elif __ANDROID__
#error Andriod specific implementation  of CaptureMouse has not been written yet. You can help out by writing it!
#endif
		}
	}
}


