#pragma once
#include <memory>
#include <functional>
#include "../Core/Mouse.h"
#include "../Core/Keyboard.h"

class Fl_Scroll;

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Point;
			class Image;
		}
		namespace UI {
			struct ImageControlInfo {
				Fl_Scroll* _Scroller = nullptr;
				std::function<void(int, int, Input::Mouse::Press, int, int)> _MouseCallback;
				std::function<void(int, Input::Keyboard::Press)> _KeyCallback;
			};
			class ImageControlImpl;
			class ImageControl {
				ImageControlImpl* _ImageControlImpl;
			public:
				ImageControl(int X, int Y, int W, int H, const char * title, ImageControlInfo&& info);
				~ImageControl();

				void ScaleImage(bool b);
				void SetMousePosition(const Utilities::Point& m);
				void SetMouseImage(const std::shared_ptr<Utilities::Image>& m);
				float GetScaleFactor() const;
				void SetImage(const std::shared_ptr<Utilities::Image>& m);
				void SetImageDif(Utilities::Point pos, const std::shared_ptr<Utilities::Image>& img);

			};
		}
	}
}
