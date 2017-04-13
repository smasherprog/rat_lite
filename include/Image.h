#pragma once
#include "Shapes.h"

namespace SL {
	namespace RAT {
		const int PixelStride = 4;
		struct Image {
			Image() {}
			Image(const Rect& r, const char* d, const size_t& l) : Rect_(r), Data(d), Length(l) {}
			Rect Rect_;
			const char* Data = nullptr;
			size_t Length = 0;
		};
	}
}