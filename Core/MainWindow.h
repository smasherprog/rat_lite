#pragma once

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class MainWindowImpl;
			class MainWindow {
				MainWindowImpl* _MainWindowImpl;
			public:
				MainWindow(const char*  dst_host, const char*  dst_port);
				~MainWindow();
			};
		}
	}
}
