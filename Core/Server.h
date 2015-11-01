#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct ServerImpl;
			struct NetworkEvents;
			class Server : public std::enable_shared_from_this<Server> {
			public:
				//does not start listening until start is called
				Server(unsigned short port, NetworkEvents& netevents);

				void Start();
				void Stop();
			private:
				std::unique_ptr<ServerImpl> _ServerImpl;
			};
		}
	}
}