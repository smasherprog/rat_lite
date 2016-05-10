#pragma once

namespace SL {
	namespace Remote_Access_Library {
		namespace Input {
			namespace Keyboard {
				enum Press : unsigned int {
					UP,
					DOWN
				};
				enum Specials : unsigned int {
					ALT,
					CTRL,
					SHIFT,
					NO_PRESS_DATA
				};
			}

			struct KeyEvent {
				Keyboard::Press PressData;
				unsigned int Key;
				Keyboard::Specials SpecialKey;
			};
			void SimulateKeyboardEvent(KeyEvent ev);
		}
	}
}