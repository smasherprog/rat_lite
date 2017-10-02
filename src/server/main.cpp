#include "Server.h"
#include "cxxopts.hpp"
#include <iostream>
#include <string>

#if _WIN32
#include "windows/resource.h"
#endif

template <typename T> void check_range(const std::string &name, T &value, const T &min, const T &max)
{
    if (value < min || value > max) {
        std::cout << name << " must be between [" << std::to_string(min) << " and " << std::to_string(max) << "] Value found "
                  << std::to_string(value) << std::endl;
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    std::string PasswordToPrivateKey, PathTo_Private_Key, PathTo_Public_Certficate;
    unsigned short tlsport = 6001;
    bool shareclipboard = false;
    int ImageCompressionSetting = 70;
    int MouseCaptureRate = 50;
    int ScreenImageCaptureRate = 100;
    bool SendGrayScaleImages = false;
    int MaxNumConnections = 10;
    int MaxWebSocketThreads = 1;

    cxxopts::Options options("Remote Access Server", "<Usage Options>");
    options.add_options()("help", "<Usage Options>")("image_compression", "Jpeg Compression setting [30, 100]",
                                                     cxxopts::value<int>(ImageCompressionSetting))(
        "websocket_port", "websocket listen port", cxxopts::value<unsigned short>(tlsport)->default_value("6001"))(
        "share_clipboard", "share this servers clipboard with clients", cxxopts::value<bool>(shareclipboard))(
        "mouse_capture_rate", "mouse capture rate in ms", cxxopts::value<int>(MouseCaptureRate)->default_value("50"))(
        "screen_capture_rate", "screen capture rate in ms", cxxopts::value<int>(ScreenImageCaptureRate)->default_value("100"))(
        "images_as_grayscale", "send images as grayscale, this improves performance significantly", cxxopts::value<bool>(SendGrayScaleImages))(
        "max_connections", "maximum number of concurrent connections -1 is unlimited", cxxopts::value<int>(MaxNumConnections)->default_value("10"))(
        "max_websocket_threads", "maximum number of threads to handle web socket threads numbers between 1 and 8 are valid",
        cxxopts::value<int>(MaxWebSocketThreads)->default_value("2"))
#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
        ("private_key_path", "path to the private key file",
         cxxopts::value<std::string>(PathTo_Private_Key)->default_value(TEST_CERTIFICATE_PRIVATE_PATH))(
            "private_key_password", "password to the private key file",
            cxxopts::value<std::string>(PasswordToPrivateKey)->default_value(TEST_CERTIFICATE_PRIVATE_PASSWORD))(
            "public_cert_path", "path to the public certificate file",
            cxxopts::value<std::string>(PathTo_Public_Certficate)->default_value(TEST_CERTIFICATE_PUBLIC_PATH))
#else
        ("private_key_path", "path to the private key file", cxxopts::value<std::string>(PathTo_Private_Key))(
            "private_key_password", "password to the private key file", cxxopts::value<std::string>(PasswordToPrivateKey))(
            "public_cert_path", "path to the public certificate file", cxxopts::value<std::string>(PathTo_Public_Certficate))
#endif
        ;

    options.parse(argc, argv);
    if (options.count("help")) {
        std::cout << options.help({"", "Group"}) << std::endl;
        exit(0);
    }

    SL::RAT_Server::Server serv;
    serv.ShareClipboard(shareclipboard);
    serv.ImageCompressionSetting(ImageCompressionSetting);
    serv.FrameChangeInterval(ScreenImageCaptureRate);
    serv.MouseChangeInterval(MouseCaptureRate);
    serv.MaxConnections(MaxNumConnections);
    serv.EncodeImagesAsGrayScale(SendGrayScaleImages);

    serv.Run(tlsport, PasswordToPrivateKey, PathTo_Private_Key, PathTo_Public_Certficate);
    return 0;
}