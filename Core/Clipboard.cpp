#include "stdafx.h"
#include "Clipboard.h"
#include "Logging.h"

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
namespace SL {
	namespace Remote_Access_Library {
		namespace Capturing {
			class ClipboardImpl : public Fl_Widget {
			public:
				const bool* Is_ClipboardShared;
				std::function<void(const char*, int)> OnChange;

				ClipboardImpl(const bool* is_clipshared, std::function<void(const char*, int)>&& onchange): Fl_Widget(0, 0, 0, 0), Is_ClipboardShared(is_clipshared), OnChange(std::move(onchange)){
					Fl::add_clipboard_notify(clip_callback, this);
				}
				virtual ~ClipboardImpl() {
					Fl::remove_clipboard_notify(clip_callback);
				}
				virtual int handle(int event) override {
					if (event == FL_PASTE) {
						if (strcmp(Fl::event_clipboard_type(), Fl::clipboard_image) == 0) { // an image is being pasted
							//To Be implemeneted...... Probably not though because images can be large and seems like its an edge case any way
						}
						else {//text is being pasted
							OnChange(Fl::event_text(), Fl::event_length());
						}
					}
					return 1;
				}
				static void clip_callback(int source, void *data) {
					auto p = (ClipboardImpl*)data;

					if (source == 1 && *(p->Is_ClipboardShared)) {
						SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "Clipboard Changed!");
						if (Fl::clipboard_contains(Fl::clipboard_plain_text)) {
							SL_RAT_LOG(Utilities::Logging_Levels::INFO_log_level, "Contains plain text");
							Fl::paste(*p, 1, Fl::clipboard_plain_text);
						}
					}
				}
				virtual void draw() override {}//do nothing
			};
		}
	}
}
SL::Remote_Access_Library::Capturing::Clipboard::Clipboard(const bool* is_clipshared, std::function<void(const char*, int)>&& onchange)
{
	_ClipboardImpl = new ClipboardImpl(is_clipshared, std::forward<std::function<void(const char*, int)>>(onchange));
}

SL::Remote_Access_Library::Capturing::Clipboard::~Clipboard()
{
	delete _ClipboardImpl;
}
void SL::Remote_Access_Library::Capturing::Clipboard::copy_to_clipboard(const char* data, int len){
    Fl::copy(data, static_cast<int>(len), 1);
}