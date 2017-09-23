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

include_directories(${COMMON_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(${PROJECT_NAME} 
	ClientWindow.h	
	main.cpp
	${${PROJECT_NAME}_PLATFORM_SRC}
)
if(${BUILD_SHARED_LIBS})
	if(WIN32) 
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
					   COMMAND ${CMAKE_COMMAND} -E copy_if_different
					   $<TARGET_FILE:rat>              
					   $<TARGET_FILE_DIR:${PROJECT_NAME}>)
				   
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
					   COMMAND ${CMAKE_COMMAND} -E copy_if_different
					   $<TARGET_FILE:${COMMON_LIBRARIES}>              
					   $<TARGET_FILE_DIR:${PROJECT_NAME}>)
	endif()
endif()
target_link_libraries(${PROJECT_NAME} rat ${COMMON_LIBRARIES})
