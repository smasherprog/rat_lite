#pragma once
#include "Shapes.h"

namespace SL {
	namespace RAT {

        enum Specials : unsigned int {
            NO_SPECIAL_DATA,
            ALT,
            CTRL,
            SHIFT
        };

        enum Press : unsigned int {
            NO_PRESS_DATA,
            UP,
            DOWN,
            DBLCLICK
        };

        enum Events : unsigned int {
            NO_EVENTDATA,
            LEFT,
            MIDDLE,
            RIGHT,
            SCROLL
        };
        struct KeyEvent {
            Press PressData;
            unsigned int Key;
            Specials SpecialKey;
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