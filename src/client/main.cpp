#include <memory>
#include <iostream>
#include "Configs.h"
#include "cxxopts.hpp"

#if _WIN32
#include "windows/resource.h"
#endif

int main(int argc, char* argv[]) {

	auto config = std::make_shared<SL::RAT::Client_Config>();

	std::string host;
    cxxopts::Options options("Remote Access Client", "<Usage Options>");
    options.add_options()
		("help", "<Usage Options>")
        ("https_port", "https listen port", cxxopts::value<unsigned short>(config->HttpTLSPort)->default_value("8080"))
        ("websocket_port", "websocket listen port", cxxopts::value<unsigned short>(config->WebSocketTLSLPort)->default_value("6001"))
        ("share_clipboard", "share this servers clipboard with clients", cxxopts::value<bool>(config->Share_Clipboard))
		("host", "enter a host or ip address to initite a connection", cxxopts::value<std::string>(host))
#if defined(DEBUG)  || defined(_DEBUG) || !defined(NDEBUG)
		("public_cert_path", "path to the public certificate file", cxxopts::value<std::string>(config->PathTo_Public_Certficate)->default_value(TEST_CERTIFICATE_PUBLIC_PATH))
#else 
		("public_cert_path", "path to the public certificate file", cxxopts::value<std::string>(config->PathTo_Public_Certficate))
#endif
		;

    options.parse(argc, argv);
    if (options.count("help"))
    {
        std::cout << options.help({ "", "Group" }) << std::endl;
        exit(0);
    }

	//SL::RAT::ConnectWindow c(config, host);

	return 0;
}