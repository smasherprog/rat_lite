#pragma once
#include <memory>
#include "CommonNetwork.h"
#include "BufferManager.h"

extern int inflight;

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			#define HEADERSIZE sizeof(PacketHeader)
			struct Packet_Impl;
			class Packet {

			public:
				//Use this to generate new Packets!
				static std::shared_ptr<Packet> CreatePacket(PacketHeader header);
				Packet(Packet_Impl&);
				~Packet();
				//pointer to the beginning of the packet payload
				char* data() const { return _Data.data; }
				PacketHeader* header() { return &_PacketHeader; }

			private:
				PacketHeader _PacketHeader;
				Utilities::Blk _Data;

			};
		}
	}
}