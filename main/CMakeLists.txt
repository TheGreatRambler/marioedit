cmake_minimum_required(VERSION 3.10)
project(main)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
	add_link_options(-g -O0)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/CLI11 ${CMAKE_CURRENT_BINARY_DIR}/third_party/CLI11)

# Libcurl for downloads
set(CURL_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/curl/lib/libcurl.lib)
set(CURL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/curl/include)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/../third_party/curl/lib/libcurl.dll
		${CMAKE_CURRENT_BINARY_DIR}/libcurl.dll)

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
	target_link_libraries(main PUBLIC marioedit CLI11::CLI11 ${CURL_LIB} SDL3-static gdi32)
elseif(ANDROID)
	target_link_libraries(main PUBLIC marioedit CLI11::CLI11 ${CURL_LIB} GLESv3 EGL)
elseif(APPLE)
	target_link_libraries(main PUBLIC marioedit CLI11::CLI11 ${CURL_LIB} "-framework OpenGl" "-framework CoreFoundation" "-framework CoreGraphics" "-framework CoreText" "-framework CoreServices" "-framework Cocoa" "-framework Metal" "-framework Foundation" "-framework QuartzCore" glfw)
endif()
target_include_directories(main PUBLIC include ${SDL_INCLUDES})

target_compile_options(main PRIVATE -Wall -Wextra -Wno-deprecated-declarations)

set_target_properties(main PROPERTIES
		OUTPUT_NAME "marioedit"
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		CXX_VISIBILITY_PRESET hidden
		POSITION_INDEPENDENT_CODE ON)