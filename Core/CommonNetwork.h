#pragma once
#include <functional>
#include <memory>
#include <iostream>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			class Socket;
			class Packet;
			inline void DefaultOnConnect(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) { std::cout << "DEFAULT OnConnected Called!" << std::endl; };
			inline void DefaultOnReceive(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr, std::shared_ptr<SL::Remote_Access_Library::Network::Packet>& pac) { std::cout << "DEFAULT OnReceive Called!" <<  std::endl; };
			inline void DefaultOnClose(const std::shared_ptr<SL::Remote_Access_Library::Network::Socket> ptr) { std::cout << "DEFAULT OnClose Called!" << std::endl; };
			
			struct NetworkEvents {
				std::function<void(const std::shared_ptr<Socket>)> OnConnect = DefaultOnConnect;
				std::function<void(const std::shared_ptr<Socket>, std::shared_ptr<Packet>&)> OnReceive = DefaultOnReceive;
				std::function<void(const std::shared_ptr<Socket>)> OnClose = DefaultOnClose;
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
				unsigned int UnCompressedlen = 0;
				PACKET_TYPES Packet_Type = PACKET_TYPES::INVALID;
			};



		}
	}
}