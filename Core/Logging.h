#pragma once
#include <string>

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
			const std::string Logging_level_Names[] = { "DEBUG", "ERROR", "FATAL", "INFO", "WARN"};
			void Log(std::string str, Logging_Levels level, std::string file, std::string line, std::string func);
#define S(x) #x
#define S_(x) S(x)

#define SL_RAT_LOG(str, level) Log(str, level, S_(__FILE__), S_(__LINE__), S_(__func__))
		}
	}
}