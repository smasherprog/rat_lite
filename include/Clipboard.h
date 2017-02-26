#pragma once
#include <functional>

namespace SL {
	namespace RAT {
		class ClipboardImpl;
		class Clipboard  {
			ClipboardImpl* ClipboardImpl_ = nullptr;
		public:
	
			Clipboard();
			~Clipboard();

			bool is_ClipboardShared()const;
			void shareClipboard(bool share);
			void onChange(const std::function<void(const char*, int)>& cb);
			void updateClipbard(const char* data, int len);
		};
	}
}