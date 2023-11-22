cmake_minimum_required(VERSION 3.15)

project(server)

set(CMAKE_EXE_LINKER_FLAGS "-export-dynamic")
set (APP_VERSION_MAJOR 1)
set (APP_VERSION_MINOR 0)
set (APP_TARGET server)
set (CMAKE_CXX_STANDARD 20)

find_package (PkgConfig REQUIRED)
pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)
                       
foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
endforeach()

foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
endforeach()

link_directories(${LIBMYSQLCLIENT_LIBRARY_DIRS}) 

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/server)

set(SOURCES_DIR ../XTouchServer/src/)
set(SOURCES 
        ${SOURCES_DIR}server.cxx
        ${SOURCES_DIR}SessionManager.cxx)

add_executable(${APP_TARGET} ${SOURCES})
                
target_include_directories(${APP_TARGET}  PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
target_link_libraries(${APP_TARGET}  PUBLIC ${LIBMYSQLCLIENT_LIBRARIES})
target_link_libraries(${APP_TARGET}  PUBLIC mysqlcppconn)

target_include_directories(${APP_TARGET} PRIVATE "/usr/include/mysql-cppconn-8/jdbc/")