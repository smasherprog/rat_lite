#pragma once
#include <string>
#include <sstream>

namespace SL {
	namespace Remote_Access_Library {
		namespace Utilities {
			enum Logging_Levels {
				Debug_log_level,
				ERROR_log_level,
				FATAL_log_level,
				INFO_log_level,
				WARN_log_level
			};
			const std::string Logging_level_Names[] = { "DEBUG", "ERROR", "FATAL", "INFO", "WARN" };
			void Log(Logging_Levels level, const char* file, int line, const char* func, std::ostringstream& data);
		}
	}
}
#define UNUSED(x) (void)(x)
#define S(x) #x
#define S_(x) S(x)
//Usage  SL_RAT_LOG(SL::Remote_Access_Library::Utilities::Logging_Levels::Debug_log_level, "Message goes here "<< 56 <<" Any Valid cout stuff works");

#define SL_RAT_LOG(level, msg) {\
std::ostringstream buffer; \
buffer << msg; \
SL::Remote_Access_Library::Utilities::Log(level, __FILE__, __LINE__, __func__, buffer);\
}

