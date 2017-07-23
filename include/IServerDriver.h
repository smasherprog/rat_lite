#pragma once
#include "INetworkHandlers.h"

namespace SL {
	namespace RAT {
   

		struct MouseEvent;
		struct KeyEvent;
		class IServerDriver : public INetworkHandlers{
		public:
			virtual ~IServerDriver() {}

			virtual void onReceive_Mouse(const MouseEvent* m) = 0;
			virtual void onReceive_Key(const KeyEvent* m) = 0;
			virtual void onReceive_ClipboardText(const unsigned char* data, unsigned int len) = 0;


		};

	}
}
