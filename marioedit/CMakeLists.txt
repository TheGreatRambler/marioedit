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

# Skia for rendering
set(SKIA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/skia)
set(SKIA_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/skia)
execute_process(
	COMMAND python tools/git-sync-deps
	WORKING_DIRECTORY ${SKIA_DIR})
if(APPLE)
	execute_process(
		COMMAND bin/gn gen ${SKIA_BUILD_DIR} "--args=is_official_build=true target_cpu=\"arm64\" is_component_build=true skia_use_system_libjpeg_turbo=false skia_use_system_libwebp=false skia_use_system_expat=false skia_use_system_zlib=false skia_use_system_libpng=false skia_use_system_harfbuzz=false skia_use_system_icu=false skia_use_system_icu=false"
		WORKING_DIRECTORY ${SKIA_DIR})
	add_library(skia SHARED IMPORTED)
	set_property(TARGET skia PROPERTY
		IMPORTED_LOCATION ${SKIA_BUILD_DIR}/libskia.dylib)
elseif(WIN32)
	execute_process(
		COMMAND bin/gn gen ${SKIA_BUILD_DIR} "--args=is_official_build=true clang_win=\"C:/Program Files/LLVM\" extra_cflags=[\"/MDd\"] skia_use_system_libjpeg_turbo=false skia_use_system_libwebp=false skia_use_system_expat=false skia_use_system_zlib=false skia_use_system_libpng=false skia_use_system_harfbuzz=false skia_use_system_icu=false"
		WORKING_DIRECTORY ${SKIA_DIR})
	add_library(skia STATIC IMPORTED)
	set_property(TARGET skia PROPERTY
		IMPORTED_LOCATION ${SKIA_BUILD_DIR}/skia.lib)
else()
	# Other (Android)
endif()
execute_process(COMMAND ninja WORKING_DIRECTORY ${SKIA_BUILD_DIR})

# fmt for some formatting
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/fmt ${CMAKE_CURRENT_BINARY_DIR}/third_party/fmt)

# Zlib for decompressing
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/zlib ${CMAKE_CURRENT_BINARY_DIR}/third_party/zlib)
# Replace .res.res files with .res, windows thing
if(NOT APPLE)
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
endif()

set(marioedit_SOURCES
	src/parser.cpp
	src/drawer.cpp
	src/SMM2CourseDecryptor/aes.cpp
	src/SMM2CourseDecryptor/decrypt.cpp
	src/MM2/level.cpp
	src/kaitai/kaitaistream.cpp
)

add_library(marioedit STATIC ${marioedit_SOURCES})
set_target_properties(marioedit PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(marioedit PRIVATE -Wall -Wextra -Wno-unused-parameter -Wno-missing-braces)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/third_party/zlib ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/zlib)

if(WIN32)
	target_link_libraries(marioedit PUBLIC skia fmt zlibstatic opengl32)
elseif(ANDROID)
	target_link_libraries(marioedit PUBLIC skia fmt zlibstatic)
elseif(APPLE)
	target_link_libraries(marioedit PUBLIC skia fmt zlibstatic)
endif()
target_include_directories(marioedit PUBLIC include ${SKIA_DIR} ${SKIA_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/fmt/include)