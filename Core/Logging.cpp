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
		__android_log_print(ANDROID_LOG_DEBUG, APPNAME, msg.c_str());
		break;
	case(ERROR_log_level):
		__android_log_print(ANDROID_LOG_ERROR, APPNAME, msg.c_str());
		break;
	case(FATAL_log_level):
		__android_log_print(ANDROID_LOG_FATAL, APPNAME, msg.c_str());
		break;
	case(INFO_log_level):
		__android_log_print(ANDROID_LOG_INFO, APPNAME, msg.c_str());
		break;
	case(WARN_log_level):
		__android_log_print(ANDROID_LOG_WARN, APPNAME, msg.c_str());
		break;
	default:
		__android_log_print(ANDROID_LOG_INFO, APPNAME, msg.c_str());

	}
}
#else 
void SL::Remote_Access_Library::Utilities::Log(std::string str, Logging_Levels level, std::string file, std::string line, std::string func)
{
	auto msg = Logging_level_Names[level] + ":";
	msg += " File: " + file;
	msg += " Line: " + line;
	msg += " Func: " + func;
	msg += " Msg: " + str;

}
#endif

