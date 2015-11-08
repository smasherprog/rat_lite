#pragma once
#include <memory>


namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			
			struct Blk {
				size_t size = 0;
				char* data = nullptr;
			};
			struct BufferManagerImpl;
			class BufferManager {
				BufferManagerImpl* _BufferManagerImpl;
			public:
				BufferManager(size_t max_buffersize = 1024 *1024 *64);//default 64MBs
				~BufferManager();
				Blk AquireBuffer(size_t req_bytes);
				void ReleaseBuffer(Blk& buffer);
			};

		}
	}
}