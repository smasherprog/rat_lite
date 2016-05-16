#pragma once
#include "IBaseNetworkDriver.h"


namespace SL {
	namespace Remote_Access_Library {
		namespace Input {
			struct MouseEvent;
			struct KeyEvent;
		}
		namespace Utilities {
			class Point;
		}
		namespace Network {
			class Packet;
			class ISocket;

			class IServerDriver : public IBaseNetworkDriver {
			public:
				IServerDriver() {}
				virtual ~IServerDriver() {}


				virtual void OnMouse(Input::MouseEvent* m) = 0;
				virtual void OnKey(Input::KeyEvent* m) = 0;

			};
		}
	}
}
