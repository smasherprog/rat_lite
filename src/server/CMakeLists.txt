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
	-DTEST_CERTIFICATE_PRIVATE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../../Test/private.key"
	-DTEST_CERTIFICATE_PUBLIC_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../../Test/public.crt"
	-DTEST_CERTIFICATE_PRIVATE_PASSWORD="Test pass"
)
find_package(ZLIB REQUIRED) 
find_package(OpenSSL REQUIRED) 
find_package(LIBJPEGTURBO REQUIRED)  

include_directories(
	${COMMON_INCLUDE_DIRS} 
	${CMAKE_CURRENT_SOURCE_DIR}
	${LIBJPEGTURBO_INCLUDE_DIRS}
)
add_executable(${PROJECT_NAME} 	
	Server.h
	main.cpp
	Server.cpp
	ServerFunctions.h
	ServerFunctions.cpp
	Client.h
	${${PROJECT_NAME}_PLATFORM_SRC}
)

if(WIN32) 
	target_link_libraries(${PROJECT_NAME} Crypt32 Dwmapi)
elseif(APPLE) 	
	find_library(corefoundation_lib CoreFoundation)
	find_library(cocoa_lib Cocoa)
	find_package(Threads REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${corefoundation_lib} ${cocoa_lib})
else()
	find_package(X11 REQUIRED)
	find_package(Threads REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} dl ${X11_LIBRARIES} ${X11_Xfixes_LIB} ${X11_XTest_LIB} ${X11_Xinerama_LIB})
endif()

if(WIN32)
	set(LIBEXTENSION .lib)
else()
	if(${BUILD_SHARED_LIBS})
		set(LIBEXTENSION ${CMAKE_SHARED_LIBRARY_SUFFIX})
	else()
		set(LIBEXTENSION ${CMAKE_STATIC_LIBRARY_SUFFIX})
	endif()
endif() 

target_link_libraries(${PROJECT_NAME} 
	rat_lite
	${CMAKE_BINARY_DIR}/bin/lib/${CMAKE_SHARED_LIBRARY_PREFIX}websocket_lite${LIBEXTENSION}    
	${CMAKE_BINARY_DIR}/bin/lib/${CMAKE_SHARED_LIBRARY_PREFIX}input_lite${LIBEXTENSION}    
	${CMAKE_BINARY_DIR}/bin/lib/${CMAKE_SHARED_LIBRARY_PREFIX}clipboard_lite${LIBEXTENSION}    
	${CMAKE_BINARY_DIR}/bin/lib/${CMAKE_SHARED_LIBRARY_PREFIX}screen_capture_lite${LIBEXTENSION}
	${OPENSSL_LIBRARIES} 
	${ZLIB_LIBRARIES}
	${LIBJPEGTURBO_LIBRARIES}    
)

if(${BUILD_SHARED_LIBS}) 
	set_target_properties(${PROJECT_NAME} PROPERTIES DEFINE_SYMBOL RAT_LITE_DLL)  
	set(librarylocation bin)
	if(APPLE)
		set(librarylocation lib)
	endif()
	
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_BINARY_DIR}/bin/${librarylocation}/${CMAKE_SHARED_LIBRARY_PREFIX}websocket_lite${CMAKE_SHARED_LIBRARY_SUFFIX}             
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_BINARY_DIR}/bin/${librarylocation}/${CMAKE_SHARED_LIBRARY_PREFIX}input_lite${CMAKE_SHARED_LIBRARY_SUFFIX}          
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_BINARY_DIR}/bin/${librarylocation}/${CMAKE_SHARED_LIBRARY_PREFIX}clipboard_lite${CMAKE_SHARED_LIBRARY_SUFFIX}             
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_BINARY_DIR}/bin/${librarylocation}/${CMAKE_SHARED_LIBRARY_PREFIX}screen_capture_lite${CMAKE_SHARED_LIBRARY_SUFFIX}            
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:rat_lite>              
		 $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif() 
