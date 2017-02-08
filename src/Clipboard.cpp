#include "Clipboard.h"
#include "Logging.h"

namespace SL {
	namespace RAT {

		void clip_callback(int source, void *data) {
			auto p = (Clipboard*)data;

			if (source == 1 && p->is_ClipboardShared()) {
				SL_RAT_LOG(Logging_Levels::INFO_log_level, "Clipboard Changed!");
				if (Fl::clipboard_contains(Fl::clipboard_plain_text)) {
					SL_RAT_LOG(Logging_Levels::INFO_log_level, "Contains plain text");
					Fl::paste(*p, 1, Fl::clipboard_plain_text);
				}
			}
		}
		//caller should lock the mutex to prevent races
		bool is_datasame(const char* data, int len, std::vector<char>& lastclipdata) {
			auto size = static_cast<size_t>(len);
			if (size != lastclipdata.size()) return false;
			if (memcmp(lastclipdata.data(), data, size) == 0) return true;
			return false;
		}

		Clipboard::Clipboard() : Fl_Widget(0, 0, 0, 0) {
			sharedClipboard = false;
			Fl::add_clipboard_notify(clip_callback, this);
		}
		Clipboard::~Clipboard() {
			Fl::remove_clipboard_notify(clip_callback);
		}

		int Clipboard::handle(int event) {
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
						emitclipevent = !is_datasame(data, len, LastClipData);
						if (emitclipevent) {
							LastClipData.resize(len);
							memcpy(LastClipData.data(), data, len);
						}
					}
					if (emitclipevent && ChangeCallback) {
						ChangeCallback(data, len);
					}

				}

			}
			return 1;
		}
	
		void Clipboard::updateClipbard(const char* data, int len) {
			bool updateclipboard = false;
			{
				std::lock_guard<std::mutex> lock(LastClipDataLock);
				updateclipboard = !is_datasame(data, len, LastClipData);
				if (updateclipboard) {
					LastClipData.resize(len);
					memcpy(LastClipData.data(), data, len);
				}
			}
			if (updateclipboard)  Fl::copy(data, static_cast<int>(len), 1);
		}
	}
}
