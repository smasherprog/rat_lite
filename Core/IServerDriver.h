#pragma once
#include "IBaseNetworkDriver.h"
#include "MouseInput.h"

namespace SL {
	namespace Remote_Access_Library {
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


				virtual void OnMouse(Utilities::Point& pos) = 0;
				virtual void OnMouse(Input::MouseEvents ev, Input::MousePress press) = 0;

			};
		}
	}
}
