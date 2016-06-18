#include "stdafx.h"
#include "InMemoryCrypoLoader.h"
#include <vector>
#include <assert.h>
#include "Logging.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Crypto {
			class InMemoryCrypoLoaderImpl {
			public:
				InMemoryCrypoLoaderImpl(const char* data, size_t size) {
					Data.resize(size);
					memcpy(Data.data(), data, size);
				}
				~InMemoryCrypoLoaderImpl() {
					memset(Data.data(), 0, Data.size());
				}
				const char* get_buffer() {
					//data cannot be empty!
					if(Data.empty()) SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "Data not found!");
					return Data.data();
				}
				size_t get_size() {
					//data cannot be empty!
					assert(!Data.empty());
					return Data.size();
				}
				std::vector<char> Data;

			};
		}
	}
}


SL::Remote_Access_Library::Crypto::InMemoryCrypoLoader::InMemoryCrypoLoader(const char* data, size_t size)
{
	_InMemoryCrypoLoaderImpl = new InMemoryCrypoLoaderImpl(data, size);
}

SL::Remote_Access_Library::Crypto::InMemoryCrypoLoader::~InMemoryCrypoLoader()
{

	delete _InMemoryCrypoLoaderImpl;
}

const char* SL::Remote_Access_Library::Crypto::InMemoryCrypoLoader::get_buffer()
{
	return _InMemoryCrypoLoaderImpl->get_buffer();
}

size_t SL::Remote_Access_Library::Crypto::InMemoryCrypoLoader::get_size()
{
	return _InMemoryCrypoLoaderImpl->get_size();
}
