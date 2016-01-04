#pragma once
#include <memory>
#include <thread>

namespace SL {
	namespace Remote_Access_Library {
		namespace INTERNAL {
			class ServerImpl;
		}
		class Server {
		public:
			Server(unsigned short port);
			~Server();

		private:
			std::shared_ptr<INTERNAL::ServerImpl> _ServerImpl;
			
		};
	}
}

