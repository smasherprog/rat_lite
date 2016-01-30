#pragma once

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class ConnectWindowImpl;
			class ConnectWindow {
				ConnectWindowImpl* _ConnectWindowImpl = nullptr;
			public:
				ConnectWindow();
				~ConnectWindow();
				void Init();
			};
		}
	}
}
