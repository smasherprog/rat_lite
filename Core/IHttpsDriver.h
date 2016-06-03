#pragma once
#include "IBaseNetworkDriver.h"


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
	
			class ISocket;
			class Packet;
			class IHttpsDriver : public IBaseNetworkDriver {
			public:

				virtual ~IHttpsDriver() {}

	

			};

		}
	}
}