#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image;
		}
		namespace Capturing {
	
			struct MousePoint { int x, y; };
			MousePoint GetMousePosition();
			const std::shared_ptr<Utilities::Image> CaptureMouse();
		}
	}
}