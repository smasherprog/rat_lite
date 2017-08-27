#pragma once
#include <functional>
#include <memory>

namespace SL {
namespace Clipboard_Lite {
    // image data will be in r, g, b format   or r, g, b, a format
    struct Image {
        std::shared_ptr<unsigned char> Data;
        int Height = 0;
        int Width = 0;
        int PixelStride = 0;
    };
    class Clipboard_ManagerImpl;
    class Clipboard_Manager {
        std::shared_ptr<Clipboard_ManagerImpl> Impl_;

      public:
        Clipboard_Manager(const std::shared_ptr<Clipboard_ManagerImpl> &impl) : Impl_(impl) {}
        // copy text into the clipboard
        void copy(const std::string &text);
        // copy an image into the clipboard... image data will be in r, g, b format   or r, g, b, a format
        void copy(const Image &img);
    };
    class IClipboard_Configuration {
      public:
        virtual ~IClipboard_Configuration() {}
        virtual std::shared_ptr<IClipboard_Configuration> onText(const std::function<void(const std::string &text)> &handle) = 0;
        virtual std::shared_ptr<IClipboard_Configuration> onImage(const std::function<void(const Image &image)> &handle) = 0;
        virtual std::shared_ptr<Clipboard_Manager> run() = 0;
    };

    std::shared_ptr<IClipboard_Configuration> CreateClipboard();
} // namespace Clipboard_Lite
} // namespace SL