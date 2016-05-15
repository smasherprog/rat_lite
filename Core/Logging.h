#pragma once
#include <string>
#include <iostream>
#include <sstream>
#if __ANDROID__

#define APPNAME "Remote_Access_Library"
#include <android/log.h>

#endif

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

			struct None { };

			template<typename List>
			struct LogData {
				List list;
			};
			typedef std::ostream& (*PfnManipulator)(std::ostream&);
			inline void output(std::ostream& os, None)
			{ }

			template<typename List>
			void Log(Logging_Levels level, const char* file, int line, const char* func, LogData<List>&& data)
			{

#if __ANDROID__

				std::ostringstream buffer;

				buffer << Logging_level_Names[level] << ": FILE: " << file << " Line: " << line << " Func: " << func << " Msg: ";
				output(buffer, std::move(data.list));
				auto msg = buffer.str();
				switch (level) {
				case(Debug_log_level):
					__android_log_print(ANDROID_LOG_DEBUG, APPNAME, "%s", msg.c_str());
					break;
				case(ERROR_log_level):
					__android_log_print(ANDROID_LOG_ERROR, APPNAME, "%s", msg.c_str());
					break;
				case(FATAL_log_level):
					__android_log_print(ANDROID_LOG_FATAL, APPNAME, "%s", msg.c_str());
					break;
				case(INFO_log_level):
					__android_log_print(ANDROID_LOG_INFO, APPNAME, "%s", msg.c_str());
					break;
				case(WARN_log_level):
					__android_log_print(ANDROID_LOG_WARN, APPNAME, "%s", msg.c_str());
					break;
				default:
					__android_log_print(ANDROID_LOG_INFO, APPNAME, "%s", msg.c_str());

				}


#else 
				std::cout << Logging_level_Names[level] << ": FILE: " << file << " Line: " << line << " Func: " << func << " Msg: ";
				output(std::cout, std::move(data.list));
				std::cout << std::endl;

#endif

			}
			template<typename Begin, typename Value>
			constexpr LogData<std::pair<Begin&&, Value&&>> operator<<(LogData<Begin>&& begin, Value&& value) noexcept
			{
				return{ { std::forward<Begin>(begin.list), std::forward<Value>(value) } };
			}

			template<typename Begin, size_t n>
			constexpr LogData<std::pair<Begin&&, const char*>> operator<<(LogData<Begin>&& begin, const char(&value)[n]) noexcept
			{
				return{ { std::forward<Begin>(begin.list), value } };
			}
			template<typename Begin>
			constexpr LogData<std::pair<Begin&&, PfnManipulator>> operator<<(LogData<Begin>&& begin, PfnManipulator value) noexcept
			{
				return{ { std::forward<Begin>(begin.list), value } };
			}

			template <typename Begin, typename Last>
			void output(std::ostream& os, std::pair<Begin, Last>&& data)
			{
				output(os, std::move(data.first));
				os << data.second;
			}





		}
	}
}

#define S(x) #x
#define S_(x) S(x)
//Usage  SL_RAT_LOG(SL::Remote_Access_Library::Utilities::Logging_Levels::Debug_log_level, "Message goes here "<< 56 <<" Any Valid cout stuff works");
#define SL_RAT_LOG(level, msg) Log(level, __FILE__, __LINE__,__func__, SL::Remote_Access_Library::Utilities::LogData<SL::Remote_Access_Library::Utilities::None>() << msg)