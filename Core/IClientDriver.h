#pragma once
#include "IBaseNetworkDriver.h"

namespace SL {
	namespace Remote_Access_Library {

		namespace Utilities {
			class Image;
			class Point;
		}
		namespace Network {
			class IClientDriver : public IBaseNetworkDriver {
			public:
				IClientDriver() {}
				virtual ~IClientDriver() {}
				virtual void OnReceive_ImageDif(const std::shared_ptr<ISocket>& socket, Utilities::Point pos, std::shared_ptr<Utilities::Image>& img) = 0;
				virtual void OnReceive_Image(const std::shared_ptr<ISocket>& socket,std::shared_ptr<Utilities::Image>& img) = 0;
				virtual void OnReceive_MouseImage(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Utilities::Image>& img) = 0;
				virtual void OnReceive_MousePos(const std::shared_ptr<ISocket>& socket, Utilities::Point* pos) = 0;
	
			};
		}
	}
}
