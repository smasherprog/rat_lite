#pragma once
#include "INetworkHandlers.h"

namespace SL {
	namespace RAT {

		class Point;
		class ISocket;
		class IClientDriver : public INetworkHandlers {
		public:

			virtual ~IClientDriver() {}
			virtual void OnReceive_ImageDif(const Rect* rect, std::shared_ptr<char>& data) = 0;
			virtual void OnReceive_Image(const Rect* rect, std::shared_ptr<char>& data) = 0;
			virtual void OnReceive_MouseImage(const Size* rect, const char* data) = 0;
			virtual void OnReceive_MousePos(const Point* pos) = 0;
			virtual void OnReceive_ClipboardText(const char* data, unsigned int len) = 0;

		};

	}
}
