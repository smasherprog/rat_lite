#pragma once
#include "Keyboard.h"
#include "Mouse.h"

#include <vector>
#include <string>

namespace SL {
	namespace Remote_Access_Library {
		namespace UI {
			struct ScreenImageInfo {

				std::function<int()> get_Top;
				std::function<int()> get_Left;
				std::function<int()> get_Height;

				std::function<void(int, Input::Keyboard::Press)> OnKey;
				std::function<void(int, int, Input::Mouse::Press, int, int)> OnMouse;
				std::function<void(std::vector<std::string>&)> OnDND;//drag and drop. vector of strings
			};

			class ImageControlImpl;
			class ImageControl {
				ImageControlImpl* _ImageControlImpl;
			public:
				ImageControl(int X, int Y, int W, int H, const char * title, ScreenImageInfo&& info);
				~ImageControl();

				void OnResize(int W, int H, int SS);
				bool is_ImageScaled() const;
				void set_ScreenImage(std::shared_ptr<Utilities::Image>& img);
				void set_ImageDifference(Utilities::Point& pos, const std::shared_ptr<Utilities::Image>& img);
				void set_MouseImage(std::shared_ptr<Utilities::Image>& img);
				void set_MousePosition(Utilities::Point* pos);

			};
		}
	}
}
