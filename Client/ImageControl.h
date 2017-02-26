#pragma once
#include "Input.h"

#include <vector>
#include <string>
#include <functional>

namespace SL {
	namespace Screen_Capture {
		struct Monitor;
	}
	namespace RAT {

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
		struct Image;
		class ImageControl {
			ImageControlImpl* ImageControlImpl_;
		public:
			ImageControl(int X, int Y, int W, int H, const char * title, ScreenImageInfo&& info);
			~ImageControl();

			void OnResize(int W, int H, int SS);
			bool is_ImageScaled() const;
			void set_Monitors(const Screen_Capture::Monitor* monitors, int num_of_monitors);
			void set_ScreenImage(const Image& img, int monitor_id);
			void set_ImageDifference(const Image& img, int monitor_id);
			void set_MouseImage(const Image& img);
			void set_MousePosition(const Point* pos);

		};

	}
}
