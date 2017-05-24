cmake_minimum_required(VERSION 3.8)
project(WebSocket_Lite)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_definitions(-DBOOST_REGEX_NO_LIB -DBOOST_DATE_TIME_NO_LIB -DASIO_STANDALONE -DBOOST_NO_EXCEPTIONS)

add_subdirectory(src)

enable_testing() 
add_subdirectory(Test)