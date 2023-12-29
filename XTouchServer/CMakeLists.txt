cmake_minimum_required(VERSION 3.15)

project(server)

set(CMAKE_EXE_LINKER_FLAGS "-export-dynamic")
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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/server)

set(SOURCES_DIR ../XTouchServer/src/)
set(SOURCES 
        ${SOURCES_DIR}main.cxx
        ${SOURCES_DIR}SessionManager.cxx
        ${SOURCES_DIR}Server.cxx)

add_executable(${APP_TARGET} ${SOURCES})
                
target_include_directories(${APP_TARGET} PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
target_link_libraries(${APP_TARGET} PUBLIC ${LIBMYSQLCLIENT_LIBRARIES})
target_link_libraries(${APP_TARGET} PUBLIC mysqlcppconn)
target_link_directories(${APP_TARGET} PUBLIC ${LIBMYSQLCLIENT_LIBRARY_DIRS}) 
target_include_directories(${APP_TARGET} PRIVATE "/usr/include/mysql-cppconn-8/jdbc/")