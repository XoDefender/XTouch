cmake_minimum_required(VERSION 3.15)
project(XTouch)

set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_subdirectory("XTouchClient")
add_subdirectory("XTouchServer")