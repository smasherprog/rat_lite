#pragma once
#include <functional>
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class Socket;
			class Packet;
	
			struct NetworkEvents {
				std::function<void(const Socket*)> OnConnect;
				std::function<void(const Socket*, std::shared_ptr<Packet>&)> OnReceive;
				std::function<void(const Socket*)> OnClose;
			};

			enum class PACKET_TYPES : unsigned char {
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
			struct PacketHeader {
				unsigned int PayloadLen = 0;
				PACKET_TYPES Packet_Type = PACKET_TYPES::INVALID;
			};



		}
	}
}