#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct ListinerImpl;
			struct NetworkEvents;
			class Listener : public std::enable_shared_from_this<Listener> {
			public:
				//does not start listening until start is called
				Listener(unsigned short port, NetworkEvents& netevents);
				~Listener();

				void Start();
				void Stop();
			private:
				ListinerImpl* _ListinerImpl;
			};
		}
	}
}