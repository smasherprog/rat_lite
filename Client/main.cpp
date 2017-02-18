#include <FL/Fl.H>
#include <memory>
#include <iostream>
#include "ConnectWindow.h"
#include "RAT.h"

#if _WIN32
//work around for now for boost on windows
#define BOOST_PROGRAM_OPTIONS_DYN_LINK 
#include "windows/resource.h"
#endif
#include <boost/program_options.hpp>


int main(int argc, char* argv[]) {
	Fl::args(argc, argv);
	Fl::get_system_colors();
	Fl::visual(FL_RGB);

	auto config = std::make_shared<SL::RAT::Client_Config>();

	std::string host;


	boost::program_options::options_description desc("Allowed options", 80, 40);
	desc.add_options()
		("help", "<Usage Options>")
		("https_port", boost::program_options::value<unsigned short>(&config->HttpTLSPort)->default_value(8080), "https connetion port")
		("websocket_port", boost::program_options::value<unsigned short>(&config->WebSocketTLSLPort)->default_value(6001), "websocket connection port")
		("share_clipboard", boost::program_options::value<bool>(&config->Share_Clipboard)->default_value(true), "share this clients clipboard with the server")
		("host", boost::program_options::value<std::string>(&host), "enter a host or ip address to initite a connection")
#if defined(DEBUG)  || defined(_DEBUG) || !defined(NDEBUG)
		("public_cert_path", boost::program_options::value<std::string>(&config->PathTo_Public_Certficate)->default_value(TEST_CERTIFICATE_PUBLIC_PATH), "path to the public certificate file")
#else 
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


	SL::RAT::ConnectWindow c(config, host);
	Fl::lock();
	while (Fl::wait() > 0) {
		if (Fl::thread_message()) {

		}
	}
	return 0;
}