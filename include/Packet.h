#pragma once

namespace SL {
	namespace Remote_Access_Library {
		namespace Network {

			enum class PACKET_TYPES : unsigned int {
				INVALID,
				HTTP_MSG,
				SCREENIMAGE,
				SCREENIMAGEDIF,
				MOUSEPOS,
				MOUSEIMAGE,
				KEYEVENT,
				MOUSEEVENT,
				CLIPBOARDTEXTEVENT,
				//use LAST_PACKET_TYPE as the starting point of your custom packet types. Everything before this is used internally by the library
				LAST_PACKET_TYPE
			};
	
		}
	}
}