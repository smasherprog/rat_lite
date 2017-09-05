project(rat_server)


if(WIN32)
	set(${PROJECT_NAME}_PLATFORM_SRC
		windows/resource.h
		windows/RAT_Server.rc
		../Desktop_Server.ico
	)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)
elseif(APPLE)

else()
 
endif()

add_definitions(
	-DTEST_CERTIFICATE_PRIVATE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../Test/private.key"
	-DTEST_CERTIFICATE_PUBLIC_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../Test/public.crt"
	-DTEST_CERTIFICATE_PRIVATE_PASSWORD="Test pass"
)

include_directories(${REMOTE_ACCESS_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} 	
	main.cpp
	Server.cpp
	${${PROJECT_NAME}_PLATFORM_SRC}
)
target_link_libraries(${PROJECT_NAME} ${REMOTE_ACCESS_LIBS})
