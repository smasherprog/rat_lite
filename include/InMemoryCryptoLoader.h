#pragma once
#include "ICryptoLoader.h"

namespace SL {
	namespace Remote_Access_Library {
		class InMemoryCryptoLoaderImpl;
		class InMemoryCryptoLoader : public ICryptoLoader {
			InMemoryCryptoLoaderImpl* _InMemoryCryptoLoaderImpl;
		public:

			InMemoryCryptoLoader(const char* data, size_t size);
			virtual ~InMemoryCryptoLoader();

			virtual const char* get_buffer();
			//in bytes
			virtual size_t get_size();

		};
	}
}