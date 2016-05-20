#pragma once
#include <string>

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct Server_Config {
				//both the viewer and web client communicate over web sockets.
				unsigned short WebSocketListenPort = 0;
				unsigned short WebSocketTLSListenPort = 0;
				unsigned int Read_Timeout = 5;//in seconds
				unsigned int Write_Timeout = 5;//in seconds
				unsigned short HttpListenPort = 0;
				unsigned short HttpTLSListenPort = 0;
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
			};
		}
	}
}