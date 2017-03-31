project(libRemote_Access)


if(WIN32)
    set(REMOTE_ACCESS_PLATFORM_SRC     

    )

	find_package(LibUV REQUIRED)
	set(REMOTE_ACCESS_PLATFORM_LIBS    
		${LIBUV_LIBRARIES}
    )
	set(REMOTE_ACCESS_PLATFORM_INC     
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/windows
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/windows
		${LIBUV_INCLUDE_DIRS}
    )
elseif(APPLE)

    set(REMOTE_ACCESS_PLATFORM_SRC
  
    )
    set(REMOTE_ACCESS_PLATFORM_INC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/ios
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/ios
    )
else()
	set(REMOTE_ACCESS_PLATFORM_SRC
	
	)
  
	set(REMOTE_ACCESS_PLATFORM_INC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/linux
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/linux
		${X11_INCLUDE_DIR}
    )
	find_package(Threads REQUIRED)
	set(REMOTE_ACCESS_PLATFORM_LIBS
		${CMAKE_THREAD_LIBS_INIT}
	)
	
endif()

set(REMOTE_ACCESS_COMMON_SRC 	

	${CMAKE_CURRENT_SOURCE_DIR}/../src/ClientNetworkDriver.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Clipboard.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Input.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/ServerNetworkDriver.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/../include/IClientDriver.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/ClientNetworkDriver.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/Input.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/ServerNetworkDriver.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/IServerDriver.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/Clipboard.h

	${CMAKE_CURRENT_SOURCE_DIR}/../include/Image.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/Configs.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/IWebSocket.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/internal/WebSocket.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/INetworkHandlers.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/Logging.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/Shapes.h
)
set(REMOTE_ACCESS_COMMON_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

find_package(Boost COMPONENTS program_options system date_time REQUIRED)
find_package(libjpegturbo REQUIRED)
set(FLTK_SKIP_FLUID TRUE)
find_package(FLTK REQUIRED)
find_package(OpenSSL REQUIRED)

set(REMOTE_ACCESS_INCLUDE_DIRS 
	${REMOTE_ACCESS_COMMON_INC} 
	${REMOTE_ACCESS_PLATFORM_INC} 
	uWS
	${OPENSSL_INCLUDE_DIR}
	${LIBJPEGTURBO_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${FLTK_INCLUDE_DIR}
	${SCREEN_CAPTURE_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/../uwebsockets/src/
	CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE
)

set(REMOTE_ACCESS_LIBS
	${PROJECT_NAME}
	${REMOTE_ACCESS_PLATFORM_LIBS} 
	${OPENSSL_LIBRARIES} 
	${LIBUWS_LIBRARY} 
	${LIBJPEGTURBO_LIBRARIES}
	${FLTK_LIBRARIES}
	${SCREEN_CAPTURE_LIB}
	${Boost_LIBRARIES}
	${CMAKE_CURRENT_SOURCE_DIR}/uwebsockets/src
	CACHE INTERNAL "${PROJECT_NAME}: Include Libraries" FORCE
)

include_directories(${REMOTE_ACCESS_INCLUDE_DIRS})
add_library(${PROJECT_NAME} ${REMOTE_ACCESS_COMMON_SRC} ${REMOTE_ACCESS_PLATFORM_SRC})

target_link_libraries(${REMOTE_ACCESS_LIBS})

