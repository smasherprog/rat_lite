#pragma once
#include <memory>
#include <string>

namespace SL {
    namespace RAT {
        struct Client_Config;
    }
    namespace RAT_Client {
        const char WindowTitle[] = "Remote Access Tool";
        class ClientWindowImpl;

        class ClientWindow {
            ClientWindowImpl* ClientWindowImpl_ = nullptr;
        public:
            ClientWindow(std::shared_ptr<SL::RAT::Client_Config>& config, const std::string& host);
            ~ClientWindow();
            void Run();
        };

    }
}
