
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "IClientDriver.h"
#include "IServerDriver.h"
#include "ClientDriver.h"
#include "ServerDriver.h"

#include <memory>
#include "Configs.h"
#include <thread>
#include <chrono>


class TestClientDriver : public SL::RAT::IClientDriver {
public:


	TestClientDriver()  {

	}


	virtual ~TestClientDriver() {}
	virtual void onReceive_Monitors(const SL::Screen_Capture::Monitor* monitors, int num_of_monitors) override {

	}
	virtual void onReceive_ImageDif(const SL::RAT::Image& img, int monitor_id) {

	}
	virtual void onReceive_MouseImage(const SL::RAT::Image& img) {

	}
	virtual void onReceive_MousePos(const SL::RAT::Point* pos) {

	}
	virtual void onReceive_ClipboardText(const char* data, unsigned int length) {

	}

	virtual void onConnection(const std::shared_ptr<SL::RAT::IWebSocket>& socket) override
	{
	}
	virtual void onMessage(const SL::RAT::IWebSocket & socket, const char * data, size_t length) override
	{
	}
	virtual void onDisconnection(const SL::RAT::IWebSocket & socket, int code, char * message, size_t length) override
	{
	}
};

class TestServerDriver : public SL::RAT::IServerDriver {
public:


	TestServerDriver() {

	}


	virtual ~TestServerDriver() {}




	// Inherited via IServerDriver
	virtual void onConnection(const std::shared_ptr<SL::RAT::IWebSocket>& socket) override
	{
	}

	virtual void onMessage(const SL::RAT::IWebSocket & socket, const char * data, size_t length) override
	{
	}

	virtual void onDisconnection(const SL::RAT::IWebSocket & socket, int code, char * message, size_t length) override
	{
	}

	virtual void onReceive_Mouse(const SL::RAT::MouseEvent * m) override
	{
	}

	virtual void onReceive_Key(const SL::RAT::KeyEvent * m) override
	{
	}

	virtual void onReceive_ClipboardText(const char * data, unsigned int len) override
	{
	}

};




TEST_CASE("Testing Client Driver", "[TestClientDriver]") {

	SL::RAT::ServerDriver server;

	auto serverconfig = std::make_shared<SL::RAT::Server_Config>();

	serverconfig->WebSocketTLSLPort = 6001;// listen for websockets
	serverconfig->HttpTLSPort = 8080;
	serverconfig->Share_Clipboard = true;

	serverconfig->ImageCompressionSetting = 70;
	serverconfig->MousePositionCaptureRate = 50;
	serverconfig->ScreenImageCaptureRate = 100;
	serverconfig->SendGrayScaleImages = false;
	serverconfig->MaxNumConnections = 2;
	serverconfig->MaxWebSocketThreads = 2;
	serverconfig->PathTo_Private_Key = TEST_CERTIFICATE_PRIVATE_PATH;
	serverconfig->PasswordToPrivateKey=TEST_CERTIFICATE_PRIVATE_PASSWORD;
	serverconfig->PathTo_Public_Certficate = TEST_CERTIFICATE_PUBLIC_PATH;
	TestServerDriver testserver;

	server.Start(&testserver, serverconfig);

	auto clientconfig = std::make_shared<SL::RAT::Client_Config>();
	clientconfig->HttpTLSPort=8080;
	auto host = "localhost";
	clientconfig->WebSocketTLSLPort=6001;
	clientconfig->Share_Clipboard=true;
	clientconfig->PathTo_Public_Certficate=TEST_CERTIFICATE_PUBLIC_PATH;


	TestClientDriver testclient;
	SL::RAT::ClientDriver client(&testclient);
	client.Connect(clientconfig, host);




	auto keeprunningserver = true;
	while (keeprunningserver) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

}