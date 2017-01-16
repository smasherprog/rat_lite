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
					NO_PRESS_DATA,
					ALT,
					CTRL,
					SHIFT
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