#pragma once

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			//pure virtual
			template<typename SOCKTYPE, typename PACKTYPE>class IBaseNetworkDriver {
			public:
				virtual ~IBaseNetworkDriver() {}

				virtual bool ValidateUntrustedCert(const SOCKTYPE& socket) = 0;

				virtual void OnConnect(const SOCKTYPE& socket) = 0;
				virtual void OnReceive(const SOCKTYPE& socket, PACKTYPE& pack) = 0;
				virtual void OnClose(const SOCKTYPE& socket) = 0;
			};
		}
	}
}
