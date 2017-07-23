#include "Clipboard_Lite.h"
#include "Clipboard.h"
#include <memory>
#include <assert.h>
#include <functional>

namespace SL {
    namespace Clipboard_Lite {

        void Clipboard_Manager::copy(const std::string& text) {
            Impl_->copy(text);
        }
        void Clipboard_Manager::copy(const Image& image) {
            Impl_->copy(image);
        }

        Clipboard_Configuration Clipboard_Configuration::onText(const std::function<void(const std::string& text)>& handle) {
            assert(!Impl_->onText);
            Impl_->onText = handle;
            return Clipboard_Configuration(Impl_);
        }
        Clipboard_Configuration Clipboard_Configuration::onImage(const std::function<void(const Image& image)>& handle) {
            assert(!Impl_->onImage);
            Impl_->onImage = handle;
            return Clipboard_Configuration(Impl_);
        }
        Clipboard_Manager Clipboard_Configuration::run()
        {
            Impl_->run();
            return Clipboard_Manager(Impl_);
        }
        Clipboard_Configuration CreateClipboard()
        {
            return Clipboard_Configuration(std::make_shared<Clipboard_ManagerImpl>());
        }
    }
}


