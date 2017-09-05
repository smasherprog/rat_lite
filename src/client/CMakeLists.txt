project(rat_client)


if(WIN32)
    set(${PROJECT_NAME}_PLATFORM_SRC
		windows/ClientWindow.cpp
		windows/resource.h   
		windows/RAT_Client.rc
		../Desktop_Server.ico
		windows/targetver.h
	)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)
elseif(APPLE)
    set(${PROJECT_NAME}_PLATFORM_SRC
		ios/ClientWindow.cpp
	)

else()
	set(${PROJECT_NAME}_PLATFORM_SRC
		linux/ClientWindow.cpp
	)
endif()

add_definitions(
	-DTEST_CERTIFICATE_PRIVATE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../Test/private.key"
	-DTEST_CERTIFICATE_PUBLIC_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../Test/public.crt"
	-DTEST_CERTIFICATE_PRIVATE_PASSWORD="Test pass"
)

include_directories(${REMOTE_ACCESS_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${${PROJECT_NAME}_PLATFORM_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} 
	ClientWindow.h	
	main.cpp
	${${PROJECT_NAME}_PLATFORM_SRC}
)

target_link_libraries(${PROJECT_NAME} ${REMOTE_ACCESS_LIBS} ${${PROJECT_NAME}_PLATFORM_LIBS})
