cmake_minimum_required(VERSION 3.10)
project(main)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
	add_link_options(-g -O0)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/CLI11 ${CMAKE_CURRENT_BINARY_DIR}/third_party/CLI11)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/fmt ${CMAKE_CURRENT_BINARY_DIR}/third_party/fmt)

add_executable(main ${APPLICATION_TYPE}
	src/main.cpp
)

target_include_directories(main PUBLIC include src)
target_compile_options(main PRIVATE -Wall -Wextra -Wno-deprecated-declarations)

set_target_properties(main PROPERTIES
		OUTPUT_NAME "marioedit"
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..
		CXX_VISIBILITY_PRESET hidden
		POSITION_INDEPENDENT_CODE ON)

target_link_libraries(main PUBLIC marioedit CLI11::CLI11 fmt)