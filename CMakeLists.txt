cmake_minimum_required(VERSION 3.8)
project(rat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_EXAMPLE ON)
option(BUILD_SHARED_LIBS "Build the shared library" ON) 
 
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

find_package(ZLIB REQUIRED) 
find_package(OpenSSL REQUIRED) 
set(COMMON_INCLUDE_DIRS
	include
	${PROJECT_BINARY_DIR}/bin/include
	${OPENSSL_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIRs} 
)
include_directories(
	${COMMON_INCLUDE_DIRS}
)
link_directories(${PROJECT_BINARY_DIR}/bin/lib)
add_library(${PROJECT_NAME} 	
	include/RAT.h
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

add_dependencies(${PROJECT_NAME} libjpeg-turbo websocket_lite input_lite clipboard_lite screen_capture_lite)

install (TARGETS ${PROJECT_NAME}  
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

install (FILES 
	include/IClientDriver.h 
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
