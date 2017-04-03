#pragma once
#include <memory>

namespace SL {
	namespace RAT {

		struct MouseEvent;
		struct KeyEvent;
		class IClientDriver;
		class ClientDriverImpl;
		struct Client_Config;
		class ClientDriver {
			ClientDriverImpl* ClientDriverImpl_;

		public:
			ClientDriver(IClientDriver* r );
			virtual ~ClientDriver();

			void Connect(std::shared_ptr<Client_Config> config, const char* dst_host);

			void SendKey(const KeyEvent& m);
			void SendMouse(const MouseEvent& m);
			void SendClipboardText(const char* data, unsigned int len);


		};
	}

}
