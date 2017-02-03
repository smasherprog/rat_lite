#pragma once
#include <memory>

namespace SL {
	namespace RAT {

		struct MouseEvent;
		struct KeyEvent;

		class IClientDriver;
		class ClientNetworkDriverImpl;
		struct Client_Config;
		class ISocket;
		class ClientNetworkDriver {
			ClientNetworkDriverImpl* _ClientNetworkDriverImpl;

		public:
			ClientNetworkDriver(IClientDriver* r, std::shared_ptr<Client_Config> config, const char* dst_host);
			virtual ~ClientNetworkDriver();

			//after creating ServerNetworkDriver, Start() must be called to start the network processessing
			void Start();
			//Before calling Stop, you must ensure that any external references to shared_ptr<ISocket> have been released
			void Stop();

			void SendKey(const KeyEvent& m);
			void SendMouse(const MouseEvent& m);
			void SendClipboardText(const char* data, unsigned int len);


		};
	}

}
