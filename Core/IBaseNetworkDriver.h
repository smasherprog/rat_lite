#pragma once

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class ISocket;
			class Packet;
			//pure virtual
			class IBaseNetworkDriver {
			public:
				virtual ~IBaseNetworkDriver() {}

				virtual bool ValidateUntrustedCert(const std::shared_ptr<ISocket>& socket) = 0;

				virtual void OnConnect(const std::shared_ptr<ISocket>& socket) = 0;
				virtual void OnReceive(const std::shared_ptr<ISocket>& socket, std::shared_ptr<Packet>& pack) = 0;
				virtual void OnClose(const ISocket* socket) = 0;
			};
		}
	}
}
