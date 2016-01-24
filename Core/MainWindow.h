#pragma once

#include <string>
class wxFrame;
class wxScrolledWindow;

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class MainWindowImpl;
			class MainWindow
			{
			public:

				MainWindow(wxFrame* frame, const std::string title, std::string dst_host, std::string dst_port);
				~MainWindow();
				wxScrolledWindow* get_Frame();
			private:
				MainWindowImpl* _MainWindowImpl;

			};


		}
	}
}
