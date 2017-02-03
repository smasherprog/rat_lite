#pragma once
#include "INetworkHandlers.h"

namespace SL {
	namespace RAT {
		struct Client_Config;
		void Connect(Client_Config* config, INetworkHandlers* driver, const char* host);
	}
}
