#pragma once
#include <string>
#include "ICryptoLoader.h"

namespace SL {
	namespace RAT {
		class FileCryptoLoaderImpl;
		class FileCryptoLoader : public ICryptoLoader {
			FileCryptoLoaderImpl* _FileCryptoLoaderImpl;
		public:

			FileCryptoLoader(const std::string& filepath);
			virtual ~FileCryptoLoader();

			virtual const char* get_buffer();
			//in bytes
			virtual size_t get_size();

		};
	}
}