#pragma once
#include "INetworkHandlers.h"

namespace SL {
	namespace Remote_Access_Library {

		struct MouseEvent;
		struct KeyEvent;
		class IServerDriver : public INetworkHandlers{
		public:
			virtual ~IServerDriver() {}

			virtual void OnReceive_Mouse(const MouseEvent* m) = 0;
			virtual void OnReceive_Key(const KeyEvent* m) = 0;
			virtual void OnReceive_ClipboardText(const char* data, unsigned int len) = 0;
		};

	}
}
