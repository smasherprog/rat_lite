#pragma once
#include <memory>
#include <string>

namespace SL {
	namespace RAT {

		struct Server_Config;
		enum Server_Status {
			SERVER_RUNNING,
			SERVER_STOPPING,
			SERVER_STOPPED
		};


		class ServerImpl;
		class Server {
			std::shared_ptr<ServerImpl> ServerImpl_;
		public:

			Server(std::shared_ptr<Server_Config> config);
			~Server();
			void Run();


#if __ANDROID__
			void OnImage(char* buf, int width, int height);
#endif


		};
	}
}

