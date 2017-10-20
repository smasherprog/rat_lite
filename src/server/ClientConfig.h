#pragma once
#include <vector>

namespace SL {
namespace RAT_Server {
    struct ClientConfig {
        bool ShareClip = false;
        int ImageCompressionSetting = 70;

        bool IgnoreIncomingKeyboardEvents = false;
        bool IgnoreIncomingMouseEvents = false;
        bool EncodeImagesAsGrayScale = false;
        std::vector<int> MonitorsToWatch;
    };

} // namespace RAT_Server
} // namespace SL
