#pragma once

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			class ViewerControllerImpl;
			class ViewerController {
				ViewerControllerImpl* _ViewerControllerImpl;
			public:
				ViewerController(const char*  dst_host, const char*  dst_port);
				~ViewerController();

				void ScaleView(bool b);
			};
		}
	}
}
