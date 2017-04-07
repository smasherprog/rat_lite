#include <thread>
#include <memory>
#include <string>
#include <iostream>

#include "Server.h"
#include "Configs.h"

#if _WIN32
//work around for now for boost on windows
#define BOOST_PROGRAM_OPTIONS_DYN_LINK 
#include "windows/resource.h"
#endif
#include <boost/program_options.hpp>

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

	std::string private_key_path, public_cert_path;


	boost::program_options::options_description desc("Allowed options", 80, 40);
	desc.add_options()
		("help", "<Usage Options>")
		("image_compression", boost::program_options::value<int>(&config->ImageCompressionSetting)->default_value(70)->notifier([](int v) { check_range("image_compression", v, 30, 100); }), "Jpeg Compression setting [30, 100]")
		("https_port", boost::program_options::value<unsigned short>(&config->HttpTLSPort)->default_value(8080), "https listen port")
		("websocket_port", boost::program_options::value<unsigned short>(&config->WebSocketTLSLPort)->default_value(6001), "websocket listen port")
		("share_clipboard", boost::program_options::value<bool>(&config->Share_Clipboard)->default_value(true), "share this servers clipboard with clients")
		("mouse_capture_rate", boost::program_options::value<int>(&config->MousePositionCaptureRate)->default_value(50), "mouse capture rate in ms")
		("screen_capture_rate", boost::program_options::value<int>(&config->ScreenImageCaptureRate)->default_value(100), "screen capture rate in ms")
		("images_as_grayscale", boost::program_options::value<bool>(&config->SendGrayScaleImages)->default_value(0), "send images as grayscale, this improves performance significantly")
		("max_connections", boost::program_options::value<int>(&config->MaxNumConnections)->default_value(10)->notifier([](int v) { check_range("max_connections", v, -1, 100); }), "maximum number of concurrent connections -1 is unlimited")
		("max_websocket_threads", boost::program_options::value<int>(&config->MaxWebSocketThreads)->default_value(2)->notifier([](int v) { check_range("max_websocket_threads", v, 1, 8); }), "maximum number of threads to handle web socket threads numbers between 1 and 8 are valid")
#if defined(DEBUG)  || defined(_DEBUG) || !defined(NDEBUG)
		("private_key_path", boost::program_options::value<std::string>(&config->PathTo_Private_Key)->default_value(TEST_CERTIFICATE_PRIVATE_PATH), "path to the private key file")
		("private_key_password", boost::program_options::value<std::string>(&config->PasswordToPrivateKey)->default_value(TEST_CERTIFICATE_PRIVATE_PASSWORD), "password to the private key file")
		("public_cert_path", boost::program_options::value<std::string>(&config->PathTo_Public_Certficate)->default_value(TEST_CERTIFICATE_PUBLIC_PATH), "path to the public certificate file")
#else 
		("private_key_path", boost::program_options::value<std::string>(&config->PathTo_Private_Key)->required(), "path to the private key file")
		("private_key_password", boost::program_options::value<std::string>(&config->PasswordToPrivateKey)->required(), "password to the private key file")
		("public_cert_path", boost::program_options::value<std::string>(&config->PathTo_Public_Certficate)->required(), "path to the public certificate file")
#endif
		;


	boost::program_options::variables_map vm;
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm);
	}
	catch (std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		std::cout << desc << "\n";
		return 1;
	}
	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}

	SL::RAT::Server serv;
	serv.Start(config);


	return 0;
}