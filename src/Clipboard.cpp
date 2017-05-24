#include "Clipboard.h"
#include "Logging.h"

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>

#include <vector>
#include <mutex>

namespace SL {
	namespace RAT {


		//caller should lock the mutex to prevent races
		bool is_datasame(const char* data, int len, std::vector<char>& lastclipdata) {
			auto size = static_cast<size_t>(len);
			if (size != lastclipdata.size()) return false;
			if (memcmp(lastclipdata.data(), data, size) == 0) return true;
			return false;
		}

		class ClipboardImpl : public Fl_Widget {
			bool sharedClipboard;
			std::function<void(const char*, int)> ChangeCallback;
			//contention on this mutex is low.. no need to try an optimize 
			std::mutex LastClipDataLock;
			std::vector<char> LastClipData;



		public:

			static void clip_callback(int source, void *data) {
#if FLTK_ABI_VERSION >= 10303
				auto p = (ClipboardImpl*)data;

				if (source == 1 && p->is_ClipboardShared()) {
					SL_RAT_LOG(Logging_Levels::INFO_log_level, "Clipboard Changed!");
					if (Fl::clipboard_contains(Fl::clipboard_plain_text)) {
						SL_RAT_LOG(Logging_Levels::INFO_log_level, "Contains plain text");
						Fl::paste(*p, 1, Fl::clipboard_plain_text);
					}
				}
#endif
			}

			ClipboardImpl() : Fl_Widget(0, 0, 0, 0) {
				sharedClipboard = false;
#if FLTK_ABI_VERSION >= 10303
				Fl::add_clipboard_notify(clip_callback, this);
#endif
			}
			virtual ~ClipboardImpl() {
#if FLTK_ABI_VERSION >= 10303
				Fl::remove_clipboard_notify(clip_callback);
#endif
			}



			virtual void draw() override {}
			virtual int handle(int event) override {
#if FLTK_ABI_VERSION >= 10303
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
#endif
				return 1;
			}
			bool is_ClipboardShared()const { return sharedClipboard; }
			void shareClipboard(bool share) { sharedClipboard = share; }
			void onChange(const std::function<void(const char*, int)>& cb) { ChangeCallback = cb; }
			void updateClipbard(const char* data, int len) {
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

		};


		Clipboard::Clipboard() : ClipboardImpl_(new ClipboardImpl())
		{

		}

		Clipboard::~Clipboard()
		{
			delete ClipboardImpl_;
		}

		bool Clipboard::is_ClipboardShared() const
		{
			return ClipboardImpl_->is_ClipboardShared();
		}

		void Clipboard::shareClipboard(bool share)
		{
			ClipboardImpl_->shareClipboard(share);
		}

		void Clipboard::onChange(const std::function<void(const char*, int)>& cb)
		{
			return ClipboardImpl_->onChange(cb);
		}

		void Clipboard::updateClipbard(const char * data, int len)
		{
			return ClipboardImpl_->updateClipbard(data, len);
		}

	}
}
