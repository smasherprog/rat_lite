#pragma once
#include "INetworkHandlers.h"

namespace SL {
	namespace Screen_Capture{
		struct Image;
	}
	namespace Remote_Access_Library {

		class Point;
		class ISocket;
		class IClientDriver : public INetworkHandlers {
		public:

			virtual ~IClientDriver() {}
			virtual void OnReceive_ImageDif(Point pos, std::shared_ptr<Screen_Capture::Image>& img) = 0;
			virtual void OnReceive_Image(std::shared_ptr<Screen_Capture::Image>& img) = 0;
			virtual void OnReceive_MouseImage(std::shared_ptr<Screen_Capture::Image>& img) = 0;
			virtual void OnReceive_MousePos(Point* pos) = 0;
			virtual void OnReceive_ClipboardText(const char* data, unsigned int len) = 0;

		};

	}
}
