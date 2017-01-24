#pragma once
#include "ICryptoLoader.h"

namespace SL {
	namespace Remote_Access_Library {
		class FileCryptoLoaderImpl;
		class FileCryptoLoader : public ICryptoLoader {
			FileCryptoLoaderImpl* _FileCryptoLoaderImpl;
		public:

			FileCryptoLoader(const char* filepath);
			virtual ~FileCryptoLoader();

			virtual const char* get_buffer();
			//in bytes
			virtual size_t get_size();

		};
	}
}