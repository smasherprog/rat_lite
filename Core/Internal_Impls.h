#pragma once
#include "BufferManager.h"

namespace SL {
	namespace Remote_Access_Library {
		//this namespace should only be accessed by the Core library and NEVER by a consumer of this library
		namespace INTERNAL {
			extern Utilities::BufferManager _Buffer;
		}
	}
}