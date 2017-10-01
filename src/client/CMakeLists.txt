project(rat_html)

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
	TARGET rat_html
	POST_BUILD
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMAND tsc
)