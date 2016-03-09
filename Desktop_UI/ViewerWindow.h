#pragma once

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class ViewerWindowImpl;
			class ViewerWindow {
				ViewerWindowImpl* _ViewerWindowImpl;
			public:
				ViewerWindow(const char*  dst_host, const char*  dst_port);
				~ViewerWindow();
			};
		}
	}
}
