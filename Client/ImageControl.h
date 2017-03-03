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

		class Point;
		class ImageControlImpl;
		struct Image;
		class ImageControl {
			ImageControlImpl* ImageControlImpl_;
		public:
			ImageControl(int X, int Y, int W, int H, const char * title);
			~ImageControl();

			void OnResize(int W, int H, int SS);
			void Scaled(bool scaled);
			bool Scaled() const;
			void setMonitors(const Screen_Capture::Monitor* monitors, int num_of_monitors);
			void setImageDifference(const Image& img, int monitor_id);
			void setMouseImage(const Image& img);
			void setMousePosition(const Point* pos);

			void onKey(const std::function<void(int, Press)>& func);
			void onMouse(const std::function<void(int, int, Press, int, int)>& func);
			void onDragNDrop(const std::function<void(std::vector<std::string>&)>& func);//drag and drop. vector of strings



		};

	}
}
