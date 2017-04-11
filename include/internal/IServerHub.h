#pragma once

namespace SL {
	namespace RAT {

		class IServerHub {
		public:
			virtual ~IServerHub() {}
			virtual void Broadcast(char* data, size_t len) = 0;

		};
	}
}
