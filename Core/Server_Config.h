#pragma once
#include <string>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct Server_Config {
				//both the viewer and web client communicate over web sockets.
				unsigned short HttpTLSListenPort =8080;
				unsigned short WebSocketTLSListenPort = 6001;
				unsigned int Read_Timeout = 5;//in seconds
				unsigned int Write_Timeout = 5;//in seconds
		

				//this is where files are issued out for the webserver. If no path is specified, wwwroot in applications directory is chosen as a default
				std::string WWWRoot;
				
				unsigned int MouseImageCaptureRate = 1000;// measured in ms
				unsigned int MousePositionCaptureRate = 50;// measured in ms
				unsigned int ScreenImageCaptureRate = 100;// measured in ms

				//Image Settings
				bool SendGrayScaleImages = false;//this will improve bandwith usage
				int ImageCompressionSetting = 70;// this is [0, 100]    = [WORST, BEST]

				bool IgnoreIncomingMouseEvents = false;
				bool IgnoreIncomingKeyboardEvents = false;

				std::string Password;//if this is set, a password is required to connect to this server



				//CRYPTO SETTINGS
				std::string FullPathToCertificate;
				std::string FullPathToPrivateKey;
				std::string PasswordToPrivateKey;
			};
		}
	}
}