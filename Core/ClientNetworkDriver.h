#pragma once
#include "IBaseNetworkDriver.h"
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Input {
			struct MouseEvent;
			struct KeyEvent;
		}
		namespace Network {
			class IClientDriver;
			class ClientNetworkDriverImpl;
			struct Client_Config;

			class ClientNetworkDriver {
				ClientNetworkDriverImpl* _ClientNetworkDriverImpl;

			public:
				ClientNetworkDriver(IClientDriver* r, std::shared_ptr<Client_Config> config, const char* dst_host);
				virtual ~ClientNetworkDriver();

				//after creating ServerNetworkDriver, Start() must be called to start the network processessing
				void Start();
				//Before calling Stop, you must ensure that any external references to shared_ptr<ISocket> have been released
				void Stop();

				void SendKey(const Input::KeyEvent& m);
				void SendMouse(const Input::MouseEvent& m);
				void SendClipboardText(const char* data, unsigned int len);

                std::shared_ptr<ISocket> get_Socket()const;
				bool ConnectedToSelf() const;
			};
		}
	}
}
