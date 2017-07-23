
#include "IClientDriver.h"
#include "IServerDriver.h"
#include "ClientDriver.h"
#include "ServerDriver.h"
#include "Input.h"
#include "SCCommon.h"
#include "Configs.h"

#include "WS_Lite.h"

#include <memory>
#include <thread>
#include <chrono>
#include <assert.h>
#include <chrono>

using namespace std::chrono_literals;



std::vector<std::shared_ptr<SL::Screen_Capture::Monitor>> MonitorsToSend;

class TestClientDriver : public SL::RAT::IClientDriver {
   
 
public:

    std::shared_ptr<SL::RAT::Client_Config> clientconfig;
    std::unique_ptr<SL::RAT::ClientDriver> lowerlevel;

	TestClientDriver() {
        clientconfig = std::make_shared<SL::RAT::Client_Config>();
        clientconfig->HttpTLSPort = 8080;
        clientconfig->WebSocketTLSLPort = 6001;
        clientconfig->Share_Clipboard = true;
        clientconfig->PathTo_Public_Certficate = TEST_CERTIFICATE_PUBLIC_PATH;
        lowerlevel = std::make_unique<SL::RAT::ClientDriver>(this);

	}


	virtual ~TestClientDriver() {}
	virtual void onReceive_Monitors(const SL::Screen_Capture::Monitor* monitors, int num_of_monitors) override {
		SL_RAT_LOG(SL::RAT::Logging_Levels::INFO_log_level, "Received Monitors from Server " << num_of_monitors);
		assert(num_of_monitors == (int)MonitorsToSend.size());
		for (auto i = 0; i < num_of_monitors; i++) {
			assert(MonitorsToSend[i]->Height == monitors[i].Height);
			assert(MonitorsToSend[i]->Id == monitors[i].Id);
			assert(MonitorsToSend[i]->Index == monitors[i].Index);
			assert(MonitorsToSend[i]->Name == std::string(monitors[i].Name));
			assert(MonitorsToSend[i]->OffsetX == monitors[i].OffsetX);
			assert(MonitorsToSend[i]->OffsetY == monitors[i].OffsetY);
			assert(MonitorsToSend[i]->Width == monitors[i].Width);
		}
		SL::RAT::KeyEvent k;
		k.Key = 'a';
		k.PressData = SL::RAT::Press::DOWN;
		k.SpecialKey = SL::RAT::Specials::NO_SPECIAL_DATA;
		lowerlevel->SendKey(k);
	}
	virtual void onReceive_ImageDif(const SL::RAT::Image& img, int monitor_id) {

	}
	virtual void onReceive_MouseImage(const SL::RAT::Image& img) {

	}
	virtual void onReceive_MousePos(const SL::RAT::Point* pos) {

	}
	virtual void onReceive_ClipboardText(const unsigned char* data, unsigned int length) {

	}
    virtual void onConnection(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket) override
    {
    }
    virtual void onMessage(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& msg) override
    {
    }
    virtual void onDisconnection(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) override
    {
    }
};

class TestServerDriver : public SL::RAT::IServerDriver {
    std::shared_ptr<SL::RAT::Server_Config> serverconfig;
    std::unique_ptr<SL::RAT::ServerDriver> lowerlevel;
public:

	bool done = false;

    TestServerDriver() {
        serverconfig = std::make_shared<SL::RAT::Server_Config>();

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
        serverconfig->PasswordToPrivateKey = TEST_CERTIFICATE_PRIVATE_PASSWORD;
        serverconfig->PathTo_Public_Certficate = TEST_CERTIFICATE_PUBLIC_PATH;

        lowerlevel = std::make_unique<SL::RAT::ServerDriver>(this, serverconfig);

		MonitorsToSend.push_back(SL::Screen_Capture::CreateMonitor(2, 4, 1028, 2046, -1, -3, std::string("firstmonitor")));
	}


	virtual ~TestServerDriver() {}


    virtual void onConnection(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket) override
    {
        lowerlevel->SendMonitorInfo(socket, MonitorsToSend);
    }
    virtual void onMessage(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& msg) override
    {
    }
    virtual void onDisconnection(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) override
    {
        done = true;
    }

	virtual void onReceive_Mouse(const SL::RAT::MouseEvent * m) override
	{
	}

	virtual void onReceive_Key(const SL::RAT::KeyEvent * m) override
	{
		SL_RAT_LOG(SL::RAT::Logging_Levels::INFO_log_level, "Received Key event from Client" << m->Key);
		done = true;
	}

	virtual void onReceive_ClipboardText(const unsigned char * data, unsigned int len) override
	{
	}

};




int main(int argc, char* argv[]) {


	TestServerDriver testserver;
	TestClientDriver testclient;
    auto host = "localhost";
    testclient.lowerlevel->Connect(testclient.clientconfig, host);

	while (!testserver.done) {
		std::this_thread::sleep_for(1s);
	}

	return 0;
}