#pragma once
#include <string>
#include <functional>
#include <thread>
#include <atomic>

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef __cplusplus
}
#endif


namespace SL {
    namespace Clipboard_Lite {
        struct Image;
        class Clipboard_ManagerImpl {
            std::thread BackGroundWorker;
            std::atomic<bool> Copying;


        public:

            std::function<void(const std::string& text)> onText;
            std::function<void(const Image& image)> onImage;

            Clipboard_ManagerImpl();
            ~Clipboard_ManagerImpl();

            void run();
            void copy(const std::string& text);

        };
    }
}
