#pragma once
namespace SL {
	namespace Remote_Access_Library {
		namespace Network {


			namespace Commands {
				enum class PACKET_TYPES : unsigned int {
					INVALID,

					IMAGEDIF,
					MOUSEEVENT,
					KEYEVENT

				};
			
			}


		}
	}
}