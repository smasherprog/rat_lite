cmake_minimum_required(VERSION 3.8)
project(rat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_EXAMPLE ON)
option(BUILD_SHARED_LIBS "Build the shared library" OFF)
option(CUSTOM_OPENSSL "Build the library using a custom openssl version" OFF)
option(CUSTOM_ZLIB "Build the library using a custom zlib version" OFF)

set(GLOBAL_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/install)

# Sets global output directory for single configuration (GCC)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GLOBAL_OUTPUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GLOBAL_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GLOBAL_OUTPUT_PATH})

# Sets global output directory for sub-configurations (msvc, mingw)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${GLOBAL_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${GLOBAL_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${GLOBAL_OUTPUT_PATH})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

include(CMakeLists-ExternalProjects.txt)

if(WIN32)
	set(REMOTE_ACCESS_PLATFORM_INC     
        include/windows
    )
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)
elseif(APPLE)
    set(REMOTE_ACCESS_PLATFORM_INC
        include/ios
    )
else() 
	set(REMOTE_ACCESS_PLATFORM_INC
		include/linux
    )
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

add_definitions(-DBOOST_REGEX_NO_LIB -DBOOST_DATE_TIME_NO_LIB -DASIO_STANDALONE -DBOOST_NO_EXCEPTIONS)

set(COMMON_INCLUDE_DIRS
	include
	${ZLIB_INCLUDE_DIRS} 
	${OPENSSL_INCLUDE_DIRS}
	${LIBJPEG_TURBO_INCLUDE_DIRS}
	${INPUT_LITE_INCLUDE_DIRS}
	${CLIPBOARD_LITE_INCLUDE_DIRS} 
	${SCREEN_CAPTURE_LITE_INCLUDE_DIRS}
	${WEBSOCKET_LITE_INCLUDE_DIRS}	
)
include_directories(
	${COMMON_INCLUDE_DIRS}
)
add_library(${PROJECT_NAME} 	
	src/ClientDriver.cpp
	src/ServerDriver.cpp
)

set(COMMON_LIBRARIES 
	${LIBJPEG_TURBO_LIBRARIES} 
	${WEBSOCKET_LITE_LIBRARIES} 
	${INPUT_LITE_LIBRARIES}
	${CLIPBOARD_LITE_LIBRARIES}	
	${SCREEN_CAPTURE_LITE_LIBRARIES}
	${OPENSSL_LIBRARIES} 
	${ZLIB_LIBRARIES} 
)

if(BUILD_SHARED_LIBS)
	set_target_properties(${PROJECT_NAME} PROPERTIES DEFINE_SYMBOL RAT_LITE_DLL)
	if(WIN32) 
		target_link_libraries(${PROJECT_NAME} ${COMMON_LIBRARIES} Crypt32)
	elseif(APPLE) 
		find_package(Threads REQUIRED)
		target_link_libraries(${PROJECT_NAME} ${COMMON_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
	else()
		find_package(Threads REQUIRED)
		target_link_libraries(${PROJECT_NAME} ${COMMON_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} dl)
	endif()
endif()

if(NOT CUSTOM_OPENSSL)
	add_dependencies(${PROJECT_NAME} openssl)
endif()
if(NOT CUSTOM_ZLIB)
	add_dependencies(${PROJECT_NAME} zlib)
endif()
add_dependencies(${PROJECT_NAME} libjpeg-turbo websocket_lite input_lite clipboard_lite screen_capture_lite)

install (TARGETS ${PROJECT_NAME}  
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

install (FILES 
	include/IClientDriver.h
	include/ClientDriver.h
	include/ServerDriver.h
	include/IServerDriver.h
	include/NetworkStructs.h
	include/Configs.h
	include/INetworkHandlers.h
	include/Logging.h	
	include/Shapes.h
	DESTINATION include
)

if(BUILD_EXAMPLE)
	add_subdirectory(src/client)
	add_subdirectory(src/server)
endif()

enable_testing() 
add_subdirectory(test)
