#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include "Socket.h"
#include "Listener.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace INTERNAL {
			class ServerImpl {
			public:
				ServerImpl();
				~ServerImpl();

				std::shared_ptr<Network::Listener> _Listener;
				Network::NetworkEvents _NetworkEvents;
				std::vector<std::shared_ptr<Network::Socket>> Clients;
				std::mutex ClientsLock;
				bool Keepgoing;
				std::thread _Runner;
			};
		}

	}
}