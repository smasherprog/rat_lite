#pragma once
#include <memory>
#include <string>

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

            void SendKeyEvent(const KeyEvent& kevent);
            void SendMouseEvent(const MouseEvent& mevent);
            void SendClipboardChanged(const std::string& text);


		};
	}

}
