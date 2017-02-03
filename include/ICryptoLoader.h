#pragma once
namespace SL {
	namespace RAT {
		class ICryptoLoader {
		public:
			virtual ~ICryptoLoader() {}

			virtual const char* get_buffer() = 0;
			//in bytes
			virtual size_t get_size() = 0;

		};
	}
}