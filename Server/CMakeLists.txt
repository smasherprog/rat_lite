project(RAT_Server)


if(WIN32)
	set(${PROJECT_NAME}_PLATFORM_SRC
		${CMAKE_CURRENT_SOURCE_DIR}/windows/resource.h
		${CMAKE_CURRENT_SOURCE_DIR}/windows/RAT_Server.rc
		${CMAKE_CURRENT_SOURCE_DIR}/../Desktop_Server.ico
	)
elseif(APPLE)


else()
    find_package(Threads REQUIRED)
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4100")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

set(${PROJECT_NAME}_EXAMPLE_SRC 
	main.cpp
	Server.h
	Server.cpp
)

add_definitions(
	-DTEST_CERTIFICATE_PRIVATE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../Test_Certs/private.key"
	-DTEST_CERTIFICATE_PUBLIC_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../Test_Certs/public.crt"
	-DTEST_CERTIFICATE_PRIVATE_PASSWORD="Test pass"
)

include_directories(${REMOTE_ACCESS_INCLUDE_DIRS} uWebSockets/include)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_EXAMPLE_SRC} ${${PROJECT_NAME}_PLATFORM_SRC})
target_link_libraries(${PROJECT_NAME} ${REMOTE_ACCESS_LIBS} ${CMAKE_THREAD_LIBS_INIT})
