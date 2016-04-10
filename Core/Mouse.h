#pragma once
#include "Shapes.h"
#include <functional>
#include <memory>

namespace SL {
	namespace Remote_Access_Library {

		namespace Utilities {
			class Image;
		}
		namespace INTERNAL {
			struct MouseImpl;
		}
		namespace Capturing {
			class Mouse {
				INTERNAL::MouseImpl* _MouseImpl;
				void _run();
			public:
				Mouse(std::function<void(std::shared_ptr<Utilities::Image>)> img_func, std::function<void(Utilities::Point)> pos_func, int img_dely = 1000, int pos_dely = 20);
				~Mouse();
			};
			bool SetCursorPosition(Utilities::Point p);
		}
	}
}