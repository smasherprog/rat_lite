#include "stdafx.h"
#include "Logging.h"






#if __ANDROID__
#include <android/log.h>
#define APPNAME "Remote_Access_Library"

void SL::Remote_Access_Library::Utilities::Log(std::string str, Logging_Levels level, std::string file, std::string line, std::string func)
{
	auto msg = Logging_level_Names[level] + ":";
	msg += " File: " + file;
	msg += " Line: " + line;
	msg += " Func: " + func;
	msg += " Msg: " + str;
	switch (level) {
	case(Debug_log_level):
		__android_log_print(ANDROID_LOG_DEBUG, APPNAME,"%s", msg.c_str());
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
}
#else 
#include <iostream>
void SL::Remote_Access_Library::Utilities::Log(std::string str, Logging_Levels level, std::string file, std::string line, std::string func)
{
	auto msg = Logging_level_Names[level] + ":";
	msg += " File: " + file;
	msg += " Line: " + line;
	msg += " Func: " + func;
	msg += " Msg: " + str;
	std::cout << msg << std::endl;
}
#endif

#ifdef BOOST_NO_EXCEPTIONS
//must provide custom definition to catch the exceptions thrown
namespace boost {
	void throw_exception(std::exception const & e)
	{
		SL_RAT_LOG(e.what(), SL::Remote_Access_Library::Utilities::Logging_Levels::ERROR_log_level);
		
	}
}
#endif