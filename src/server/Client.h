#pragma once
#include <memory>
#include <vector>

namespace SL {
namespace WS_LITE {
    class IWSocket;
}
namespace RAT_Server {
    struct Client {
        bool ShareClip = false;
        int ImageCompressionSetting = 70;

        bool IgnoreIncomingKeyboardEvents = false;
        bool IgnoreIncomingMouseEvents = false;
        bool EncodeImagesAsGrayScale = false;
        std::vector<int> MonitorsToWatch;
        std::vector<int> MonitorsNeeded;

        std::shared_ptr<WS_LITE::IWSocket> Socket;
    };

} // namespace RAT_Server
} // namespace SL
