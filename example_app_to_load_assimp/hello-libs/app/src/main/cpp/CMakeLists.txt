
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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -v")
set( CMAKE_VERBOSE_MAKEFILE on )

# add_library(lib_freeglut STATIC IMPORTED)
# set_target_properties(lib_freeglut PROPERTIES IMPORTED_LOCATION
#     ${distribution_DIR}/freeglut/lib/${ANDROID_ABI}/libfreeglut-gles.a)

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


file(GLOB coder_src
    "coders/*.c"
)

file(GLOB magickwand_src
    "magickwand/*.c"
)

file(GLOB Magick_src
    "Magick++/*.cpp"
)


add_library(hello-libs SHARED
            # hello-libs.c
            fg_runtime_android.c
            tutorial38.cpp
            ogldev_skinned_mesh.cpp
            gl3stub.cpp
            math_3d.cpp
            ogldev_texture.cpp
            skinning_technique.cpp
            technique.cpp
            ogldev_util.cpp
            ogldev_app.cpp
            camera.cpp
            glut_backend.cpp
            pipeline.cpp
            android_native_app_glue.c
            fg_cursor_android.c
            fg_ext_android.c
            fg_gamemode_android.c
            fg_init_android.c
            fg_input_devices_android.c
            fg_joystick_android.c
            fg_main_android.c
            fg_spaceball_android.c
            fg_state_android.c
            fg_structure_android.c
            fg_window_android.c
            fg_callbacks.c
            fg_cursor.c
            fg_display.c
            fg_ext.c
            fg_font_data.c
            fg_gamemode.c
            fg_geometry.c
            fg_gl2.c
            fg_init.c
            fg_input_devices.c
            fg_joystick.c
            fg_main.c
            fg_misc.c
            fg_overlay.c
            fg_spaceball.c
            fg_state.c
            fg_stroke_mono_roman.c
            fg_stroke_roman.c
            fg_structure.c
            fg_teapot.c
            fg_videoresize.c
            fg_window.c
            gles_stubs.c
            fg_display_egl.c
            fg_ext_egl.c
            fg_init_egl.c
            fg_state_egl.c
            fg_structure_egl.c
            fg_window_egl.c
            xparsegeometry_repl.c
            accelerate.c
            animate.c
            annotate.c
            artifact.c
            attribute.c
            blob.c
            cache-view.c
            distribute-cache.c
            magic.c
            quantize.c
            threshold.c
            cache.c
            draw.c
            magick.c
            quantum-export.c
            timer.c
            channel.c
            effect.c
            matrix.c
            quantum-import.c
            token.c
            cipher.c
            enhance.c
            memory.c
            quantum.c
            transform.c
            client.c
            exception.c
            mime.c
            random.c
            type.c
            coder.c
            feature.c
            module.c
            registry.c
            utility.c
            color.c
            fourier.c
            monitor.c
            resample.c
            version.c
            colormap.c
            fx.c
            montage.c
            resize.c
            vision.c
            colorspace.c
            gem.c
            morphology.c
            resource.c
            vms.c
            compare.c
            geometry.c
            nt-base.c
            segment.c
            widget.c
            composite.c
            histogram.c
            nt-feature.c
            semaphore.c
            xml-tree.c
            compress.c
            identify.c
            opencl.c
            shear.c
            xwindow.c
            configure.c
            image-view.c
            option.c
            signature.c
            constitute.c
            image.c
            paint.c
            splay-tree.c
            decorate.c
            layer.c
            pixel.c
            static.c
            delegate.c
            linked-list.c
            policy.c
            statistic.c
            deprecate.c
            list.c
            prepress.c
            stream.c
            display.c
            locale.c
            profile.c
            string.c
            distort.c
            log.c
            property.c
            thread.c
            ${magickwand_src}
            ${Magick_src}
            analyze.c
            ${coder_src}
             )

target_include_directories(hello-libs PRIVATE
                           ${distribution_DIR}/gmath/include
                           ${distribution_DIR}/gperf/include
                           ${distribution_DIR}/assimp/include
                           ${distribution_DIR}/imagemagick/include
                           ${distribution_DIR}/imagemagick/include/coders
                           ${distribution_DIR}/../app/src/main/cpp
                           ${distribution_DIR}/imagemagick/include/ImageMagick-6
                           ${distribution_DIR}/freeglut/include
                           ${ANDROID_NDK}/sources/android/native_app_glue)

target_link_libraries(hello-libs
                      android
                      lib_assimp
                      log
                      EGL
                      z
                      stdc++
                      lib_stlport
                      GLESv1_CM
                      GLESv2)
