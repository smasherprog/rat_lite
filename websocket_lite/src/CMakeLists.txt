project(libWebSocket_Lite)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

if(WIN32)
    set(WEBSOCKET_LITE_PLATFORM_SRC     
	
    )
    set(WEBSOCKET_LITE_PLATFORM_INC     
   
    )
	set(WEBSOCKET_LITE_PLATFORM_LIBS
		Crypt32
    )

elseif(APPLE)
    set(WEBSOCKET_LITE_PLATFORM_SRC
    )
    set(WEBSOCKET_LITE_PLATFORM_INC
    )
	set(WEBSOCKET_LITE_PLATFORM_LIBS

    )
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl/*)
else()
	find_package(Threads REQUIRED)

     set(WEBSOCKET_LITE_PLATFORM_SRC
    )
    set(WEBSOCKET_LITE_PLATFORM_INC
    )
	set(WEBSOCKET_LITE_PLATFORM_LIBS
		${CMAKE_THREAD_LIBS_INIT}
    )
	
endif()

set(WEBSOCKET_LITE_COMMON_SRC 
	${CMAKE_CURRENT_SOURCE_DIR}/../include/internal/WebSocketProtocol.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/internal/Utils.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/internal/Utils.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../include/Logging.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/WS_Lite.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/internal/ListenerImpl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/internal/ClientImpl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../include/internal/DataStructures.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/internal/WSContext.cpp
)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

set(WEBSOCKET_LITE_INCLUDE_DIRS 
	${OPENSSL_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/../asio/asio/include
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	${CMAKE_CURRENT_SOURCE_DIR}/../src
	${WEBSOCKET_LITE_PLATFORM_INC}
	CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE
 )

set(WEBSOCKET_LITE_LIBS
	${PROJECT_NAME}
	${OPENSSL_LIBRARIES} 
	${ZLIB_LIBRARIES}
	${WEBSOCKET_LITE_PLATFORM_LIBS}
	CACHE INTERNAL "${PROJECT_NAME}: Include Libraries" FORCE
)
include_directories(${WEBSOCKET_LITE_INCLUDE_DIRS})
add_library(${PROJECT_NAME} ${WEBSOCKET_LITE_COMMON_SRC} ${WEBSOCKET_LITE_PLATFORM_SRC})

target_link_libraries(${PROJECT_NAME} ${WEBSOCKET_LITE_PLATFORM_LIBS})


