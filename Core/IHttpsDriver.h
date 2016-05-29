#pragma once
#include "IBaseNetworkDriver.h"


namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
	
			class ISocket;
			class Packet;
			class IHttpsDriver : public IBaseNetworkDriver<std::shared_ptr<ISocket>, std::shared_ptr<Packet>> {
			public:

				virtual ~IHttpsDriver() {}

	

			};

		}
	}
}