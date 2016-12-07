
#
# Copyright (C) The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.4.1)

# configure import libs
set(distribution_DIR ${CMAKE_SOURCE_DIR}/../../../../distribution)

add_library(lib_gmath STATIC IMPORTED)
set_target_properties(lib_gmath PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/gmath/lib/${ANDROID_ABI}/libgmath.a)

add_library(lib_magick STATIC IMPORTED)
set_target_properties(lib_magick PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/imagemagick/lib/${ANDROID_ABI}/libMagick++.a)

add_library(lib_magickcore STATIC IMPORTED)
set_target_properties(lib_magickcore PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/imagemagick/lib/${ANDROID_ABI}/libMagickCore.a)

add_library(lib_magickwand STATIC IMPORTED)
set_target_properties(lib_magickwand PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/imagemagick/lib/${ANDROID_ABI}/libMagickWand.a)

# shared lib will also be tucked into APK and sent to target
# refer to app/build.gradle, jniLibs section for that purpose.
# ${ANDROID_ABI} is handy for our purpose here. Probably this ${ANDROID_ABI} is
# the most valuable thing of this sample, the rest are pretty much normal cmake
add_library(lib_gperf SHARED IMPORTED)
set_target_properties(lib_gperf PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/gperf/lib/${ANDROID_ABI}/libgperf.so)

add_library(lib_assimp SHARED IMPORTED)
set_target_properties(lib_assimp PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/assimp/lib/${ANDROID_ABI}/libassimp.so)

add_library(lib_stlport SHARED IMPORTED)
set_target_properties(lib_stlport PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/system/lib/${ANDROID_ABI}/libstlport_shared.so)
#    ${ANDROID_NDK}/sources/cxx-stl/stlport/libs/${ANDROID_ABI}/libstlport_share.so)

# build application's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

add_library(hello-libs SHARED
            hello-libs.c
            ogldev_skinned_mesh.cpp
            gl3stub.cpp
            math_3d.cpp
            ogldev_texture.cpp
            skinning_technique.cpp
            technique.cpp
            ogldev_util.cpp
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

target_include_directories(hello-libs PRIVATE
                           ${distribution_DIR}/gmath/include
                           ${distribution_DIR}/gperf/include
                           ${distribution_DIR}/assimp/include
                           ${distribution_DIR}/imagemagick/include
                           ${distribution_DIR}/imagemagick/include/ImageMagick-6
                           ${ANDROID_NDK}/sources/android/native_app_glue)

target_link_libraries(hello-libs
                      android
                      lib_assimp
                      lib_magick
                      lib_magickcore
                      lib_magickwand
                      log
                      EGL
                      z
                      stdc++
                      lib_stlport
                      GLESv2)
