#pragma once
#include "INetworkHandlers.h"

namespace SL {
	namespace RAT {
   

		struct MouseEvent;
		struct KeyEvent;
		class IServerDriver : public INetworkHandlers{
		public:
			virtual ~IServerDriver() {}

            virtual void onKeyUp(char key) = 0;
            virtual void onKeyUp(wchar_t key) = 0;
            virtual void onKeyUp(Input_Lite::SpecialKeyCodes key) = 0;

            virtual void onKeyDown(char key) = 0;
            virtual void onKeyDown(wchar_t key) = 0;
            virtual void onKeyDown(Input_Lite::SpecialKeyCodes key) = 0;

            virtual void onMouseEvent(const MouseEvent& mevent) = 0;
            virtual void onClipboardChanged(const std::string& text) = 0;

		};

	}
}
