#pragma once
#include "IBaseNetworkDriver.h"

namespace SL {
	namespace Remote_Access_Library {

		namespace Utilities {
			class Image;
			class Point;
		}
		namespace Network {
			class ISocket;
			class Packet;
			class IClientDriver : public IBaseNetworkDriver {
			public:

				virtual ~IClientDriver() {}
				virtual void OnReceive_ImageDif(Utilities::Point pos, std::shared_ptr<Utilities::Image>& img) = 0;
				virtual void OnReceive_Image(std::shared_ptr<Utilities::Image>& img) = 0;
				virtual void OnReceive_MouseImage(std::shared_ptr<Utilities::Image>& img) = 0;
				virtual void OnReceive_MousePos(Utilities::Point* pos) = 0;
				virtual void OnReceive_ClipboardText(const char* data, unsigned int len) = 0;
	
			};
		}
	}
}
