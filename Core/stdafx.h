// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WIN32


#define NOMINMAX 
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>

#elif __APPLE__

#elif __ANDROID__

#elif __linux__

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <sys/shm.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/XShm.h>


#endif




#include <future>
#include <chrono>
#include <fstream>
#include <sys/stat.h>

#define UNUSED(x) (void)(x)

template<typename R>
bool is_ready(std::future<R> const& f)
{
	return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}
namespace SL {
	inline bool File_Exists(const std::string& name) {
		std::ifstream f(name.c_str());
		return f.good();
	}
	inline bool Directory_Exists(const std::string& path)
	{
		struct stat info;
		if (stat(path.c_str(), &info) != 0)
			return false;
		else if (info.st_mode & S_IFDIR)
			return true;
		else
			return false;
	}
}
