#pragma once
#include <string>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct Server_Config {
				unsigned short TCPListenPort;
				unsigned short TCPTLSListenPort;
				unsigned short WebSocketListenPort;
				unsigned short WebSocketTLSListenPort;
				unsigned short HttpListenPort;
				unsigned short HttpTLSListenPort;
				std::string WWWRoot;//this is where files are issued out for the webserver
			};
		}
	}
}