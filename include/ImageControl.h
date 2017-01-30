#pragma once
#include "Input.h"

#include <vector>
#include <string>
#include <functional>
#include <memory>

namespace SL {
	namespace Screen_Capture {
		struct Image;
	}
	namespace Remote_Access_Library {

		struct ScreenImageInfo {

			std::function<int()> get_Top;
			std::function<int()> get_Left;
			std::function<int()> get_Height;

			std::function<void(int, Press)> OnKey;
			std::function<void(int, int, Press, int, int)> OnMouse;
			std::function<void(std::vector<std::string>&)> OnDND;//drag and drop. vector of strings
		};
		class Point;
		class ImageControlImpl;
		class ImageControl {
			ImageControlImpl* _ImageControlImpl;
		public:
			ImageControl(int X, int Y, int W, int H, const char * title, ScreenImageInfo&& info);
			~ImageControl();

			void OnResize(int W, int H, int SS);
			bool is_ImageScaled() const;
			void set_ScreenImage(std::shared_ptr<Screen_Capture::Image>& img);
			void set_ImageDifference(Point& pos, const std::shared_ptr<Screen_Capture::Image>& img);
			void set_MouseImage(std::shared_ptr<Screen_Capture::Image>& img);
			void set_MousePosition(Point* pos);

		};

	}
}
