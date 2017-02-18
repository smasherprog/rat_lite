#pragma once
#include <string>
#include <memory>
#include "Logging.h"
#include "Shapes.h"
#include "ISocket.h"
#include "INetworkHandlers.h"

namespace SL {
	namespace RAT {

		struct Client_Config {

			//both the viewer and web client communicate over web sockets.
			unsigned short HttpTLSPort = 8080;
			unsigned short WebSocketTLSLPort = 6001;
			unsigned int Read_Timeout = 5;//in seconds
			unsigned int Write_Timeout = 5;//in seconds
			bool Share_Clipboard = true;//share your clipboard?
			bool Scale_Image = false;

			std::string PathTo_Public_Certficate;
		};

		struct Server_Config : Client_Config {

			//this is where files are issued out for the webserver. If no path is specified, wwwroot in applications directory is chosen as a default
			std::string WWWRoot;

			int MousePositionCaptureRate = 50;// measured in ms
			int ScreenImageCaptureRate = 100;// measured in ms

											 //Image Settings
			bool SendGrayScaleImages = false;//this will improve bandwith usage
			int ImageCompressionSetting = 70;// this is [0, 100]    = [WORST, BEST]

			bool IgnoreIncomingMouseEvents = false;
			bool IgnoreIncomingKeyboardEvents = false;

			size_t MaxWebSocketThreads = 2;//
			int MaxNumConnections = 10;//

									   //CRYPTO SETTINGS
			std::string PathTo_Private_Key;
			std::string PasswordToPrivateKey;
		};
	}
}