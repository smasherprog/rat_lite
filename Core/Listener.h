#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct ListinerImpl;
			struct NetworkEvents;
			class Listener : public std::enable_shared_from_this<Listener> {
			public:
				//factory to create a listening socket
				static std::shared_ptr<Listener> CreateListener(unsigned short port, NetworkEvents& netevents);

				Listener(ListinerImpl* impl);
				~Listener();

				void Start();
				void Stop();
			private:
				ListinerImpl* _ListinerImpl;
			};
		}
	}
}