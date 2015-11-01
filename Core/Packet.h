#pragma once
#include <memory>
namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			enum PACKET_TYPES {
				INVALID,
				RESOLUTIONCHANGE,
				UPDATEREGION,
				MOUSEEVENT,
				KEYEVENT,
				FOLDER,
				FILE,
				CAD,
				INIT_ENCRYPTION,
				CLIPBOARDCHANGED,
				DISCONNECTANDREMOVE,
				SETTINGS,
				ELEVATEPROCESS,
				CONNECT_REQUEST,
				CONNECT_REQUEST_FAILED,
				KEEPALIVE,
				UAC_BLOCKED,
				ELEVATE_SUCCESS,
				ELEVATE_FAILED
			};

			struct Blk;

			class Packet {
			public:
				//this will construct a packet of packetsize bytes
				Packet(PACKET_TYPES ptype, size_t packetsize);
				~Packet();
				//gets a pointer to the beginning of the allocated payload
				char* get_Payload();
				//gets the size of the users payload, NOT including the library headers which are added onto a packet
				size_t get_Payloadsize();
				//gets the entire packets size including the extra headers prepended by the library 
				size_t get_Packetsize();
				//gets the type of the packet
				PACKET_TYPES get_Packettype();
			private:
				std::shared_ptr<Blk> _Data;
			};
		}
	}
}