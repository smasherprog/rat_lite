#pragma once
#include "INetworkHandlers.h"
#include "Image.h"

namespace SL {
	namespace Screen_Capture {
		struct Monitor;
	}
	namespace RAT {

		class Point;
	
		class IClientDriver : public INetworkHandlers {
		public:

			virtual ~IClientDriver() {}
			virtual void onReceive_Monitors(const Screen_Capture::Monitor* monitors, int num_of_monitors) = 0;
			virtual void onReceive_ImageDif(const Image& img, int monitor_id) = 0;
			virtual void onReceive_MouseImage(const Image& img) = 0;
			virtual void onReceive_MousePos(const Point* pos) = 0;
			virtual void onReceive_ClipboardText(const char* data, size_t length) = 0;

		};

	}
}
