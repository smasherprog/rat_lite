#pragma once
#include "IBaseNetworkDriver.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image;
		}
		namespace Capturing {
			struct MouseInfo;
		}
		namespace Network {
			class Rect;
		

			class IClientDriver : public IBaseNetworkDriver {
			public:
				IClientDriver() {}
				virtual ~IClientDriver() {}
				virtual void OnReceive_ImageDif(const std::shared_ptr<ISocket>& socket, Utilities::Rect* rect, std::shared_ptr<Utilities::Image>& img) = 0;
				virtual void OnReceive_Image(const std::shared_ptr<ISocket>& socket,std::shared_ptr<Utilities::Image>& img) = 0;
				virtual void OnReceive_MouseInfo(const std::shared_ptr<ISocket>& socket, Capturing::MouseInfo* mouseinfo) = 0;
			};
		}
	}
}
