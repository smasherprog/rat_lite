#pragma once

#include <functional>
#include <string>
class wxScrolledWindow;

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class MainWindowImpl;
			class MainWindow 
			{
			public:

				MainWindow(wxFrame* frame, const std::string title, std::string dst_host, std::string dst_port, std::function<void()> ondisconnect);
				wxScrolledWindow* get_Frame();
			private:
				std::unique_ptr<MainWindowImpl> _MainWindowImpl;
			};


		}
	}
}
