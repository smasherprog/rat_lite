#pragma once
#include "INetworkHandlers.h"

namespace SL {
	namespace RAT {
   

		struct MouseEvent;
		struct KeyEvent;
		class IServerDriver : public INetworkHandlers{
		public:
			virtual ~IServerDriver() {}

            virtual void onKeyEvent(const KeyEvent& kevent) = 0;
            virtual void onMouseEvent(const MouseEvent& mevent) = 0;
            virtual void onClipboardChanged(const std::string& text) = 0;

		};

	}
}
