#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {

		struct Client_Config;
		class ViewerControllerImpl;
		class ViewerController {
			ViewerControllerImpl* _ViewerControllerImpl = nullptr;
		public:
			ViewerController(std::shared_ptr<Client_Config> config, const char*  dst_host);
			~ViewerController();
		};
	}
}

