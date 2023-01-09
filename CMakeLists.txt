cmake_minimum_required(VERSION 3.10)
set(MARIOEDIT_VERSION "0.0.1")
project(marioedit VERSION ${MARIOEDIT_VERSION})

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(marioedit)
add_subdirectory(main)
add_subdirectory(test)
add_custom_target(marioedit_all ALL DEPENDS main test)