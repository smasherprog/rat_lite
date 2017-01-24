#pragma once
#include <string>

namespace SL {
	namespace Remote_Access_Library {
		class ICrypoLoader;
		struct Client_Config {

			//both the viewer and web client communicate over web sockets.
			unsigned short HttpTLSPort = 8080;
			unsigned short WebSocketTLSLPort = 6001;
			unsigned int Read_Timeout = 5;//in seconds
			unsigned int Write_Timeout = 5;//in seconds
			bool Share_Clipboard = true;//share your clipboard?
			bool Scale_Image = false;

			std::string Password;//this is the password to connect to the endpoint or that the server requrires before allowing the connection 

			std::shared_ptr<ICrypoLoader> Public_Certficate;
		};

	}
}