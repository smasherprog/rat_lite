#pragma once
#include "Shapes.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Capturing {
			struct MouseInfo {
				unsigned int MouseType;
				Utilities::Point Pos;
			};
			MouseInfo GetCursorInfo();
		}
	}
}