#include "Clipboard.h"
#include "Logging.h"

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>

#include <mutex>
#include <vector>

namespace SL {
	namespace Remote_Access_Library {
		namespace Capturing {
			class ClipboardImpl : public Fl_Widget {
			public:
				const bool* Is_ClipboardShared;
				std::function<void(const char*, int)> OnChange;
				//contention on this mutex is low.. no need to try an optimize 
				std::mutex LastClipDataLock;
				std::vector<char> LastClipData;
				ClipboardImpl(const bool* is_clipshared, std::function<void(const char*, int)>&& onchange) : Fl_Widget(0, 0, 0, 0), Is_ClipboardShared(is_clipshared), OnChange(std::move(onchange)) {
					Fl::add_clipboard_notify(clip_callback, this);
				}
				virtual ~ClipboardImpl() {
					Fl::remove_clipboard_notify(clip_callback);
				}
				virtual void draw() override {}//do nothing
				virtual int handle(int event) override {
					if (event == FL_PASTE) {

						if (strcmp(Fl::event_clipboard_type(), Fl::clipboard_image) == 0) { // an image is being pasted
							//To Be implemeneted...... Probably not though because images can be large and seems like its an edge case any way
						}
						else {//text is being pasted
							bool emitclipevent = false;
							auto data = Fl::event_text();
							auto len = Fl::event_length();
							{
								std::lock_guard<std::mutex> lock(LastClipDataLock);
								emitclipevent = !is_datasame(data, len);
								if (emitclipevent) UpdateBuffer(data, len);
							}
							if (emitclipevent) {
								OnChange(data, len);
							}
							
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
				//caller should lock the mutex to prevent races
				bool is_datasame(const char* data, int len) {
					auto size = static_cast<size_t>(len);
					if (size != LastClipData.size()) return false;
					if (memcmp(LastClipData.data(), data, size) == 0) return true;
					return false;
				}
				//caller should lock the mutex to prevent races
				void UpdateBuffer(const char* data, int len) {
					auto size = static_cast<size_t>(len);
					LastClipData.resize(size);
					memcpy(LastClipData.data(), data, size);
				}
				void copy_to_clipboard(const char* data, int len) {
					bool updateclipboard = false;
					{
						std::lock_guard<std::mutex> lock(LastClipDataLock);
						updateclipboard = !is_datasame(data, len);
						if (updateclipboard) UpdateBuffer(data, len);
					}
					if (updateclipboard)  Fl::copy(data, static_cast<int>(len), 1);
				}
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
void SL::Remote_Access_Library::Capturing::Clipboard::copy_to_clipboard(const char* data, int len) {
	_ClipboardImpl->copy_to_clipboard(data, len);
}