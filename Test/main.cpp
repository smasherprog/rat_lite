
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
SL::RAT::KeyEvent k = { SL::RAT::Press::DOWN, 'a', SL::RAT::Specials::SHIFT };
SL::RAT::MouseEvent mouseevent = { SL::RAT::Events::MIDDLE, SL::RAT::Point(39, 678), 78, SL::RAT::Press::DOWN };
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

        lowerlevel->SendKeyEvent(k);
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
        lowerlevel->SendMouseEvent(mouseevent);
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
    std::shared_ptr<SL::WS_LITE::IWSocket> Socket;
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
        Socket = socket;
        lowerlevel->SendMonitorsChanged(socket, MonitorsToSend);
    }
    virtual void onMessage(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, const SL::WS_LITE::WSMessage& msg) override
    {
    }
    virtual void onDisconnection(const std::shared_ptr<SL::WS_LITE::IWSocket>& socket, unsigned short code, const std::string& msg) override
    {
        done = true;
    }

    virtual void onMouseEvent(const SL::RAT::MouseEvent& m) override
    {
        done = true;
    }

    virtual void onKeyEvent(const SL::RAT::KeyEvent& kevent) override
    {
        assert(k.Key == kevent.Key);
        assert(k.PressData == kevent.PressData);
        assert(k.SpecialKey == kevent.SpecialKey);
        lowerlevel->SendMousePositionChanged(Socket, mpoint);
    }

    virtual void onClipboardChanged(const std::string& text) override
    {
        assert(text == cliptext);
        lowerlevel->SendClipboardChanged(Socket, text);
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