#pragma once
#include "ICrypoLoader.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Crypto {
			class InMemoryCrypoLoaderImpl;
			class InMemoryCrypoLoader: public ICrypoLoader {
				InMemoryCrypoLoaderImpl* _InMemoryCrypoLoaderImpl;
			public:

				InMemoryCrypoLoader(const char* data, size_t size);
				virtual ~InMemoryCrypoLoader();

				virtual const char* get_buffer();
				//in bytes
				virtual size_t get_size();

			};
		}
	}
}