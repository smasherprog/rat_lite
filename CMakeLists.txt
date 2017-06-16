cmake_minimum_required(VERSION 3.8)
project(RAT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(BUILD_SHARED_LIBS OFF)

if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

add_subdirectory(Screen_Capture/src)
add_subdirectory(websocket_lite/src)

add_subdirectory(src)
if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if(BUILD_EXAMPLE)
	add_subdirectory(Client)
	add_subdirectory(Server)
endif()

enable_testing() 
add_subdirectory(Test)
