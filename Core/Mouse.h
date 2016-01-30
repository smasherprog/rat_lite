#pragma once
#include <memory>
#include "Shapes.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image;
		}
		namespace Capturing {
	
			const std::shared_ptr<Utilities::Image> CaptureMouse();
		}
	}
}