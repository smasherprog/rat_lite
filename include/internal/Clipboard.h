#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <mutex>
#include <vector>
#include <functional>

namespace SL {
	namespace RAT {

		class Clipboard : public Fl_Widget {
			bool sharedClipboard;
			std::function<void(const char*, int)> ChangeCallback;
			//contention on this mutex is low.. no need to try an optimize 
			std::mutex LastClipDataLock;
			std::vector<char> LastClipData;

		public:
	
			Clipboard();
			virtual ~Clipboard();

			virtual void draw() override {}
			virtual int handle(int event)override;
			bool is_ClipboardShared()const { return sharedClipboard; }
			bool shareClipboard(bool share) { sharedClipboard = share; }
			void onChange(std::function<void(const char*, int)>& cb) { ChangeCallback = cb; }
			void updateClipbard(const char* data, int len);
		};
	}
}