#pragma once
#include <functional>
#include <memory>
#include "Configs.h"
#include "IServerDriver.h"

namespace SL {
	namespace RAT {
		class ServerHubImpl;
		class ServerHub  {
			std::unique_ptr<ServerHubImpl> ServerHubImpl_;

		public:
			ServerHub(IServerDriver * r, std::shared_ptr<Server_Config> config);
			~ServerHub();

			void Run();
			void Broadcast(char * data, size_t len);
			int get_ClientCount() const;

		};
	}
}
