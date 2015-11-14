#pragma once
#include <functional>
#include <memory>
#include <iostream>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			class Socket;
			class Packet;
			void DefaultOnConnect(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr);
			void DefaultOnReceive(const Socket* ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac);
			void DefaultOnClose(const Socket* ptr);
			
			struct NetworkEvents {
				std::function<void(const std::shared_ptr<Socket>)> OnConnect = DefaultOnConnect;
				std::function<void(const Socket*, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>&)> OnReceive = DefaultOnReceive;
				std::function<void(const Socket*)> OnClose = DefaultOnClose;
			};

			enum class PACKET_TYPES : unsigned int {
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
				unsigned int UnCompressedlen = 0;
				PACKET_TYPES Packet_Type = PACKET_TYPES::INVALID;
			}; 
		

		}
	}
}