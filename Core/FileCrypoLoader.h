#pragma once
#include "ICrypoLoader.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Crypto {
			class FileCrypoLoaderImpl;
			class FileCrypoLoader:public ICrypoLoader {
				FileCrypoLoaderImpl* _FileCrypoLoaderImpl;
			public:

				FileCrypoLoader(const char* filepath);
				virtual ~FileCrypoLoader();

				virtual const char* get_buffer();
				//in bytes
				virtual size_t get_size();

			};
		}
	}
}