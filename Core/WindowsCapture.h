#pragma once
#include <memory>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			class Image_Wrapper;
		}
		namespace WindowsCapturing {
			
			std::shared_ptr<Utilities::Image_Wrapper> CaptureDesktop();
		}
	}
};
