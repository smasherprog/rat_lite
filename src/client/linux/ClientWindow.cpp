#include "ClientWindow.h"
#include "Configs.h"
#include <gtk/gtk.h>


namespace SL
{
namespace RAT_Client
{
    class ClientWindowImpl
    {
        std::shared_ptr<SL::RAT::Client_Config> Config;

    public:
        ClientWindowImpl(std::shared_ptr<SL::RAT::Client_Config>& config, const std::string& host)
            : Config(config)
        {
        }
        void Run()
        {
            while(1) {
               
            }
        }
    };

    ClientWindow::ClientWindow(std::shared_ptr<SL::RAT::Client_Config>& config, const std::string& host)
    {
        ClientWindowImpl_ = new ClientWindowImpl(config, host);
    }
    ClientWindow::~ClientWindow()
    {
        delete ClientWindowImpl_;
    }

    void ClientWindow::Run()
    {
        ClientWindowImpl_->Run();
    }
}
}
