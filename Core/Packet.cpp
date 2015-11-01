#include <vector>
#include <mutex>
#include <algorithm>
#include "Packet.h"

static BufferManager Buffer_Manager;
namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct Blk {
				size_t size = 0;
				char* data = nullptr;
			};
		}
	}
}
size_t getSize(const std::shared_ptr<SL::Remote_Access_Library::Network::Blk>& b)
{
	return b->size;
}

char* getData(const std::shared_ptr<SL::Remote_Access_Library::Network::Blk>& b)
{
	return b->data;
}

std::shared_ptr<SL::Remote_Access_Library::Network::Blk> AquireBuffer(size_t req_bytes)
{
	return Buffer_Manager.AquireBuffer(req_bytes);
}

void ReleaseBuffer(std::shared_ptr<SL::Remote_Access_Library::Network::Blk>& buffer)
{
	Buffer_Manager.ReleaseBuffer(buffer);
}

class BufferManager {
	size_t _Bytes_Allocated = 0;
	const size_t MAX_ALLOCATED_BYTES = 1024 * 1024 * 64;//64 MB MAX
	std::vector<std::shared_ptr<SL::Remote_Access_Library::Network::Blk>> _Buffer;
	std::mutex _BufferLock;
public:
	std::shared_ptr<SL::Remote_Access_Library::Network::Blk> AquireBuffer(size_t req_bytes) {
		std::lock_guard<std::mutex> lock(_BufferLock);
		auto found = std::remove_if(begin(_Buffer), end(_Buffer), [=](std::shared_ptr<SL::Remote_Access_Library::Network::Blk>& c) {  return c->size >= req_bytes; });

		if (found == _Buffer.end()) {
			auto b = new SL::Remote_Access_Library::Network::Blk;
			b->data = new char[req_bytes];
			b->size = req_bytes;
			return std::shared_ptr<SL::Remote_Access_Library::Network::Blk>(b, [](SL::Remote_Access_Library::Network::Blk* b) { delete[] b->data; delete b; });
		}
		else {
			auto ret(*found);
			_Bytes_Allocated -= ret->size;
			_Buffer.erase(found);
			return ret;
		}
	}
	void ReleaseBuffer(std::shared_ptr<SL::Remote_Access_Library::Network::Blk>& buffer) {
		if (buffer == nullptr) return;
		if (_Bytes_Allocated < MAX_ALLOCATED_BYTES) {//ignore the fact that this will mean our buffer holds more than our maxsize
			std::lock_guard<std::mutex> lock(_BufferLock);
			auto found = std::find(begin(_Buffer), end(_Buffer), buffer);
			if (found == _Buffer.end()) {
				_Bytes_Allocated += buffer->size;
				_Buffer.emplace_back(buffer);
			}
		}//otherwise ignore and let it be reclaimed
	}
};

SL::Remote_Access_Library::Network::Packet::Packet(PACKET_TYPES ptype, size_t packetsize)
{

}

SL::Remote_Access_Library::Network::Packet::~Packet()
{
	Buffer_Manager.ReleaseBuffer(_Data);
}


char* SL::Remote_Access_Library::Network::Packet::get_Payload() {

}

size_t SL::Remote_Access_Library::Network::Packet::get_Payloadsize() {

}
size_t SL::Remote_Access_Library::Network::Packet::get_Packetsize() {

}
SL::Remote_Access_Library::Network::PACKET_TYPES SL::Remote_Access_Library::Network::Packet::get_Packettype() {

}


