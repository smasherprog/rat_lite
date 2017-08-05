#include "ClientWindow.h"
#include "Configs.h"

namespace SL {
    namespace RAT_Client {
        class ClientWindowImpl {
            std::shared_ptr<SL::RAT::Client_Config> Config;
        public:

            ClientWindowImpl(std::shared_ptr<SL::RAT::Client_Config>& config, const std::string& host) :Config(config) {
              
            }
            void Run() {

            }


        };


        ClientWindow::ClientWindow(std::shared_ptr<SL::RAT::Client_Config>& config, const std::string& host) {
            ClientWindowImpl_ = new ClientWindowImpl(config, host);
        }
        ClientWindow::~ClientWindow() {
            delete ClientWindowImpl_;
        }

        void ClientWindow::Run() {
            ClientWindowImpl_->Run();
        }
    }
}
