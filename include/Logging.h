#pragma once
#include <iostream>
#include <sstream>
#include <string>

#if __ANDROID__

#define APPNAME "RAT"
#include <android/log.h>

#endif

namespace SL {
namespace RAT_Lite {

    enum Logging_Levels { Debug_log_level, ERROR_log_level, FATAL_log_level, INFO_log_level, WARN_log_level };
    const std::string Logging_level_Names[] = {"DEBUG", "ERROR", "FATAL", "INFO", "WARN"};
    inline void Log(Logging_Levels level, const char *file, int line, const char *func, std::ostringstream &data)
    {
#if __ANDROID__

        std::ostringstream buffer;
        buffer << Logging_level_Names[level] << ": FILE: " << file << " Line: " << line << " Func: " << func << " Msg: " << data.str();
        auto msg = buffer.str();
        switch (level) {
        case (Debug_log_level):
            __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "%s", msg.c_str());
            break;
        case (ERROR_log_level):
            __android_log_print(ANDROID_LOG_ERROR, APPNAME, "%s", msg.c_str());
            break;
        case (FATAL_log_level):
            __android_log_print(ANDROID_LOG_FATAL, APPNAME, "%s", msg.c_str());
            break;
        case (INFO_log_level):
            __android_log_print(ANDROID_LOG_INFO, APPNAME, "%s", msg.c_str());
            break;
        case (WARN_log_level):
            __android_log_print(ANDROID_LOG_WARN, APPNAME, "%s", msg.c_str());
            break;
        default:
            __android_log_print(ANDROID_LOG_INFO, APPNAME, "%s", msg.c_str());
        }
#else
        std::cout << Logging_level_Names[level] << ": FILE: " << file << " Line: " << line << " Func: " << func << " Msg: " << data.str()
                  << std::endl;

#endif
    }

} // namespace RAT_Lite
} // namespace SL
#define UNUSED(x) (void)(x)
#define S(x) #x
#define S_(x) S(x)
// Usage  SL_RAT_LOG(SL::RAT::Logging_Levels::Debug_log_level, "Message goes here "<< 56 <<" Any Valid cout stuff works");

#define SL_RAT_LOG(level, msg)                                                                                                                       \
    {                                                                                                                                                \
        \
std::ostringstream buffersl134nonesd;                                                                                                                \
        \
buffersl134nonesd                                                                                                                                           \
            << msg;                                                                                                                                  \
        \
SL::RAT_Lite::Log(level, __FILE__, __LINE__, __func__, buffersl134nonesd);                                                                           \
    \
}
