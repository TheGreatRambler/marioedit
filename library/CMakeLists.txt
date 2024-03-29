cmake_minimum_required(VERSION 3.10)
project(library)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
	add_link_options(-g -O0)
endif()

add_library(library SHARED
	src/marioedit_library.cpp
)

if(APPLE)
	set(CMAKE_MACOSX_RPATH 1)
	set(CMAKE_INSTALL_RPATH ".")
endif()

target_include_directories(library PUBLIC include src)
target_compile_options(library PRIVATE -Wall -Wextra -Wno-deprecated-declarations)

set_target_properties(library PROPERTIES
		OUTPUT_NAME "marioedit"
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..
		CXX_VISIBILITY_PRESET hidden
		POSITION_INDEPENDENT_CODE ON)

target_link_libraries(library PUBLIC marioedit)