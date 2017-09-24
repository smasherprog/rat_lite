#pragma once
#include <string>

namespace SL {
namespace RAT_Client {

    const char WindowTitle[] = "Remote Access Tool";
    class ClientWindowImpl;

    class ClientWindow {
        ClientWindowImpl *ClientWindowImpl_ = nullptr;

      public:
        ClientWindow(std::string host, unsigned short port = 6001, std::string pathtocertificate = "");
        void ShareClipboard(bool share);
        bool ShareClipboard() const;
        ~ClientWindow();
        void Run();
    };

} // namespace RAT_Client
} // namespace SL
