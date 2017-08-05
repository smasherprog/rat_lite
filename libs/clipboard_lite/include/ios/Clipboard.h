#pragma once
#include <string>
#include <functional>
#include <thread>
#include <atomic>


namespace SL {
    namespace Clipboard_Lite {
        struct Image;
        class Clipboard_ManagerImpl {
            std::thread BackGroundWorker;
            std::atomic<bool> Copying;
            bool KeepRunning = false;
            int ChangeCount =-1;
        public:

            std::function<void(const std::string& text)> onText;
            std::function<void(const Image& image)> onImage;

            Clipboard_ManagerImpl();
            ~Clipboard_ManagerImpl();

            void run();
            void copy(const std::string& text);
            void copy(const Image& img);
        };
    }
}
