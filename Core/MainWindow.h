#pragma once
#include <memory>

class wxFrame;
class wxScrolledWindow;

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			wxScrolledWindow* CreateMainWindow(wxFrame* parent, const char* title, const char*  dst_host, const char*  dst_port);
		}
	}
}
