#pragma once
#include <memory>
#include <string>
#include "Input_Lite.h"

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

            void SendMouseEvent(const MouseEvent& mevent);
            void SendClipboardChanged(const std::string& text);

            void SendKeyUp(char key);
            void SendKeyUp(wchar_t key);
            void SendKeyUp(Input_Lite::SpecialKeyCodes key);

            void SendKeyDown(char key);
            void SendKeyDown(wchar_t key);
            void SendKeyDown(Input_Lite::SpecialKeyCodes key);

		};
	}

}
