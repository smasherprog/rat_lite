cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(BUILD_SHARED_LIBS OFF)

project(RAT)

option(BUILD_EXAMPLE "Build example" On)

add_subdirectory(Screen_Capture/src)
add_subdirectory(src)
add_subdirectory(libuv)

set(LIBUV_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libuv/include)
set(LIBUV_LIBRARY uv)

add_subdirectory(uwebsockets/src)

if(BUILD_EXAMPLE)
	add_subdirectory(Client)
	add_subdirectory(Server)
endif()