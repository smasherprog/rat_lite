#pragma once
#include <memory>

namespace SL {
	namespace RAT {

		struct MouseEvent;
		struct KeyEvent;
		class IClientDriver;
		class ClientNetworkDriverImpl;
		struct Client_Config;
		class ClientNetworkDriver {
			ClientNetworkDriverImpl* _ClientNetworkDriverImpl;

		public:
			ClientNetworkDriver(IClientDriver* r );
			virtual ~ClientNetworkDriver();

			void Connect(std::shared_ptr<Client_Config> config, const char* dst_host);

			void SendKey(const KeyEvent& m);
			void SendMouse(const MouseEvent& m);
			void SendClipboardText(const char* data, unsigned int len);


		};
	}

}
