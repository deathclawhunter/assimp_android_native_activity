# Install script for directory: ../../../assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/Volumes/Elements/proj/assimp_with_native_activity/assimp_android_native_activity/assimp_port_android/bin/code/code/libassimp.3.3.1.dylib"
    "/Volumes/Elements/proj/assimp_with_native_activity/assimp_android_native_activity/assimp_port_android/bin/code/code/libassimp.3.dylib"
    "/Volumes/Elements/proj/assimp_with_native_activity/assimp_android_native_activity/assimp_port_android/bin/code/code/libassimp.dylib"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.3.3.1.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.3.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.dylib"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      execute_process(COMMAND "/usr/bin/install_name_tool"
        -id "/usr/local/lib/libassimp.3.dylib"
        "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "../../../assimp/code/../include/assimp/anim.h"
    "../../../assimp/code/../include/assimp/ai_assert.h"
    "../../../assimp/code/../include/assimp/camera.h"
    "../../../assimp/code/../include/assimp/color4.h"
    "../../../assimp/code/../include/assimp/color4.inl"
    "../../../assimp/code/../include/assimp/config.h"
    "../../../assimp/code/../include/assimp/defs.h"
    "../../../assimp/code/../include/assimp/cfileio.h"
    "../../../assimp/code/../include/assimp/light.h"
    "../../../assimp/code/../include/assimp/material.h"
    "../../../assimp/code/../include/assimp/material.inl"
    "../../../assimp/code/../include/assimp/matrix3x3.h"
    "../../../assimp/code/../include/assimp/matrix3x3.inl"
    "../../../assimp/code/../include/assimp/matrix4x4.h"
    "../../../assimp/code/../include/assimp/matrix4x4.inl"
    "../../../assimp/code/../include/assimp/mesh.h"
    "../../../assimp/code/../include/assimp/postprocess.h"
    "../../../assimp/code/../include/assimp/quaternion.h"
    "../../../assimp/code/../include/assimp/quaternion.inl"
    "../../../assimp/code/../include/assimp/scene.h"
    "../../../assimp/code/../include/assimp/metadata.h"
    "../../../assimp/code/../include/assimp/texture.h"
    "../../../assimp/code/../include/assimp/types.h"
    "../../../assimp/code/../include/assimp/vector2.h"
    "../../../assimp/code/../include/assimp/vector2.inl"
    "../../../assimp/code/../include/assimp/vector3.h"
    "../../../assimp/code/../include/assimp/vector3.inl"
    "../../../assimp/code/../include/assimp/version.h"
    "../../../assimp/code/../include/assimp/cimport.h"
    "../../../assimp/code/../include/assimp/importerdesc.h"
    "../../../assimp/code/../include/assimp/Importer.hpp"
    "../../../assimp/code/../include/assimp/DefaultLogger.hpp"
    "../../../assimp/code/../include/assimp/ProgressHandler.hpp"
    "../../../assimp/code/../include/assimp/IOStream.hpp"
    "../../../assimp/code/../include/assimp/IOSystem.hpp"
    "../../../assimp/code/../include/assimp/Logger.hpp"
    "../../../assimp/code/../include/assimp/LogStream.hpp"
    "../../../assimp/code/../include/assimp/NullLogger.hpp"
    "../../../assimp/code/../include/assimp/cexport.h"
    "../../../assimp/code/../include/assimp/Exporter.hpp"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "../../../assimp/code/../include/assimp/Compiler/pushpack1.h"
    "../../../assimp/code/../include/assimp/Compiler/poppack1.h"
    "../../../assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

