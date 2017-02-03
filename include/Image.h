#pragma once
#include "Shapes.h"

namespace SL {
	namespace RAT {
		const int PixelStride = 4;
		struct Image {
			Rect Rect;
			std::shared_ptr<char> Data;
		};
	}
}