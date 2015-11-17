#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network{ 
			struct NetworkEvents;
		}
		struct ServerImpl;

		class Server {
		public:
			Server(unsigned short port, Network::NetworkEvents& netevents);
			~Server();

		private:
			std::shared_ptr<ServerImpl> _ServerImpl;
		};
	}
}

