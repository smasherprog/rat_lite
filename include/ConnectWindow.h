#pragma once
#include <string>
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		struct Client_Config;
		class ConnectWindowImpl;
		class ConnectWindow {
			ConnectWindowImpl* _ConnectWindowImpl = nullptr;
		public:
			ConnectWindow(const std::shared_ptr<Client_Config> config, const std::string& host);
			~ConnectWindow();
		};
	}
}
