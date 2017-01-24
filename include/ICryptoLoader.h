#pragma once
namespace SL {
	namespace Remote_Access_Library {
		class ICryptoLoader {
		public:
			virtual ~ICryptoLoader() {}

			virtual const char* get_buffer() = 0;
			//in bytes
			virtual size_t get_size() = 0;

		};
	}
}