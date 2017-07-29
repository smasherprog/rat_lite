#include <thread>
#include <memory>
#include <string>
#include <iostream>

#include "cxxopts.hpp"
#include "Server.h"
#include "Configs.h"

#if _WIN32
#include "windows/resource.h"
#endif

template<typename T>
void check_range(const std::string& name, T& value, const T& min, const T& max)
{
	if (value < min || value > max)
	{
		std::cout << name << " must be between [" << std::to_string(min) << " and " << std::to_string(max) << "] Value found " << std::to_string(value) << std::endl;
		exit(1);
	}
}

int main(int argc, char* argv[]) {

	auto config = std::make_shared<SL::RAT::Server_Config>();

	config->WebSocketTLSLPort = 6001;// listen for websockets
	config->HttpTLSPort = 8080;
	config->Share_Clipboard = true;

    cxxopts::Options options("Remote Access Server", "<Usage Options>");
    options.add_options()
        ("help", "<Usage Options>")
		("image_compression", "Jpeg Compression setting [30, 100]", cxxopts::value<int>(config->ImageCompressionSetting))
		("https_port", "https listen port", cxxopts::value<unsigned short>(config->HttpTLSPort)->default_value("8080"))
		("websocket_port", "websocket listen port", cxxopts::value<unsigned short>(config->WebSocketTLSLPort)->default_value("6001"))
		("share_clipboard", "share this servers clipboard with clients", cxxopts::value<bool>(config->Share_Clipboard))
		("mouse_capture_rate", "mouse capture rate in ms", cxxopts::value<int>(config->MousePositionCaptureRate)->default_value("50"))
		("screen_capture_rate", "screen capture rate in ms", cxxopts::value<int>(config->ScreenImageCaptureRate)->default_value("100"))
		("images_as_grayscale", "send images as grayscale, this improves performance significantly", cxxopts::value<bool>(config->SendGrayScaleImages))
		("max_connections", "maximum number of concurrent connections -1 is unlimited", cxxopts::value<int>(config->MaxNumConnections)->default_value("10"))
		("max_websocket_threads", "maximum number of threads to handle web socket threads numbers between 1 and 8 are valid", cxxopts::value<int>(config->MaxWebSocketThreads)->default_value("2"))
#if defined(DEBUG)  || defined(_DEBUG) || !defined(NDEBUG)
		("private_key_path", "path to the private key file", cxxopts::value<std::string>(config->PathTo_Private_Key)->default_value(TEST_CERTIFICATE_PRIVATE_PATH))
		("private_key_password", "password to the private key file", cxxopts::value<std::string>(config->PasswordToPrivateKey)->default_value(TEST_CERTIFICATE_PRIVATE_PASSWORD))
		("public_cert_path", "path to the public certificate file", cxxopts::value<std::string>(config->PathTo_Public_Certficate)->default_value(TEST_CERTIFICATE_PUBLIC_PATH))
#else 
		("private_key_path", cxxopts::value<std::string>(config->PathTo_Private_Key), "path to the private key file")
		("private_key_password", cxxopts::value<std::string>(config->PasswordToPrivateKey), "password to the private key file")
		("public_cert_path", cxxopts::value<std::string>(config->PathTo_Public_Certficate), "path to the public certificate file")
#endif
		;

    options.parse(argc, argv);
    if (options.count("help"))
    {
        std::cout << options.help({ "", "Group" }) << std::endl;
        exit(0);
    }

    check_range("image_compression", config->ImageCompressionSetting, 30, 100);
    check_range("max_connections", config->MaxNumConnections, -1, 100);
    check_range("max_websocket_threads", config->MaxWebSocketThreads, 1, 8);

	SL::RAT::Server serv(config);
	serv.Run();
	return 0;
}