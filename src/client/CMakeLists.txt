project(rat_client)

add_library(${PROJECT_NAME} 	
	dummy.cpp
)
add_custom_command(
	TARGET rat_client
	POST_BUILD
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND npm install
) 
