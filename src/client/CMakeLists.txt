project(rat_client)


if(WIN32)
    set(${PROJECT_NAME}_PLATFORM_SRC
		windows/ClientWindow.cpp
		windows/resource.h   
		windows/RAT_Client.rc
		../Desktop_Server.ico
		windows/targetver.h
	)
	add_definitions(-DNOMINMAX)
elseif(APPLE)
    set(${PROJECT_NAME}_PLATFORM_SRC
		ios/ClientWindow.cpp
	)

else()
	set(${PROJECT_NAME}_PLATFORM_SRC
		linux/ClientWindow.cpp
	)

endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4100")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

add_definitions(
	-DTEST_CERTIFICATE_PRIVATE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../Test/private.key"
	-DTEST_CERTIFICATE_PUBLIC_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../Test/public.crt"
	-DTEST_CERTIFICATE_PRIVATE_PASSWORD="Test pass"
)
set(${PROJECT_NAME}_EXAMPLE_SRC 
	${CMAKE_CURRENT_SOURCE_DIR}/../../include/cxxopts.hpp	
	ClientWindow.h	
	main.cpp
)

include_directories(${REMOTE_ACCESS_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_EXAMPLE_SRC} ${${PROJECT_NAME}_PLATFORM_SRC})

target_link_libraries(${PROJECT_NAME} ${REMOTE_ACCESS_LIBS})
