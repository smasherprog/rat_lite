project(rat_client)

add_library(${PROJECT_NAME} 	
	dummy.cpp
	rat_lite.ts
	electron-main.ts
	input_lite.ts
	rat_client.ts
	index.html 
	tsconfig.json
	package.json
)
add_custom_command(
	TARGET rat_client
	POST_BUILD
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND npm install
) 
add_custom_command(
	TARGET rat_client
	POST_BUILD
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND tsc
)
