#include "stdafx.h"
#include <vector>
#include <mutex>
#include <algorithm>
#include "BufferManager.h"


struct SL::Remote_Access_Library::Utilities::BufferManagerImpl {
	size_t _Bytes_Allocated = 0;
	size_t Max_Buffer_Size = 0;
	std::vector<Blk> _Buffer;
	std::mutex _BufferLock;
};

SL::Remote_Access_Library::Utilities::BufferManager::BufferManager(size_t max_buffersize)
{
	_BufferManagerImpl = new BufferManagerImpl();
	_BufferManagerImpl->Max_Buffer_Size = max_buffersize;
}

SL::Remote_Access_Library::Utilities::BufferManager::~BufferManager()
{
	delete _BufferManagerImpl;
}

SL::Remote_Access_Library::Utilities::Blk SL::Remote_Access_Library::Utilities::BufferManager::AquireBuffer(size_t req_bytes) {
	if (req_bytes == 0) return Blk();
	std::lock_guard<std::mutex> lock(_BufferManagerImpl->_BufferLock);
	auto found = std::remove_if(begin(_BufferManagerImpl->_Buffer), end(_BufferManagerImpl->_Buffer), [=](SL::Remote_Access_Library::Utilities::Blk& c) {  return c.size >= req_bytes; });

	if (found == _BufferManagerImpl->_Buffer.end()) {
		SL::Remote_Access_Library::Utilities::Blk b;
		b.data = new char[req_bytes];
		b.size = req_bytes;
		return b;
	}
	else {
		auto ret(*found);
		_BufferManagerImpl->_Bytes_Allocated -= ret.size;
		_BufferManagerImpl->_Buffer.erase(found);
		return ret;
	}
}
void SL::Remote_Access_Library::Utilities::BufferManager::ReleaseBuffer(SL::Remote_Access_Library::Utilities::Blk& buffer) {
	if (buffer.data == nullptr || buffer.size == 0) return;
	if (_BufferManagerImpl->_Bytes_Allocated < _BufferManagerImpl->Max_Buffer_Size) {//ignore the fact that this will mean our buffer holds more than our maxsize
		std::lock_guard<std::mutex> lock(_BufferManagerImpl->_BufferLock);
		_BufferManagerImpl->_Bytes_Allocated += buffer.size;
		_BufferManagerImpl->_Buffer.push_back(buffer);
	}
	else {
		delete[] buffer.data;//free the buffer!
		buffer.data = nullptr;
		buffer.size = 0;
	}
}
