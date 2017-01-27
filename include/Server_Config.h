#pragma once
#include <string>
#include "Client_Config.h"

namespace SL {
	namespace Remote_Access_Library {
		class ICryptoLoader;
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
			size_t MaxNumConnections = 10;//

			//CRYPTO SETTINGS
			std::shared_ptr<ICryptoLoader> Private_Key;
			std::string PasswordToPrivateKey;
		};

	}
}