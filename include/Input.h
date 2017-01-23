#pragma once
#include "Shapes.h"

namespace SL {
	namespace Remote_Access_Library {

		enum Specials : unsigned int {
			NO_SPECIAL_DATA,
			ALT,
			CTRL,
			SHIFT
		};

		enum Press : unsigned int {
			NO_PRESS_DATA,
			UP,
			DOWN,
			DBLCLICK
		};

		enum Events : unsigned int {
			NO_EVENTDATA,
			LEFT,
			MIDDLE,
			RIGHT,
			SCROLL

		};

		struct KeyEvent {
			Press PressData;
			unsigned int Key;
			Specials SpecialKey;
		};

		void SimulateKeyboardEvent(KeyEvent ev);


		struct MouseEvent {
			Events EventData;
			Point Pos;
			int ScrollDelta;
			Press PressData;
		};
		void SimulateMouseEvent(const MouseEvent& m);

	}
}