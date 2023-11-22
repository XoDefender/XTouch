cmake_minimum_required(VERSION 3.15)

project(ServerApp)

set(CMAKE_EXE_LINKER_FLAGS "-export-dynamic")
set (APP_VERSION_MAJOR 1)
set (APP_VERSION_MINOR 0)
set (APP_TARGET ServerApp)
set (CMAKE_CXX_STANDARD 20)

add_subdirectory(./Server)
