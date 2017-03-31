project(RAT_Client)


if(WIN32)
    set(${PROJECT_NAME}_PLATFORM_SRC
		${CMAKE_CURRENT_SOURCE_DIR}/windows/resource.h
		${CMAKE_CURRENT_SOURCE_DIR}/windows/RAT_Client.rc
		${CMAKE_CURRENT_SOURCE_DIR}/../Desktop_Server.ico
	)
elseif(APPLE)


else()
    

endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4100 /wd4458")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

add_definitions(
	-DTEST_CERTIFICATE_PUBLIC_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../Test_Certs/public.crt"
)


set(${PROJECT_NAME}_EXAMPLE_SRC 
	main.cpp
	ConnectWindow.h
	ViewerController.h
	ImageControl.h
	ConnectWindow.cpp
	ViewerController.cpp
	ImageControl.cpp
)
include_directories(${REMOTE_ACCESS_INCLUDE_DIRS} uWebSockets/src)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_EXAMPLE_SRC} ${${PROJECT_NAME}_PLATFORM_SRC})

target_link_libraries(${PROJECT_NAME} ${REMOTE_ACCESS_LIBS})
