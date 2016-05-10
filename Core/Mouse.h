#pragma once
#include "Shapes.h"
#include <functional>
#include <memory>
#include <future>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image;
		}
		namespace Input {
			namespace Mouse {
				enum Events : unsigned int {
					LEFT,
					RIGHT,
					MIDDLE,
					SCROLL,
					NO_EVENTDATA
				};
				enum Press : unsigned int {
					UP,
					DOWN,
					DBLCLICK,
					NO_PRESS_DATA
				};

			}

			struct MouseEvent {
				Mouse::Events EventData;
				Utilities::Point Pos;
				int ScrollDelta;
				Mouse::Press PressData;
			};
			void SimulateMouseEvent(const Input::MouseEvent& m);
			std::future<std::shared_ptr<Utilities::Image>> get_MouseImage();
			std::future<Utilities::Point> get_MousePosition();
		}
	
	}
}