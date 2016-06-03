#pragma once
#include "IBaseNetworkDriver.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Input {
			struct MouseEvent;
			struct KeyEvent;
		}
		namespace Network {
			class ISocket;
			class Packet;
			class IServerDriver : public IBaseNetworkDriver{
			public:
				virtual ~IServerDriver() {}

				virtual void OnMouse(Input::MouseEvent* m) = 0;
				virtual void OnKey(Input::KeyEvent* m) = 0;

			};
		}
	}
}
