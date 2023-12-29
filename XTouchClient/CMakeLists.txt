cmake_minimum_required(VERSION 3.15)

project(client)

set(CMAKE_EXE_LINKER_FLAGS "-export-dynamic")
set (APP_TARGET client)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/adm/cmake" ${CMAKE_MODULE_PATH})
set (OpenCASCADE_DIR "/usr/local/lib/cmake/opencascade")
set (CLIENT_DIR "../XTouchClient/src/client/")
set (MAIN_DIR "../XTouchClient/src/main/")
set (USE_GLES2 OFF CACHE BOOL "Use OpenGL ES instead of desktop OpenGL")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/client)

include(FindPkgConfig)
if (USE_GLES2)
  add_definitions (-DHAVE_GLES2)
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

set(SOURCES 
              ${MAIN_DIR}main.cxx 
              ${MAIN_DIR}global.cxx 
              ${MAIN_DIR}LoginWindow.cxx
              ${MAIN_DIR}AdminWindow.cxx
              ${MAIN_DIR}MainWindow.cxx
              ${MAIN_DIR}InFolderWindow.cxx
              ${MAIN_DIR}ModelFileManager.cxx
              ${MAIN_DIR}OcctGtkViewer.cxx
              ${MAIN_DIR}ViewerInteractor.cxx
	            ${MAIN_DIR}ControlPanel.cxx
              ${MAIN_DIR}PaintWindow.cxx
              ${MAIN_DIR}DrawHelper.cxx
              ${CLIENT_DIR}Client.cxx)
                              
add_executable(${APP_TARGET} ${SOURCES})

target_include_directories(${APP_TARGET} PRIVATE ${CLIENT_DIR})

if (USE_GLES2)
  target_link_libraries (${APP_TARGET} PRIVATE TKOpenGles)
  target_link_libraries (${APP_TARGET} PRIVATE GLESv2)
else()
  target_link_libraries (${APP_TARGET} PRIVATE TKOpenGl)
  target_link_libraries (${APP_TARGET} PRIVATE ${OPENGL_LIBRARIES})
endif()

set (OCCT_LIBS TKXDEIGES TKIGES TKXDESTEP TKSTEP TKSTEPAttr TKSTEP209 TKSTEPBase TKXSBase TKBinXCAF TKXmlXCAF TKBin TKXml TKXmlL TKBinL TKXCAF TKVCAF TKCAF TKV3d TKMesh TKOffset TKFillet TKShHealing TKBool TKBO TKHLR TKPrim TKService TKTopAlgo TKGeomAlgo TKBRep TKGeomBase TKG3d TKG2d TKMath TKLCAF TKCDF TKernel)
target_link_libraries (${APP_TARGET} PRIVATE ${OCCT_LIBS})
target_include_directories(${APP_TARGET} PRIVATE ${OpenCASCADE_INCLUDE_DIR})

target_link_libraries (${APP_TARGET} PRIVATE EGL)
target_link_libraries (${APP_TARGET} PRIVATE X11)
target_link_libraries (${APP_TARGET} PRIVATE dl)
target_link_libraries (${APP_TARGET} PRIVATE pthread)

target_link_libraries(${APP_TARGET} PUBLIC PkgConfig::GTKMM)
target_include_directories(${APP_TARGET} PUBLIC ${GTKMM_INCLUDE_DIRS})

target_link_libraries(${APP_TARGET} PUBLIC ${LIBMYSQLCLIENT_LIBRARIES})
target_link_libraries(${APP_TARGET} PUBLIC mysqlcppconn)

target_include_directories(${APP_TARGET} PUBLIC ${LIBMYSQLCLIENT_LIBRARY_DIRS})
target_include_directories(${APP_TARGET} PRIVATE "/usr/include/mysql-cppconn-8/jdbc/")
target_include_directories(${APP_TARGET} PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})