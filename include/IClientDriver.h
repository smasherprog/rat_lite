#pragma once
#include "INetworkHandlers.h"
#include <string>

namespace SL {
	namespace Screen_Capture {
		struct Monitor;
	}
	namespace RAT {
        struct Image;
		class Point;
        struct MouseEvent;
        struct KeyEvent;

		class IClientDriver : public INetworkHandlers {
		public:

			virtual ~IClientDriver() {}
            //events raised from the server
			virtual void onMonitorsChanged(const Screen_Capture::Monitor monitors[], size_t num_of_monitors) = 0;
            virtual void onFrameChanged(const Image& img, int monitor_id) = 0;
            virtual void onNewFrame(const Image& img, int monitor_id) = 0;
            virtual void onMouseImageChanged(const Image& img) = 0;

			virtual void onKeyEvent(const KeyEvent& kevent) = 0;
			virtual void onMouseEvent(const MouseEvent& mevent) = 0;
			virtual void onClipboardChanged(const std::string& text) = 0;

		};

	}
}
