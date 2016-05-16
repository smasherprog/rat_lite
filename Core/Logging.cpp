#include "stdafx.h"
#include "Logging.h"
#include <iostream>

#if __ANDROID__

#define APPNAME "Remote_Access_Library"
#include <android/log.h>

#endif

void SL::Remote_Access_Library::Utilities::Log(Logging_Levels level, const char* file, int line, const char* func, std::ostringstream& data)
{

#if __ANDROID__

	std::ostringstream buffer;
	buffer << Logging_level_Names[level] << ": FILE: " << file << " Line: " << line << " Func: " << func << " Msg: " << data.str();
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
	std::cout << Logging_level_Names[level] << ": FILE: " << file << " Line: " << line << " Func: " << func << " Msg: " << data.str() << std::endl;

#endif

			}

#ifdef BOOST_NO_EXCEPTIONS
//must provide custom definition to catch the exceptions thrown
namespace boost {
	void throw_exception(std::exception const & e)
	{
		SL_RAT_LOG(e.what(), SL::Remote_Access_Library::Utilities::Logging_Levels::ERROR_log_level);

	}
}
#endif