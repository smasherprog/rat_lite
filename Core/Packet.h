#pragma once
#include <memory>
#include "CommonNetwork.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			struct Blk;
			size_t getSize(const std::shared_ptr<Blk>& b);
			char* getData(const std::shared_ptr<Blk>& b);

			class Packet {
				Packet(const Packet&) = delete;
				Packet& operator=(const Packet&) = delete;

			public:
				//only move allowed, no copy
				Packet(Packet&& pac) : _Data(std::move(pac._Data)) {}
				Packet& operator=(const Packet&& pac) {
					_Data = std::move(pac._Data);
				}
				//this will construct a packet of packetsize bytes
				Packet(std::shared_ptr<Blk>& blk);
				static std::shared_ptr<Packet> CreatePacket(PACKET_TYPES ptype, size_t packetsize);

				~Packet();
				//gets a pointer to the beginning of the allocated payload
				char* get_Payload();
				//gets the size of the users payload, NOT including the library headers which are added onto a packet
				size_t get_Payloadsize();
				//gets the type of the packet
				PACKET_TYPES get_Packettype();
				std::shared_ptr<Blk> get_Blk() const { return _Data; }
			private:
				std::shared_ptr<Blk> _Data;
			};
		}
	}
}