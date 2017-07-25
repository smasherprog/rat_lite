#pragma once
#include "INetworkHandlers.h"
#include <string>
#include <vector>

namespace SL {
	namespace Screen_Capture {
		struct Monitor;
	}
	namespace RAT {
        struct Image;
		class Point;

		class IClientDriver : public INetworkHandlers {
		public:

			virtual ~IClientDriver() {}
            //events raised from the server
			virtual void onMonitorsChanged(const std::vector<Screen_Capture::Monitor>& monitors) = 0;
            virtual void onFrameChanged(const Image& img, const SL::Screen_Capture::Monitor& monitor) = 0;
            virtual void onNewFrame(const Image& img, const SL::Screen_Capture::Monitor& monitor) = 0;
            virtual void onMouseImageChanged(const Image& img) = 0;
			virtual void onMousePositionChanged(const Point& mevent) = 0;
			virtual void onClipboardChanged(const std::string& text) = 0;

		};

	}
}
