#pragma once
#include <memory>
#include <functional>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image;
		}

		namespace Capturing {
			std::future<std::shared_ptr<Utilities::Image>> get_ScreenImage();

		}
	}
};
