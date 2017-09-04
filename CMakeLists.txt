cmake_minimum_required(VERSION 3.8)
project(WebSocket_Lite)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src) 
add_subdirectory(zlib)
add_subdirectory(libressl)

enable_testing() 
add_subdirectory(Test)