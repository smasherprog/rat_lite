cmake_minimum_required(VERSION 3.8)
project(remote_access_tool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_EXAMPLE ON)

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
        src/windows
    )
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)
elseif(APPLE)
    set(REMOTE_ACCESS_PLATFORM_INC
        include/ios
		src/ios
    )
else() 
	set(REMOTE_ACCESS_PLATFORM_INC
		include/linux
        src/linux
    )
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

add_definitions(-DBOOST_REGEX_NO_LIB -DBOOST_DATE_TIME_NO_LIB -DASIO_STANDALONE -DBOOST_NO_EXCEPTIONS)
include_directories(include src)

add_library(${PROJECT_NAME} 	
	src/ClientDriver.cpp
	src/ServerDriver.cpp
)

if(BUILD_SHARED_LIBS)
	install (TARGETS ${PROJECT_NAME}  
		LIBRARY DESTINATION bin
	)
else()
	install (TARGETS ${PROJECT_NAME} 
		ARCHIVE DESTINATION lib
	)
endif()

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
