#include "ClientWindow.h"
#include "cxxopts.hpp"
#include <iostream>
#include <memory>

int main(int argc, char *argv[])
{

    std::string host, pathtocert;
    unsigned short httpport = 8080;
    unsigned short tlsport = 6001;
    bool shareclipboard = false;

    cxxopts::Options options("Remote Access Client", "<Usage Options>");
    options.add_options()("help", "<Usage Options>")("https_port", "https listen port",
                                                     cxxopts::value<unsigned short>(httpport)->default_value("8080"))(
        "websocket_port", "websocket listen port", cxxopts::value<unsigned short>(tlsport)->default_value("6001"))(
        "share_clipboard", "share this servers clipboard with clients",
        cxxopts::value<bool>(shareclipboard))("host", "enter a host or ip address to initite a connection", cxxopts::value<std::string>(host))
#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
        ("public_cert_path", "path to the public certificate file",
         cxxopts::value<std::string>(pathtocert)->default_value(TEST_CERTIFICATE_PUBLIC_PATH))
#else
        ("public_cert_path", "path to the public certificate file", cxxopts::value<std::string>(pathtocert))
#endif
        ;

    options.parse(argc, argv);
    if (options.count("help")) {
        std::cout << options.help({"", "Group"}) << std::endl;
        exit(0);
    }
    SL::RAT_Client::ClientWindow c(host, tlsport);
    c.ShareClipboard(shareclipboard);
    c.Run();
    return 0;
}