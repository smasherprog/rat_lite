project(rat_html)

add_library(${PROJECT_NAME} 	
	dummy.cpp
	ClientWindow.ts
	Input_Lite.ts
	Rat_Lite.ts
	index.html
	tsconfig.json
)

add_custom_command(
	TARGET rat_html
	POST_BUILD
	WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMAND tsc
)