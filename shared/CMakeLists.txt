cmake_minimum_required(VERSION 3.10)
project(shared)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
	add_link_options(-g -O0)
endif()

add_library(shared SHARED
	src/marioedit_library.cpp
)

target_include_directories(shared PUBLIC include src)
target_compile_options(shared PRIVATE -Wall -Wextra -Wno-deprecated-declarations)

set_target_properties(shared PROPERTIES
		OUTPUT_NAME "marioedit"
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..
		CXX_VISIBILITY_PRESET hidden
		POSITION_INDEPENDENT_CODE ON)

target_link_libraries(shared PUBLIC marioedit)