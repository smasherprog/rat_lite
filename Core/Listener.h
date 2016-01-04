#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace INTERNAL {
			class ListinerImpl;
		}
		namespace Network {
		
			class BaseNetworkDriver;
			class Listener : public std::enable_shared_from_this<Listener> {
			public:
				//factory to create a listening socket
				static std::shared_ptr<Listener> CreateListener(unsigned short port, Network::BaseNetworkDriver* netevents);

				Listener(INTERNAL::ListinerImpl* impl);
				~Listener();

				void Start();
			
			private:
				INTERNAL::ListinerImpl* _ListinerImpl;
			};
		}
	}
}