#pragma once
#include <memory>
#include "BufferManager.h"
#include "Commands.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace INTERNAL {
			extern Utilities::BufferManager _PacketBuffer;
		}
		namespace Network {
	

			struct PacketHeader {
				unsigned int PayloadLen = 0;
				unsigned int UnCompressedlen = 0;
				Commands::PACKET_TYPES Packet_Type = Commands::PACKET_TYPES::INVALID;
			};
			#define HEADERSIZE sizeof(PacketHeader)

			struct Packet_Impl;
			class Packet {

			public:
				//Use this to generate new Packets!
				static std::shared_ptr<Packet> CreatePacket(PacketHeader header); 
				template<class T> static std::shared_ptr<Packet> CreatePacket(PacketHeader header, T& commandheader, char* data, size_t len);
				Packet(Packet_Impl&);
				~Packet();
				//pointer to the beginning of the packet payload
				char* data() const;
				PacketHeader* header();

				bool compressed() const;
				//compressionlevel 1- 19
				void compress(int compressionlevel=4);
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