#pragma once
#include "INetworkHandlers.h"

namespace SL {
	namespace Remote_Access_Library {
		struct Client_Config;
		void Connect(Client_Config* config, INetworkHandlers* driver, const char* host);
	}
}
