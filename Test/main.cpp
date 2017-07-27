
#include "IClientDriver.h"
#include "IServerDriver.h"
#include "ClientDriver.h"
#include "ServerDriver.h"
#include "NetworkStructs.h"
#include "SCCommon.h"
#include "Configs.h"

#include "WS_Lite.h"

#include <memory>
#include <thread>
#include <chrono>
#include <assert.h>
#include <chrono>
#include <vector>

using namespace std::chrono_literals;

std::vector<SL::Screen_Capture::Monitor> MonitorsToSend;
SL::RAT::Point mpoint = { 67, 89 };
std::string cliptext = "thisitheweasdsxzzxc436t456u7658u/asd.";


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
    virtual void onMonitorsChanged(const std::vector<SL::Screen_Capture::Monitor>& monitors) override {
        SL_RAT_LOG(SL::RAT::Logging_Levels::INFO_log_level, "Received Monitors from Server " << monitors.size());
        assert(monitors.size() == (int)MonitorsToSend.size());

        for (size_t i = 0; i < monitors.size(); i++) {
            assert(MonitorsToSend[i].Height == monitors[i].Height);
            assert(MonitorsToSend[i].Id == monitors[i].Id);
            assert(MonitorsToSend[i].Index == monitors[i].Index);
            assert(MonitorsToSend[i].Name == std::string(monitors[i].Name));
            assert(MonitorsToSend[i].OffsetX == monitors[i].OffsetX);
            assert(MonitorsToSend[i].OffsetY == monitors[i].OffsetY);
            assert(MonitorsToSend[i].Width == monitors[i].Width);
        }
        //all asci characters
        for (auto k = ' '; k <= '~'; k++) {
            lowerlevel->SendKeyDown(k);
            lowerlevel->SendKeyUp(k);
        }
    }
    virtual void onFrameChanged(const SL::RAT::Image& img, const SL::Screen_Capture::Monitor& monitor) override
    {
    }
    virtual void onNewFrame(const SL::RAT::Image& img, const SL::Screen_Capture::Monitor& monitor)override
    {
    }
    virtual void onMouseImageChanged(const SL::RAT::Image& img)override
    {
    }
    virtual void onMousePositionChanged(const SL::RAT::Point& mevent)override
    {
        assert(mevent == mpoint);
        lowerlevel->SendClipboardChanged(cliptext);
    }
    virtual void onClipboardChanged(const std::string& text)override
    {
        assert(text == cliptext);
        lowerlevel->SendMousePosition(mpoint);
        lowerlevel->SendMouseDown(SL::Input_Lite::MouseButtons::MIDDLE);
        lowerlevel->SendMouseUp(SL::Input_Lite::MouseButtons::RIGHT);
        lowerlevel->SendMouseScroll(46);
        lowerlevel->SendClipboardChanged(cliptext);
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
    char asciikeyreceived = 0;
public:
    std::shared_ptr<SL::WS_LITE::IWSocket> Socket;

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
        Socket = socket;
        lowerlevel->SendMonitorsChanged(socket, MonitorsToSend);
    }
    virtual void onMessage(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& msg) override
    {
    }
    virtual void onDisconnection(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) override
    {

    }
    virtual void onKeyUp(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, char key) override {
        assert(key == asciikeyreceived);
        key++;
        if (key >= '~') {
            lowerlevel->SendClipboardChanged(cliptext);
        }
    }
    virtual void onKeyUp(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, wchar_t key) override {

    }
    virtual void onKeyUp(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, SL::Input_Lite::SpecialKeyCodes key) override {

    }

    virtual void onKeyDown(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, char key) override {
        assert(key == asciikeyreceived);
    }
    virtual void onKeyDown(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, wchar_t key) override {
  
    }
    virtual void onKeyDown(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, SL::Input_Lite::SpecialKeyCodes key) override {
    
    }

    virtual void onMouseUp(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, SL::Input_Lite::MouseButtons button) override {
        assert(SL::Input_Lite::MouseButtons::RIGHT == button);
    }
    virtual void onMouseDown(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, SL::Input_Lite::MouseButtons button) override {
        assert(SL::Input_Lite::MouseButtons::MIDDLE == button);
    }
    virtual void onMouseScroll(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, int offset) override {
        assert(offset == 46);
    }
    virtual void onMousePosition(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::RAT::Point& pos)override {
        assert(pos == mpoint);
    }
    virtual void onClipboardChanged(const std::string& text) override {
        assert(text == cliptext);
    }

};

int main(int argc, char* argv[]) {


    TestServerDriver testserver;
    TestClientDriver testclient;
    auto host = "localhost";
    testclient.lowerlevel->Connect(testclient.clientconfig, host);


    std::this_thread::sleep_for(10s);


    return 0;
}