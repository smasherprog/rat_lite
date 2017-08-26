project(libinput_lite)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

if(WIN32)
    set(INPUT_LITE_PLATFORM_SRC     
		${CMAKE_CURRENT_SOURCE_DIR}/../src/windows/Input.cpp
    )

    set(INPUT_LITE_PLATFORM_INC     
       ${CMAKE_CURRENT_SOURCE_DIR}/../include/windows
       ${CMAKE_CURRENT_SOURCE_DIR}/../src/windows
    )

elseif(APPLE)
	find_library(corefoundation_lib CoreFoundation)
	find_library(cocoa_lib Cocoa)
	set(INPUT_LITE_PLATFORM_LIBS ${corefoundation_lib} ${cocoa_lib})

    set(INPUT_LITE_PLATFORM_SRC
		${CMAKE_CURRENT_SOURCE_DIR}/../src/ios/Input.cpp
    )
    set(INPUT_LITE_PLATFORM_INC
       ${CMAKE_CURRENT_SOURCE_DIR}/../include/ios
       ${CMAKE_CURRENT_SOURCE_DIR}/../src/ios
    )
else()
	set(INPUT_LITE_PLATFORM_SRC
		${CMAKE_CURRENT_SOURCE_DIR}/../src/linux/Input.cpp
    )
  
	find_package(X11 REQUIRED)
	if(!X11_XTest_FOUND)
 		message(FATAL_ERROR "X11 extensions are required, but not found!")
	endif()
	if(!X11_Xfixes_LIB)
 		message(FATAL_ERROR "X11 fixes extension is required, but not found!")
	endif()
	set(INPUT_LITE_PLATFORM_INC
		${CMAKE_CURRENT_SOURCE_DIR}/../include/linux
		${CMAKE_CURRENT_SOURCE_DIR}/../src/linux
		${X11_INCLUDE_DIR}
    )
	find_package(Threads REQUIRED)
	set(INPUT_LITE_PLATFORM_LIBS
		${X11_LIBRARIES}
		${X11_Xfixes_LIB}
		${X11_XTest_LIB}
		${CMAKE_THREAD_LIBS_INIT}
	)
	
endif()

set(INPUT_LITE_COMMON_SRC 
	${CMAKE_CURRENT_SOURCE_DIR}/../include/internal/InputManager.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/internal/InputManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../include/Input_Lite.h
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Input_Lite.cpp
)
set(INPUT_LITE_LIBS
	${PROJECT_NAME}
	CACHE INTERNAL "${PROJECT_NAME}: Include Libraries" FORCE
)

set(INPUT_LITE_INCLUDE_DIRS 
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	${CMAKE_CURRENT_SOURCE_DIR}/../src
	${CMAKE_CURRENT_SOURCE_DIR}/../include/internal
	${CMAKE_CURRENT_SOURCE_DIR}/../src/internal
	${INPUT_LITE_PLATFORM_INC}
	CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE
 )

include_directories(${INPUT_LITE_INCLUDE_DIRS})
add_library(${PROJECT_NAME} ${INPUT_LITE_COMMON_SRC} ${INPUT_LITE_PLATFORM_SRC})

target_link_libraries(${PROJECT_NAME} ${INPUT_LITE_PLATFORM_LIBS})


