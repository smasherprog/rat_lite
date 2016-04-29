#pragma once

namespace SL {
	namespace Remote_Access_Library {
		namespace Input {
			enum MouseEvents : unsigned char {
				LEFT = 1,
				RIGHT,
				MIDDLE
			};
			enum MousePress : unsigned char {
				UP=MouseEvents::MIDDLE +1, DOWN
			};
		}
	}
}
