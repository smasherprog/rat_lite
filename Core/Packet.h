#pragma once
#include <memory>
#include "BufferManager.h"
#include <vector>
#include "Commands.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			#define HEADERSIZE sizeof(PacketHeader)

			struct PacketHeader {
				unsigned int PayloadLen = 0;
				unsigned int UnCompressedlen = 0;
				Commands::PACKET_TYPES Packet_Type = Commands::PACKET_TYPES::INVALID;
			};

			struct Packet_Impl;
			class Packet {

			public:
				//Use this to generate new Packets!
				static std::shared_ptr<Packet> CreatePacket(PacketHeader header); 
				template<class T> static std::shared_ptr<Packet> CreatePacket(PacketHeader header, T& commandheader, char* data, size_t len);
				Packet(Packet_Impl&);
				~Packet();
				//pointer to the beginning of the packet payload
				char* data() const { return _Data.data; }
				PacketHeader* header() { return &_PacketHeader; }

				//compresses the packet, so make sure not to write data after this, otherwise you could corrupt the data.  tempbuffer is for performance: ompressing packets requires another buffer to copy data to, so this allows a prevous buffer to be reused. If you dont pass a buffer, one will be created and used for each call to compress
				void compress();
				//this decompresses the packet, tempbuffer is for performance: decompressing packets requires another buffer to copy data to, so this allows a prevous buffer to be reused. If you dont pass a buffer, one will be created and used for each call to decompress
				void decompress();

			private:
				PacketHeader _PacketHeader;
				Utilities::Blk _Data;

			};
			template<class T>
			inline std::shared_ptr<Packet> Packet::CreatePacket(PacketHeader header, T & commandheader, char * data, size_t len)
			{
				auto pack = Packet::CreatePacket(header);
				memcpy(pack->data(), &commandheader, sizeof(T));
				memcpy(pack->data() + sizeof(T), data, len);
				return pack;
			}
		}
	}
}