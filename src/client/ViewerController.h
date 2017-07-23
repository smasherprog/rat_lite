#pragma once
#include <memory>

namespace SL {
	namespace RAT {

		struct Client_Config;
		class ViewerControllerImpl;
		class ViewerController {
			ViewerControllerImpl* ViewerControllerImpl_ = nullptr;
		public:
			ViewerController(std::shared_ptr<Client_Config> config, const char*  dst_host);
			~ViewerController();
		};
	}
}

