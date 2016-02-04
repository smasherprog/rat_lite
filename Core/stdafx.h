// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#ifdef _WIN32


#define NOMINMAX 
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#if defined _DEBUG || defined DEBUG
#if _DLL
#pragma comment(lib, "libeay32MDd")
#pragma comment(lib,"ssleay32MDd")
#pragma comment(lib,"turbojpeg")
#else 
#pragma comment(lib, "libeay32MTd")
#pragma comment(lib,"ssleay32MTd")
#pragma comment(lib,"turbojpeg-static")
#endif

#else
#if _DLL
#pragma comment(lib, "libeay32MD")
#pragma comment(lib,"ssleay32MD")
#pragma comment(lib,"turbojpeg")
#else 
#pragma comment(lib, "libeay32MT")
#pragma comment(lib,"ssleay32MT")
#pragma comment(lib,"turbojpeg-static")
#endif
#endif

#endif

#include <stdio.h>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
#include <chrono>
#include <deque>
#include <queue>
#include <functional>
#include <condition_variable>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>

// TODO: reference additional headers your program requires here
