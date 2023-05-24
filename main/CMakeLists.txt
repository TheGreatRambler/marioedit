cmake_minimum_required(VERSION 3.10)
project(main)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
	add_link_options(-g -O0)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/CLI11 ${CMAKE_CURRENT_BINARY_DIR}/third_party/CLI11)

# SDL2 for the desktop runtime
if(NOT ANDROID)
	set(SDL_STATIC ON CACHE INTERNAL "" FORCE)
	set(SDL_INCLUDES
		${CMAKE_CURRENT_SOURCE_DIR}/../third_party/SDL/include
		${CMAKE_CURRENT_SOURCE_DIR}/../third_party/SDL/src
	)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/SDL ${CMAKE_CURRENT_BINARY_DIR}/third_party/SDL)
endif()

# For OpenGL
if(APPLE)
	set(GLFW_LIBRARY_TYPE "STATIC")
	find_package(glfw3 3.3 REQUIRED)
endif()

add_executable(main ${APPLICATION_TYPE}
	src/main.cpp
	src/ui.cpp
	src/viewer.cpp
)

if(WIN32)
	target_link_libraries(main PUBLIC marioedit CLI11::CLI11 SDL3-static gdi32 opengl32)
elseif(ANDROID)
	target_link_libraries(main PUBLIC marioedit CLI11::CLI11 GLESv3 EGL)
elseif(APPLE)
	target_link_libraries(main PUBLIC marioedit CLI11::CLI11 "-framework OpenGl" "-framework CoreFoundation" "-framework CoreGraphics" "-framework CoreText" "-framework CoreServices" "-framework Cocoa" "-framework Metal" "-framework Foundation" "-framework QuartzCore" glfw)
endif()
target_include_directories(main PUBLIC include ${SDL_INCLUDES})

target_compile_options(main PRIVATE -Wall -Wextra -Wno-deprecated-declarations)

set_target_properties(main PROPERTIES
		OUTPUT_NAME "marioedit"
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		CXX_VISIBILITY_PRESET hidden
		POSITION_INDEPENDENT_CODE ON)