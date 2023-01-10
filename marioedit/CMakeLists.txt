cmake_minimum_required(VERSION 3.10)
project(marioedit)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wno-sign-compare)
add_compile_options(-Wno-deprecated-declarations)
add_compile_options(-Wno-unused-parameter)
add_compile_options(-Wno-extern-initializer)
add_compile_options(-Wno-deprecated-enum-enum-conversion)

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
	add_link_options(-g -O0)
endif()

if(ANDROID)
	# Define Android architecture variables
	# Used for building in skia and determining build locations
	if(ANDROID_ABI STREQUAL "armeabi-v7a")
		set(ANDROID_ARCH "arm")
	elseif(ANDROID_ABI STREQUAL "arm64-v8a")
		set(ANDROID_ARCH "arm64")
	elseif(ANDROID_ABI STREQUAL "x86")
		set(ANDROID_ARCH "x86")
	elseif(ANDROID_ABI STREQUAL "x86_64")
		set(ANDROID_ARCH "x64")
	endif()
endif()

# ZXing for QR codes
set(BUILD_EXAMPLES OFF)
set(BUILD_BLACKBOX_TESTS OFF)
set(BUILD_UNIT_TESTS OFF)
set(BUILD_PYTHON_MODULE OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/zxing ${CMAKE_CURRENT_BINARY_DIR}/third_party/zxing)

# Skia for rendering QR codes and programs
set(SKIA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/skia)
set(SKIA_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/skia)
execute_process(
	COMMAND python tools/git-sync-deps
	WORKING_DIRECTORY ${SKIA_DIR})
if(ANDROID)
	execute_process(
		COMMAND bin/gn gen ${SKIA_BUILD_DIR} "--args=is_debug=false ndk=\"${ANDROID_NDK}\" target_cpu=\"${ANDROID_ARCH}\" is_component_build=true skia_use_system_libjpeg_turbo=false skia_use_system_libwebp=false skia_use_system_expat=false skia_use_system_zlib=false skia_use_system_libpng=false skia_use_system_harfbuzz=false skia_use_system_icu=false skia_use_system_icu=false"
		WORKING_DIRECTORY ${SKIA_DIR})
	add_library(skia SHARED IMPORTED)
	set_property(TARGET skia PROPERTY
		IMPORTED_LOCATION ${SKIA_BUILD_DIR}/libskia.so)
elseif(WIN32)
	execute_process(
		COMMAND bin/gn gen ${SKIA_BUILD_DIR} "--args=is_official_build=true clang_win=\"C:/Program Files/LLVM\" extra_cflags=[\"/MDd\"] skia_use_system_libjpeg_turbo=false skia_use_system_libwebp=false skia_use_system_expat=false skia_use_system_zlib=false skia_use_system_libpng=false skia_use_system_harfbuzz=false skia_use_system_icu=false"
		WORKING_DIRECTORY ${SKIA_DIR})
	add_library(skia STATIC IMPORTED)
	set_property(TARGET skia PROPERTY
		IMPORTED_LOCATION ${SKIA_BUILD_DIR}/skia.lib)
else()
	# MacOS
endif()
execute_process(COMMAND ninja WORKING_DIRECTORY ${SKIA_BUILD_DIR})

# fmt for some formatting
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/fmt ${CMAKE_CURRENT_BINARY_DIR}/third_party/fmt)

# Zlib for decompressing replays
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/zlib ${CMAKE_CURRENT_BINARY_DIR}/third_party/zlib)
# Replace .res.res files with .res, windows thing
execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_BINARY_DIR}/third_party/zlib/CMakeFiles/zlib.dir/win32/zlib1.res.res
		${CMAKE_CURRENT_BINARY_DIR}/third_party/zlib/CMakeFiles/zlib.dir/win32/zlib1.res)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/../third_party/zlib/zlib.h
		${CMAKE_CURRENT_BINARY_DIR}/third_party/zlib/zlib.h)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_BINARY_DIR}/third_party/zlib/zlib.dll
		${CMAKE_CURRENT_BINARY_DIR}/zlib.dll)

# Libcurl for downloading Miis for visualization
set(CURL_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/curl/lib/libcurl.lib)
set(CURL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/curl/include)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/../third_party/curl/lib/libcurl.dll
		${CMAKE_CURRENT_BINARY_DIR}/libcurl.dll)

# wasmtime for an alternative better Webassembly runtime
set(WASMTIME_INCLUDES 
	${CMAKE_CURRENT_SOURCE_DIR}/../third_party/wasmtime/crates/c-api/include
	${CMAKE_CURRENT_SOURCE_DIR}/../third_party/wasmtime/crates/c-api/wasm-c-api/include
)
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/wasmtime/crates/c-api ${CMAKE_CURRENT_BINARY_DIR}/third_party/wasmtime)

# SDL2 for the desktop runtime
if(NOT ANDROID)
	set(SDL_STATIC ON CACHE INTERNAL "" FORCE)
	set(SDL_INCLUDES
		${CMAKE_CURRENT_SOURCE_DIR}/../third_party/SDL/include
		${CMAKE_CURRENT_SOURCE_DIR}/../third_party/SDL/src
	)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/SDL ${CMAKE_CURRENT_BINARY_DIR}/third_party/SDL)
endif()

set(marioedit_SOURCES
	src/viewer.cpp
	src/parser.cpp
	src/drawer.cpp
	src/ui.cpp
	src/SMM2CourseDecryptor/aes.cpp
	src/SMM2CourseDecryptor/decrypt.cpp
	src/MM2/level.cpp
	src/kaitai/kaitaistream.cpp
)

add_library(marioedit STATIC ${marioedit_SOURCES})
set_target_properties(marioedit PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(marioedit PRIVATE -Wall -Wextra -Wno-unused-parameter -Wno-missing-braces)

include_directories(ZXing ${WASMTIME_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}/third_party/zlib ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/zlib ${SDL_INCLUDES} ${CURL_INCLUDE})

if(WIN32)
	target_link_libraries(marioedit PUBLIC ZXing skia fmt zlibstatic ${CURL_LIB} wasmtime SDL3-static gdi32 opengl32)
elseif(ANDROID)
	target_link_libraries(marioedit PUBLIC ZXing skia fmt zlibstatic ${CURL_LIB} wasmtime GLESv3 EGL)
endif()
target_include_directories(marioedit PUBLIC include ${SKIA_DIR} ${SKIA_DIR}/include fmt)