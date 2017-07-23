#pragma once
#include "INetworkHandlers.h"
#include "Image.h"
#include <string>

namespace SL {
	namespace Screen_Capture {
		struct Monitor;
	}
	namespace RAT {

		class Point;
	
		class IClientDriver : public INetworkHandlers {
		public:

			virtual ~IClientDriver() {}
            //server notifying client of changes for below events
			virtual void onMonitorsChanged(const Screen_Capture::Monitor monitors[], int num_of_monitors) = 0;
			virtual void onFrameChanged(const Image& img, int monitor_id) = 0;
            virtual void onNewFrame(const Image& img, int monitor_id) = 0;
            virtual void onMouseChanged(const Image& img) = 0;
            virtual void onMouseChanged(const Point& pos) = 0;
            virtual void onClipboardChanged(const std::string& text) = 0;


		};

	}
}
