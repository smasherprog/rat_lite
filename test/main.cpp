
#include "ClientDriver.h"
#include "Configs.h"
#include "IClientDriver.h"
#include "IServerDriver.h"
#include "NetworkStructs.h"
#include "ScreenCapture.h"
#include "ServerDriver.h"
#include "WS_Lite.h"

#include <assert.h>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

std::vector<SL::Screen_Capture::Monitor> MonitorsToSend;
SL::RAT::Point mpoint = {67, 89};
std::string cliptext = "thisitheweasdsxzzxc436t456u7658u/asd.";

class TestClientDriver : public SL::RAT::IClientDriver {
  public:
    std::shared_ptr<SL::RAT::Client_Config> clientconfig;
    std::unique_ptr<SL::RAT::ClientDriver> lowerlevel;

    TestClientDriver()
    {
        clientconfig = std::make_shared<SL::RAT::Client_Config>();
        clientconfig->HttpTLSPort = 8080;
        clientconfig->WebSocketTLSLPort = 6001;
        clientconfig->Share_Clipboard = true;
        clientconfig->PathTo_Public_Certficate = TEST_CERTIFICATE_PUBLIC_PATH;
        lowerlevel = std::make_unique<SL::RAT::ClientDriver>(this);
    }

    virtual ~TestClientDriver() {}
    virtual void onMonitorsChanged(const std::vector<SL::Screen_Capture::Monitor> &monitors) override
    {
        SL_RAT_LOG(SL::RAT::Logging_Levels::INFO_log_level, "Received Monitors from Server " << monitors.size());
        assert(monitors.size() == MonitorsToSend.size());

        for (size_t i = 0; i < monitors.size(); i++) {
            assert(MonitorsToSend[i].Height == monitors[i].Height);
            assert(MonitorsToSend[i].Id == monitors[i].Id);
            assert(MonitorsToSend[i].Index == monitors[i].Index);
            assert(MonitorsToSend[i].Name == std::string(monitors[i].Name));
            assert(MonitorsToSend[i].OffsetX == monitors[i].OffsetX);
            assert(MonitorsToSend[i].OffsetY == monitors[i].OffsetY);
            assert(MonitorsToSend[i].Width == monitors[i].Width);
        }
        std::cout << "Starting Ascii test" << std::endl;
        // all asci characters
        for (auto k = ' '; k <= '~'; k++) {
            // lowerlevel->SendKeyDown(k);
            // lowerlevel->SendKeyUp(k);
        }
    }
    virtual void onFrameChanged(const SL::RAT::Image &img, const SL::Screen_Capture::Monitor &monitor) override {}
    virtual void onNewFrame(const SL::RAT::Image &img, const SL::Screen_Capture::Monitor &monitor) override {}
    virtual void onMouseImageChanged(const SL::RAT::Image &img) override {}
    virtual void onMousePositionChanged(const SL::RAT::Point &mevent) override
    {
        assert(mevent == mpoint);
        lowerlevel->SendClipboardChanged(cliptext);
    }
    virtual void onClipboardChanged(const std::string &text) override
    {
        assert(text == cliptext);
        std::cout << "Sending SendMousePosition test" << std::endl;
        lowerlevel->SendMousePosition(mpoint);
        std::cout << "Sending SendMouseDown test" << std::endl;
        lowerlevel->SendMouseDown(SL::Input_Lite::MouseButtons::MIDDLE);
        std::cout << "Sending SendMouseUp test" << std::endl;
        lowerlevel->SendMouseUp(SL::Input_Lite::MouseButtons::RIGHT);
        std::cout << "Sending SendMouseScroll test" << std::endl;
        lowerlevel->SendMouseScroll(46);
        std::cout << "Sending SendClipboardChanged test" << std::endl;
        lowerlevel->SendClipboardChanged(cliptext);
    }

    virtual void onConnection(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket) override {}
    virtual void onMessage(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const SL::WS_LITE::WSMessage &msg) override {}
    virtual void onDisconnection(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, unsigned short code, const std::string &msg) override {}
};

class TestServerDriver : public SL::RAT::IServerDriver {
    std::shared_ptr<SL::RAT::Server_Config> serverconfig;
    std::unique_ptr<SL::RAT::ServerDriver> lowerlevel;
    char asciikeyreceived = ' ';

  public:
    std::shared_ptr<SL::WS_LITE::IWSocket> Socket;

    TestServerDriver()
    {
        serverconfig = std::make_shared<SL::RAT::Server_Config>();

        serverconfig->WebSocketTLSLPort = 6001; // listen for websockets
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

        //  MonitorsToSend.push_back(SL::Screen_Capture::CreateMonitor(2, 4, 1028, 2046, -1, -3, std::string("firstmonitor")));
    }

    virtual ~TestServerDriver() {}

    virtual void onConnection(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket) override
    {
        Socket = socket;
        // lowerlevel->SendMonitorsChanged(socket, MonitorsToSend);
    }
    virtual void onMessage(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const SL::WS_LITE::WSMessage &msg) override {}
    virtual void onDisconnection(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, unsigned short code, const std::string &msg) override {}
    virtual void onKeyUp(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, SL::Input_Lite::KeyCodes key) override
    {
        asciikeyreceived++;
        if (key >= '~') {
            // lowerlevel->SendClipboardChanged(cliptext);
        }
    }
    virtual void onKeyDown(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, SL::Input_Lite::KeyCodes key) override
    {
        std::cout << "Received Ascii test successfully" << std::endl;
    }

    virtual void onMouseUp(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, SL::Input_Lite::MouseButtons button) override
    {
        assert(SL::Input_Lite::MouseButtons::RIGHT == button);
        std::cout << "Received onMouseUp successfully" << std::endl;
    }
    virtual void onMouseDown(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, SL::Input_Lite::MouseButtons button) override
    {
        assert(SL::Input_Lite::MouseButtons::MIDDLE == button);
        std::cout << "Received onMouseDown successfully" << std::endl;
    }
    virtual void onMouseScroll(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, int offset) override
    {
        assert(offset == 46);
        std::cout << "Received onMouseScroll successfully" << std::endl;
    }
    virtual void onMousePosition(const std::shared_ptr<SL::WS_LITE::IWSocket> &socket, const SL::RAT::Point &pos) override
    {
        assert(pos == mpoint);
        std::cout << "Received onMousePosition successfully" << std::endl;
    }
    virtual void onClipboardChanged(const std::string &text) override
    {
        assert(text == cliptext);
        std::cout << "Received onClipboardChanged successfully" << std::endl;
    }
};

int main(int argc, char *argv[])
{
    TestServerDriver testserver;
    TestClientDriver testclient;
    auto host = "127.0.0.1";
    testclient.lowerlevel->Connect(testclient.clientconfig, host);

    std::this_thread::sleep_for(10s);

    return 0;
}