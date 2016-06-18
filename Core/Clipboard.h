#pragma once
#include <functional>


namespace SL {
	namespace Remote_Access_Library {
		namespace Capturing {
			class ClipboardImpl;
			class Clipboard {
				ClipboardImpl* _ClipboardImpl = nullptr;
			public:
				//user must guarantee that is_clipshared is valid for the lifetime of this clipboard object!
				Clipboard(const bool* is_clipshared, std::function<void(const char*, int)>&& onchange);
				~Clipboard();
                void copy_to_clipboard(const char* data, int len);
			};
		}
	}
}