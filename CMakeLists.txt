cmake_minimum_required(VERSION 3.8)
project(rat_lite)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_EXAMPLE ON)
option(BUILD_SHARED_LIBS "Build shared library" OFF) 

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
	${CMAKE_BINARY_DIR}/bin/include
	${OPENSSL_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIRs} 
)
include_directories(
	${COMMON_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} 	
	include/RAT.h
	src/ClientDriver.cpp
	src/ServerDriver.cpp
)
add_dependencies(${PROJECT_NAME} libjpeg-turbo websocket_lite input_lite clipboard_lite screen_capture_lite)
if(WIN32) 
	target_link_libraries(${PROJECT_NAME} Crypt32 Dwmapi)
elseif(APPLE) 	
	find_library(corefoundation_lib CoreFoundation)
	find_library(cocoa_lib Cocoa)
	find_package(Threads REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${corefoundation_lib} ${cocoa_lib})
else()
	find_package(X11 REQUIRED)
	find_package(Threads REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} dl ${X11_LIBRARIES} ${X11_Xfixes_LIB} ${X11_XTest_LIB} ${X11_Xinerama_LIB})
endif()

if(${BUILD_SHARED_LIBS})
	set_target_properties(${PROJECT_NAME} PROPERTIES DEFINE_SYMBOL RAT_LITE_DLL)
endif()
if(WIN32)
	set(LIBEXTENSION .lib)
else()
	if(${BUILD_SHARED_LIBS})
		set(LIBEXTENSION ${CMAKE_SHARED_LIBRARY_SUFFIX})
	else()
		set(LIBEXTENSION ${CMAKE_STATIC_LIBRARY_SUFFIX})
	endif()
endif() 
if(${BUILD_SHARED_LIBS})
	target_link_libraries(${PROJECT_NAME}  
			${CMAKE_BINARY_DIR}/bin/lib/${CMAKE_SHARED_LIBRARY_PREFIX}jpeg${LIBEXTENSION} 
			${CMAKE_BINARY_DIR}/bin/lib/${CMAKE_SHARED_LIBRARY_PREFIX}turbojpeg${LIBEXTENSION} 
			${CMAKE_BINARY_DIR}/bin/lib/${CMAKE_SHARED_LIBRARY_PREFIX}websocket_lite${LIBEXTENSION} 
			${CMAKE_BINARY_DIR}/bin/lib/${CMAKE_SHARED_LIBRARY_PREFIX}input_lite${LIBEXTENSION} 
			${CMAKE_BINARY_DIR}/bin/lib/${CMAKE_SHARED_LIBRARY_PREFIX}clipboard_lite${LIBEXTENSION} 
			${CMAKE_BINARY_DIR}/bin/lib/${CMAKE_SHARED_LIBRARY_PREFIX}screen_capture_lite${LIBEXTENSION} 
		)
endif()

install (TARGETS ${PROJECT_NAME}  
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

install (FILES 
	include/RAT.h  
	include/Logging.h
	DESTINATION include
)

if(BUILD_EXAMPLE)
	add_subdirectory(src/server)
	add_subdirectory(src/client)
endif()

enable_testing() 
add_subdirectory(test)
