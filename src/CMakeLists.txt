project(libRemote_Access)

if(WIN32)
    set(REMOTE_ACCESS_PLATFORM_SRC     

    )

	set(REMOTE_ACCESS_PLATFORM_LIBS    
	
    )
	set(REMOTE_ACCESS_PLATFORM_INC     
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/windows
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/windows
		
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

	${CMAKE_CURRENT_SOURCE_DIR}/../src/ClientDriver.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Clipboard.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Input.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../src/ServerDriver.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/../include/IClientDriver.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/ClientDriver.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/Input.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/ServerDriver.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/IServerDriver.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/Clipboard.h

	${CMAKE_CURRENT_SOURCE_DIR}/../include/internal/ServerHub.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/internal/ServerHub.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/../include/Image.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/Configs.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/INetworkHandlers.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/Logging.h
	${CMAKE_CURRENT_SOURCE_DIR}/../include/Shapes.h
)
set(REMOTE_ACCESS_COMMON_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
	${CMAKE_CURRENT_SOURCE_DIR}/../websocket_lite/include
)

find_package(Boost COMPONENTS program_options system date_time REQUIRED)
find_package(libjpegturbo REQUIRED)
set(FLTK_SKIP_FLUID TRUE)
find_package(FLTK REQUIRED)
find_package(OpenSSL REQUIRED)

set(REMOTE_ACCESS_INCLUDE_DIRS 
	${REMOTE_ACCESS_COMMON_INC} 
	${REMOTE_ACCESS_PLATFORM_INC} 
	${OPENSSL_INCLUDE_DIR}
	${LIBJPEGTURBO_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${FLTK_INCLUDE_DIR}
	${SCREEN_CAPTURE_INCLUDE_DIRS}
	CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE
)

set(REMOTE_ACCESS_LIBS
	${PROJECT_NAME}
	${REMOTE_ACCESS_PLATFORM_LIBS} 
	${OPENSSL_LIBRARIES} 
	${LIBJPEGTURBO_LIBRARIES}
	${FLTK_LIBRARIES}
	${SCREEN_CAPTURE_LIB}
	${Boost_LIBRARIES}
	CACHE INTERNAL "${PROJECT_NAME}: Include Libraries" FORCE
)

include_directories(${REMOTE_ACCESS_INCLUDE_DIRS})
add_library(${PROJECT_NAME} ${REMOTE_ACCESS_COMMON_SRC} ${REMOTE_ACCESS_PLATFORM_SRC})

target_link_libraries(${REMOTE_ACCESS_LIBS})

