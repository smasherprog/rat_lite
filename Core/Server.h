#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		class ServerImpl;
		class Server {
		public:
			Server(unsigned short port);
			~Server();

			int Run();

		private:
			std::shared_ptr<ServerImpl> _ServerImpl;
			
		};
	}
}

