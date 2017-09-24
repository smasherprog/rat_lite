#include "ClientWindow.h"


namespace SL {
    namespace RAT_Client {
        class ClientWindowImpl {
        public:

            ClientWindowImpl(const std::string& host) 
              
            }
            void Run() {

            }


        };


        ClientWindow::ClientWindow(std::string host, unsigned short port, std::string pathtocertificate)
    {
        ClientWindowImpl_ = new ClientWindowImpl(host);
    }
    ClientWindow::~ClientWindow()
    {
        delete ClientWindowImpl_;
    }
    void ClientWindow::ShareClipboard(bool share)
    {
    }
    bool ClientWindow::ShareClipboard() const
    {
        return false;
    }
    void ClientWindow::Run()
    {
        ClientWindowImpl_->Run();
    }
    }
}
