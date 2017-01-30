#pragma once

namespace SL {
	namespace Remote_Access_Library {
		class ConnectWindowImpl;
		class ConnectWindow {
			ConnectWindowImpl* _ConnectWindowImpl = nullptr;
		public:
			ConnectWindow();
			~ConnectWindow();
		};
	}
}
