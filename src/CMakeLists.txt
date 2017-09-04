project(libwebsocket_lite)


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
	
elseif(APPLE)
    set(WEBSOCKET_LITE_PLATFORM_SRC
    )
    set(WEBSOCKET_LITE_PLATFORM_INC
    )

else()
	find_package(Threads REQUIRED)

     set(WEBSOCKET_LITE_PLATFORM_SRC
    )
    set(WEBSOCKET_LITE_PLATFORM_INC
    )
	
endif()

set(WEBSOCKET_LITE_COMMON_SRC 
	${CMAKE_SOURCE_DIR}/include/internal/WebSocketProtocol.h
	${CMAKE_SOURCE_DIR}/include/internal/Utils.h
	${CMAKE_SOURCE_DIR}/src/internal/Utils.cpp
	${CMAKE_SOURCE_DIR}/include/Logging.h
	${CMAKE_SOURCE_DIR}/include/WS_Lite.h
	${CMAKE_SOURCE_DIR}/src/internal/ListenerImpl.cpp
	${CMAKE_SOURCE_DIR}/src/internal/ClientImpl.cpp
	${CMAKE_SOURCE_DIR}/include/internal/DataStructures.h
	${CMAKE_SOURCE_DIR}/src/internal/WSContext.cpp
)

set(WEBSOCKET_LITE_INCLUDE_DIRS  
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/src
	${WEBSOCKET_LITE_PLATFORM_INC} 
)

add_definitions(-DBOOST_REGEX_NO_LIB -DBOOST_DATE_TIME_NO_LIB -DASIO_STANDALONE -DBOOST_NO_EXCEPTIONS)
include_directories(${WEBSOCKET_LITE_INCLUDE_DIRS})
add_library(${PROJECT_NAME} ${WEBSOCKET_LITE_COMMON_SRC} ${WEBSOCKET_LITE_PLATFORM_SRC})

