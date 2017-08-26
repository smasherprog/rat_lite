#pragma once
#include "Input_Lite.h"
#include <memory>
#include <string>

namespace SL {
namespace RAT {

    class Point;
    class IClientDriver;
    class ClientDriverImpl;
    struct Client_Config;
    class ClientDriver {
        std::shared_ptr<ClientDriverImpl> ClientDriverImpl_;

      public:
        ClientDriver(IClientDriver *r);
        ~ClientDriver();

        void Connect(std::shared_ptr<Client_Config> config, const char *dst_host);

        void SendKeyUp(SL::Input_Lite::KeyCodes key);
        void SendKeyDown(SL::Input_Lite::KeyCodes key);

        void SendMouseUp(const Input_Lite::MouseButtons button);
        void SendMouseDown(const Input_Lite::MouseButtons button);

        void SendMouseScroll(int offset);
        void SendMousePosition(const Point &pos);

        void SendClipboardChanged(const std::string &text);
    };
} // namespace RAT

} // namespace SL
