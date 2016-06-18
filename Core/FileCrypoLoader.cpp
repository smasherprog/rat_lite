#include "stdafx.h"
#include "FileCrypoLoader.h"
#include <string>
#include <vector>
#include <fstream>
#include <assert.h>
#include "Logging.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Crypto {
			class FileCrypoLoaderImpl {
			public:
				FileCrypoLoaderImpl(const char* filepath) :FilePath(filepath) {}
				~FileCrypoLoaderImpl() {
					memset(Data.data(), 0, Data.size());
				}
				const char* get_buffer() {
					LoadData();
					return Data.data();
				}
				size_t get_size() {
					LoadData();
					return Data.size();
				}
				void LoadData() {
					if (Data.empty()) {
						std::ifstream file(FilePath, std::ios::binary | std::ios::ate);
						//if file is not found, ASSERT!.. FILE NOT FOUND!
						if (file) {
							std::streamsize size = file.tellg();
							file.seekg(0, std::ios::beg);
							Data.resize(size);
							if (!file.read(Data.data(), size)) {
								Data.resize(0);
							}
						}
						else {
							SL_RAT_LOG(Utilities::Logging_Levels::ERROR_log_level, "File " << FilePath << "not found!");
						}

					}
				}
				std::vector<char> Data;
				std::string FilePath;

			};
		}
	}
}


SL::Remote_Access_Library::Crypto::FileCrypoLoader::FileCrypoLoader(const char* filepath)
{
	_FileCrypoLoaderImpl = new FileCrypoLoaderImpl(filepath);
}

SL::Remote_Access_Library::Crypto::FileCrypoLoader::~FileCrypoLoader()
{

	delete _FileCrypoLoaderImpl;
}

const char* SL::Remote_Access_Library::Crypto::FileCrypoLoader::get_buffer()
{
	return _FileCrypoLoaderImpl->get_buffer();
}

size_t SL::Remote_Access_Library::Crypto::FileCrypoLoader::get_size()
{
	return _FileCrypoLoaderImpl->get_size();
}
