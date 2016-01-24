#pragma once
namespace SL {
	namespace Remote_Access_Library {
		namespace Network {


			namespace Commands {
				enum class PACKET_TYPES : unsigned int {
					INVALID,

					IMAGEDIF,
					MOUSEEVENT,
					KEYEVENT,
					//use LAST_PACKET_TYPE as the starting point of your custom packet types. Everything before this is used internally by the library
					LAST_PACKET_TYPE
				};
			
			}


		}
	}
}