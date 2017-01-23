#pragma once
#include "INetworkHandlers.h"

namespace SL {
	namespace Remote_Access_Library {

		struct MouseEvent;
		struct KeyEvent;

		class ISocket;
		class Packet;
		class IServerDriver : public INetworkHandlers<uWS::CLIENT> {
		public:
			virtual ~IServerDriver() {}

			virtual void OnReceive_Mouse(MouseEvent* m) = 0;
			virtual void OnReceive_Key(KeyEvent* m) = 0;
			virtual void OnReceive_ClipboardText(const char* data, unsigned int len) = 0;
		};

	}
}
