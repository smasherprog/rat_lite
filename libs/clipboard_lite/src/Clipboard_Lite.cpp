#include "Clipboard.h"
#include "Clipboard_Lite.h"
#include <assert.h>
#include <functional>
#include <memory>

namespace SL {
namespace Clipboard_Lite {

    class Clipboard_Configuration : public IClipboard_Configuration {
        std::shared_ptr<Clipboard_ManagerImpl> Impl_;

      public:
        Clipboard_Configuration(const std::shared_ptr<Clipboard_ManagerImpl> &impl) : Impl_(impl) {}

        std::shared_ptr<IClipboard_Configuration> onText(const std::function<void(const std::string &text)> &handle)
        {
            assert(!Impl_->onText);
            Impl_->onText = handle;
            return std::make_shared<Clipboard_Configuration>(Impl_);
        }
        std::shared_ptr<IClipboard_Configuration> onImage(const std::function<void(const Image &image)> &handle)
        {
            assert(!Impl_->onImage);
            Impl_->onImage = handle;
            return std::make_shared<Clipboard_Configuration>(Impl_);
        }
        std::shared_ptr<Clipboard_Manager> run()
        {
            Impl_->run();
            return std::make_shared<Clipboard_Manager>(Impl_);
        }
    };

    std::shared_ptr<IClipboard_Configuration> CreateClipboard()
    {
        return std::make_shared<Clipboard_Configuration>(std::make_shared<Clipboard_ManagerImpl>());
    }
    void Clipboard_Manager::copy(const std::string &text) { Impl_->copy(text); }
    void Clipboard_Manager::copy(const Image &img) { Impl_->copy(img); }
} // namespace Clipboard_Lite
} // namespace SL
