#pragma once
#include <memory>
#include "CommonNetwork.h"
#include "BufferManager.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			struct Blk;
		}
		namespace Network {
			extern Utilities::BufferManager PacketBufferManager;
			class Packet {
				Packet(const Packet&) = delete;
				Packet& operator=(const Packet&) = delete;

			public:
				//only move allowed, no copy
				Packet(Packet&& pac);
				Packet& operator=(const Packet&& pac);
				Packet(std::shared_ptr<Utilities::Blk>& blk);
				~Packet();
				//this will construct a packet of packetsize bytes
				static std::shared_ptr<Packet> CreatePacket(PACKET_TYPES ptype, size_t packetsize);

				
				//gets a pointer to the beginning of the allocated payload
				char* get_Payload();
				//gets the size of the users payload, NOT including the library headers which are added onto a packet
				size_t get_Payloadsize();
				//get the type of the packet
				PACKET_TYPES get_Packettype();
				std::shared_ptr<Utilities::Blk> get_Blk() const { return _Data; }
			private:
				std::shared_ptr<Utilities::Blk> _Data;
			};
		}
	}
}