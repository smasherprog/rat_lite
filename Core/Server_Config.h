#pragma once
#include <string>
#include "Client_Config.h"

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {
			struct Server_Config: Client_Config {

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

				//CRYPTO SETTINGS
				std::string FullPathToPrivateKey;
				std::string PasswordToPrivateKey;
			};
		}
	}
}