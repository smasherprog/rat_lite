#pragma once
#include "Shapes.h"
#include <functional>
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
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
			void SetMouseEvent(const Input::MouseEvent& m);
		}
		namespace Utilities {
			class Image;
		}
		namespace INTERNAL {
			struct MouseImpl;
		}
		namespace Capturing {
			class Mouse {
				std::unique_ptr<INTERNAL::MouseImpl> _MouseImpl;
				void _run();
			public:
				Mouse(std::function<void(std::shared_ptr<Utilities::Image>)> img_func, std::function<void(Utilities::Point)> pos_func, int img_dely = 1000, int pos_dely = 20);
				~Mouse();
			};


		}

	}
}