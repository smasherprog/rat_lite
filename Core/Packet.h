#pragma once
#include <memory>
#include "Commands.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			struct PacketHeader {
				unsigned int PayloadLen = 0;
				unsigned int UnCompressedlen = 0;
				unsigned int Packet_Type = 0;
			};
			#define HEADERSIZE sizeof(PacketHeader)

			struct Packet_Impl;
			class Packet {

			public:
			
				static std::shared_ptr<Packet> CreatePacket(PacketHeader header); 

				Packet(Packet_Impl&);
				~Packet();
				//pointer to the beginning of the packet payload
				char* data() const;
				PacketHeader* header();

			private:
				PacketHeader _PacketHeader;
				std::unique_ptr<char[]> _Data;
			};
		}
	}
}