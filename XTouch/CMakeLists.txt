cmake_minimum_required(VERSION 3.15)

project(XTouch)

set(CMAKE_EXE_LINKER_FLAGS "-export-dynamic")
set (APP_VERSION_MAJOR 6)
set (APP_VERSION_MINOR 0)
set (APP_TARGET XTouch)
set (CMAKE_CXX_STANDARD 20)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/adm/cmake" ${CMAKE_MODULE_PATH})
set (OpenCASCADE_DIR "/usr/local/lib/cmake/opencascade")
set (OpenCASCADE_INCLUDE_DIR "/home/vizard/OCCT/")
set (CLIENT_DIR "./Client/")

set (USE_GLES2 OFF CACHE BOOL "Use OpenGL ES instead of desktop OpenGL")

#include(${CMAKE_CURRENT_LIST_DIR}/src/paint/CMakeLists.txt)
add_subdirectory(src/Paint)

include(FindPkgConfig)
if (USE_GLES2)
  add_definitions (-DHAVE_GLES2)
  #find_package (OpenGLES2 REQUIRED)
else()
  find_package (OpenGL REQUIRED)
endif()

find_package (PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)

pkg_search_module(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)

find_package (OpenCASCADE REQUIRED)
if (NOT OpenCASCADE_FOUND)
  message (FATAL_ERROR "coult not find OpenCASCADE, please set OpenCASCADE_DIR variable" )
else()
  message (STATUS "Using OpenCASCADE from \"${OpenCASCADE_INSTALL_PREFIX}\"" )
  message (STATUS "OpenCASCADE_INCLUDE_DIR=${OpenCASCADE_INCLUDE_DIR}")
  message (STATUS "OpenCASCADE_LIBRARY_DIR=${OpenCASCADE_LIBRARY_DIR}")
endif()
                       
foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
endforeach()

link_directories(${LIBMYSQLCLIENT_LIBRARY_DIRS})  

set(SOURCES 
              main.cxx 
              global.cxx 
              LoginWindow.cxx
              AdminWindow.cxx
              MainWindow.cxx
              InFolderWindow.cxx
              ModelFileManager.cxx
              OcctGtkViewer.cxx
              ViewerInteractor.cxx
	            ControlPanel.cxx
              ${CLIENT_DIR}Client.cxx)
                              
add_executable(${APP_TARGET} ${SOURCES})

# extra search paths
include_directories(${OpenCASCADE_INCLUDE_DIR})
#link_directories   (${OpenCASCADE_LIBRARY_DIR})

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories   (${GTKMM_LIBRARY_DIRS})

target_include_directories(XTouch PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
target_include_directories(XTouch PRIVATE ${CLIENT_DIR})

# define dependencies
set (anOcctLibs TKXDEIGES TKIGES TKXDESTEP TKSTEP TKSTEPAttr TKSTEP209 TKSTEPBase TKXSBase TKBinXCAF TKXmlXCAF TKBin TKXml TKXmlL TKBinL TKXCAF TKVCAF TKCAF TKV3d TKMesh TKOffset TKFillet TKShHealing TKBool TKBO TKHLR TKPrim TKService TKTopAlgo TKGeomAlgo TKBRep TKGeomBase TKG3d TKG2d TKMath TKLCAF TKCDF TKernel)
target_link_libraries (${APP_TARGET} PRIVATE ${anOcctLibs})
if (USE_GLES2)
  target_link_libraries (${APP_TARGET} PRIVATE TKOpenGles)
  target_link_libraries (${APP_TARGET} PRIVATE GLESv2)
else()
  target_link_libraries (${APP_TARGET} PRIVATE TKOpenGl)
  target_link_libraries (${APP_TARGET} PRIVATE ${OPENGL_LIBRARIES})
endif()

if (UNIX)
  target_link_libraries (${APP_TARGET} PRIVATE EGL)
  target_link_libraries (${APP_TARGET} PRIVATE X11)
  target_link_libraries (${APP_TARGET} PRIVATE dl)
  target_link_libraries (${APP_TARGET} PRIVATE pthread)
endif()

target_link_libraries(XTouch PUBLIC PkgConfig::GTKMM)
target_link_libraries(XTouch PUBLIC ${LIBMYSQLCLIENT_LIBRARIES})
target_link_libraries(XTouch PUBLIC mysqlcppconn)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED)
target_link_libraries(asio INTERFACE pthread)

target_include_directories(${APP_TARGET} PRIVATE "/usr/include/mysql-cppconn-8/jdbc/")