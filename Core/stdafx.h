// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WIN32

#include <FL/Fl.H>
#define NOMINMAX 
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>

#endif

#define UNUSED(x) (void)(x)
#define ASIO_HAS_BOOST_DATE_TIME
#define ASIO_DISABLE_THREAD_KEYWORD_EXTENSION