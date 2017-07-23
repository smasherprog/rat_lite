#pragma once
#include <string>
#include <memory>

namespace SL {
	namespace RAT {
		struct Client_Config;
		class ConnectWindowImpl;
		class ConnectWindow {
			ConnectWindowImpl* ConnectWindowImpl_ = nullptr;
		public:
			ConnectWindow(const std::shared_ptr<Client_Config> config, const std::string& host);
			~ConnectWindow();
		};
	}
}
