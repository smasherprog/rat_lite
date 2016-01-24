#pragma once
namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct Server_Config {
				unsigned short TCPListenPort;
				unsigned short WebSocketListenPort;
			};
		}
	}
}