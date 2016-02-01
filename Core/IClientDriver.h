#pragma once
#include "IBaseNetworkDriver.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image;
		}
		namespace Network {
			class Rect;
		

			class IClientDriver : public IBaseNetworkDriver {
			public:
				IClientDriver() {}
				virtual ~IClientDriver() {}
				virtual void OnReceive_Image(const std::shared_ptr<ISocket>& socket, Utilities::Rect* rect, std::shared_ptr<Utilities::Image>& img) = 0;
			};
		}
	}
}
