#include "stdafx.h"
#include <vector>
#include <mutex>
#include <algorithm>
#include "BufferManager.h"


struct SL::Remote_Access_Library::Utilities::Blk {
	size_t size = 0;
	char* data = nullptr;
};
struct SL::Remote_Access_Library::Utilities::BufferManagerImpl {
	size_t _Bytes_Allocated = 0;
	size_t Max_Buffer_Size = 0;
	std::vector<std::shared_ptr<Blk>> _Buffer;
	std::mutex _BufferLock;
};
size_t SL::Remote_Access_Library::Utilities::getSize(const std::shared_ptr<SL::Remote_Access_Library::Utilities::Blk>& b)
{
	return b->size;
}

char* SL::Remote_Access_Library::Utilities::getData(const std::shared_ptr<SL::Remote_Access_Library::Utilities::Blk>& b)
{
	return b->data;
}

SL::Remote_Access_Library::Utilities::BufferManager::BufferManager(size_t max_buffersize)
{
	_BufferManagerImpl = new BufferManagerImpl();
	_BufferManagerImpl->Max_Buffer_Size = max_buffersize;
}

SL::Remote_Access_Library::Utilities::BufferManager::~BufferManager()
{
	delete _BufferManagerImpl;
}

std::shared_ptr<SL::Remote_Access_Library::Utilities::Blk> SL::Remote_Access_Library::Utilities::BufferManager::AquireBuffer(size_t req_bytes) {
	std::lock_guard<std::mutex> lock(_BufferManagerImpl->_BufferLock);
	auto found = std::remove_if(begin(_BufferManagerImpl->_Buffer), end(_BufferManagerImpl->_Buffer), [=](std::shared_ptr<SL::Remote_Access_Library::Utilities::Blk>& c) {  return c->size >= req_bytes; });

	if (found == _BufferManagerImpl->_Buffer.end()) {
		auto b = new SL::Remote_Access_Library::Utilities::Blk;
		b->data = new char[req_bytes];
		b->size = req_bytes;
		return std::shared_ptr<SL::Remote_Access_Library::Utilities::Blk>(b, [](SL::Remote_Access_Library::Utilities::Blk* b) { delete[] b->data; delete b; });
	}
	else {
		auto ret(*found);
		_BufferManagerImpl->_Bytes_Allocated -= ret->size;
		_BufferManagerImpl->_Buffer.erase(found);
		return ret;
	}
}
void SL::Remote_Access_Library::Utilities::BufferManager::ReleaseBuffer(std::shared_ptr<SL::Remote_Access_Library::Utilities::Blk>& buffer) {
	if (buffer == nullptr) return;
	if (_BufferManagerImpl->_Bytes_Allocated < _BufferManagerImpl->Max_Buffer_Size) {//ignore the fact that this will mean our buffer holds more than our maxsize
		std::lock_guard<std::mutex> lock(_BufferManagerImpl->_BufferLock);
		auto found = std::find(begin(_BufferManagerImpl->_Buffer), end(_BufferManagerImpl->_Buffer), buffer);
		if (found == _BufferManagerImpl->_Buffer.end()) {
			_BufferManagerImpl->_Bytes_Allocated += buffer->size;
			_BufferManagerImpl->_Buffer.emplace_back(buffer);
		}
	}//otherwise ignore and let it be reclaimed
}
