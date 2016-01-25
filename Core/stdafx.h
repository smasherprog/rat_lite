// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#ifdef _WIN32
#define NOMINMAX 
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>


#endif

#include <boost/asio.hpp>
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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "wx/platform.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


// TODO: reference additional headers your program requires here
