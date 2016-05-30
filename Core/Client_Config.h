#pragma once
#include <string>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct Client_Config {
				//both the viewer and web client communicate over web sockets.
				unsigned short HttpTLSPort =8080;
				unsigned short WebSocketTLSLPort = 6001;
				unsigned int Read_Timeout = 5;//in seconds
				unsigned int Write_Timeout = 5;//in seconds

				std::string Password;//if this is set, a password is required to connect to this server

			};
		}
	}
}