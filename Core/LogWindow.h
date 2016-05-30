#pragma once
#include <string>


namespace SL {
	namespace Remote_Access_Library {
		namespace Server {
			namespace UI {
				class LogWindowImpl;
				class LogWindow {
					LogWindowImpl* _LogWindowImpl = nullptr;
				public:
					LogWindow();
					~LogWindow();
					void Show();
					void Hide();
					void AddMessage(std::string msg);
					void set_MaxLines(int lines);
				};
			}
		}
	}
}
