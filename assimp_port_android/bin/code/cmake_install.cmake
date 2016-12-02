# Install script for directory: /Volumes/Elements/proj/3D/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Volumes/Elements/Library/Android/ndk-bundle/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/user")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Volumes/Elements/proj/3D/bin/code/libassimp.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Volumes/Elements/Library/Android/ndk-bundle/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/anim.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/ai_assert.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/camera.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/color4.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/color4.inl"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/config.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/defs.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/cfileio.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/light.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/material.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/material.inl"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/matrix3x3.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/matrix3x3.inl"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/matrix4x4.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/matrix4x4.inl"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/mesh.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/postprocess.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/quaternion.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/quaternion.inl"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/scene.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/metadata.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/texture.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/types.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/vector2.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/vector2.inl"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/vector3.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/vector3.inl"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/version.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/cimport.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/importerdesc.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/Importer.hpp"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/DefaultLogger.hpp"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/ProgressHandler.hpp"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/IOStream.hpp"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/IOSystem.hpp"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/Logger.hpp"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/LogStream.hpp"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/NullLogger.hpp"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/cexport.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/Exporter.hpp"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/Compiler/poppack1.h"
    "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "/Volumes/Elements/proj/3D/assimp/code/../include/assimp/port/AndroidJNI/AndroidJNIIOSystem.h")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Volumes/Elements/proj/3D/bin/port/AndroidJNI/cmake_install.cmake")

endif()

