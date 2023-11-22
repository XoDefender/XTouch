cmake_minimum_required(VERSION 3.15)

project(Server)

set(CMAKE_EXE_LINKER_FLAGS "-export-dynamic")
set (APP_VERSION_MAJOR 1)
set (APP_VERSION_MINOR 0)
set (APP_TARGET Server)
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

add_executable(${APP_TARGET} 
                server.cxx
                )
                
target_include_directories(Server PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
target_link_libraries(Server PUBLIC ${LIBMYSQLCLIENT_LIBRARIES})
target_link_libraries(Server PUBLIC mysqlcppconn)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED)
target_link_libraries(asio INTERFACE pthread)

target_include_directories(${APP_TARGET} PRIVATE "/usr/include/mysql-cppconn-8/jdbc/")