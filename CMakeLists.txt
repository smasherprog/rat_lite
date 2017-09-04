cmake_minimum_required(VERSION 3.8)
project(RAT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(BUILD_SHARED_LIBS OFF)
set(BUILD_EXAMPLE ON)

if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

add_subdirectory(libs/screen_capture_lite/src)
add_subdirectory(libs/websocket_lite/src)
add_subdirectory(libs/clipboard_lite/src)
add_subdirectory(libs/input_lite/src) 
add_subdirectory(src/rat_core) 
add_subdirectory(libs/websocket_lite/zlib)

set(ENABLE_SHARED OFF) 
set(WITH_JAVA OFF) 
add_subdirectory(libs/libjpeg-turbo)

if(BUILD_EXAMPLE)
	add_subdirectory(src/client)
	add_subdirectory(src/server)
endif()


enable_testing() 
add_subdirectory(test)
