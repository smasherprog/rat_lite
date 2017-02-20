#pragma once
#include "INetworkHandlers.h"
#include "Image.h"

namespace SL {
	namespace RAT {

		class Point;
	
		class IClientDriver : public INetworkHandlers {
		public:

			virtual ~IClientDriver() {}
			virtual void onReceive_ImageDif(const Image& img) = 0;
			virtual void onReceive_Image(const Image& img) = 0;
			virtual void onReceive_MouseImage(const Image& img) = 0;
			virtual void onReceive_MousePos(const Point* pos) = 0;
			virtual void onReceive_ClipboardText(const char* data, size_t length) = 0;

		};

	}
}
