#pragma once

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class ConnectionInfoWindowImpl;
			class ConnectionInfoWindow {
				ConnectionInfoWindowImpl* _ConnectWindowImpl = nullptr;
			public:
				ConnectionInfoWindow();
				~ConnectionInfoWindow();
				void Init();
			};
		}
	}
}
