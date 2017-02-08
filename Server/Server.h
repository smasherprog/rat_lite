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
			std::shared_ptr<ServerImpl> _ServerImpl;
		public:

			Server();
			~Server();

			void Start(std::shared_ptr<Server_Config> config);
			void Stop();
		
			Server_Status get_Status() const;
			static std::string Validate_Settings(std::shared_ptr<Server_Config> config);

#if __ANDROID__
			void OnImage(char* buf, int width, int height);
#endif


		};
	}
}

