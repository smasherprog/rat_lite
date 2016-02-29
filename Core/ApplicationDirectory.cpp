#include "stdafx.h"
#include "ApplicationDirectory.h"
#include <stdio.h>
#include <stdlib.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/predef.h>

std::string executable_path_fallback(const char *argv0)
{
	if (argv0 == nullptr || argv0[0] == 0)
	{
		return "";
	}
	boost::system::error_code ec;
	boost::filesystem::path p(
		boost::filesystem::canonical(
			argv0, boost::filesystem::current_path(), ec));
	return p.make_preferred().string();
}

#if (_WIN32) // {

#  include <Windows.h>

std::string executable_path(const char *argv0)
{
	char buf[1024] = { 0 };
	DWORD ret = GetModuleFileNameA(NULL, buf, 1024);
	if (ret == 0 || ret == sizeof(buf))
	{
		return executable_path_fallback(argv0);
	}
	return buf;
}
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
		// iOS Simulator
#elif TARGET_OS_IPHONE
		// iOS device
#elif TARGET_OS_MAC
#include <mach-o/dyld.h>

std::string executable_path(const char *argv0)
{
	char buf[1024] = { 0 };
	uint32_t size = sizeof(buf);
	int ret = _NSGetExecutablePath(buf, &size);
	if (0 != ret)
	{
		return executable_path_fallback(argv0);
	}
	boost::system::error_code ec;
	boost::filesystem::path p(
		boost::filesystem::canonical(buf, boost::filesystem::current_path(), ec));
	return p.make_preferred().string();
}
#else
#   error "Unknown Apple platform"
#endif

#elif (__linux__) // } {

#  include <unistd.h>

std::string executable_path(const char *argv0)
{
	char buf[1024] = { 0 };
	ssize_t size = readlink("/proc/self/exe", buf, sizeof(buf));
	if (size == 0 || size == sizeof(buf))
	{
		return executable_path_fallback(argv0);
	}
	std::string path(buf, size);
	boost::system::error_code ec;
	boost::filesystem::path p(
		boost::filesystem::canonical(
			path, boost::filesystem::current_path(), ec));
	return p.make_preferred().string();
}

#else // } {

std::string executable_path(const char *argv0)
{
	return executable_path_fallback(argv0);
}

#endif // }
