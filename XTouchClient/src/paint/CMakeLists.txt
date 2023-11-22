cmake_minimum_required(VERSION 3.15)

project(Paint)

# Set default installation directory
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}../.." CACHE PATH "Installation Directory" FORCE)
endif()

set(CMAKE_EXE_LINKER_FLAGS "-export-dynamic")
set (APP_VERSION_MAJOR 1)
set (APP_VERSION_MINOR 0)
set (APP_TARGET paint)
set (CMAKE_CXX_STANDARD 20)

find_package (PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-3.0)

pkg_search_module(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/client/paint)

add_executable(${APP_TARGET}
              paint.cpp
              )

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories   (${GTKMM_LIBRARY_DIRS})

target_link_libraries(${APP_TARGET} PUBLIC PkgConfig::GTKMM)
