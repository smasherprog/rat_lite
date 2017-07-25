#pragma once
#include "Shapes.h"
#include "Input_Lite.h"

namespace SL {
	namespace RAT {
        enum Press : int {
            NO_PRESS_DATA,
            UP,
            DOWN
        };

        enum Events : int {
            NO_EVENTDATA,
            LEFT,
            MIDDLE,
            RIGHT,
            SCROLL
        };
        struct KeyEvent {
            Press PressData;
            int Key;
            Input_Lite::SpecialKeyCodes SpecialKey;
        };

        struct MouseEvent {
            Events EventData;
            Point Pos;
            int ScrollDelta;
            Press PressData;
        };

		const int PixelStride = 4;
		struct Image {
			Image() {}
			Image(const Rect& r, const unsigned char* d, const size_t& l) : Rect_(r), Data(d), Length(l) {}
			Rect Rect_;
			const unsigned char* Data = nullptr;
			size_t Length = 0;
		};
	}
}