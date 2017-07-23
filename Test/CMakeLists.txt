project(Input_Lite_Test)


if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

set(${PROJECT_NAME}_EXAMPLE_SRC 
	main.cpp
)

include_directories(${INPUT_LITE_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_EXAMPLE_SRC} ${${PROJECT_NAME}_PLATFORM_SRC})
target_link_libraries(${PROJECT_NAME} ${INPUT_LITE_LIBS} )

add_test (NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
