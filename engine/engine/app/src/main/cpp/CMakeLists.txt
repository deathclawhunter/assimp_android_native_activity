cmake_minimum_required(VERSION 3.4.1)

# configure import libs
set(distribution_DIR ${CMAKE_SOURCE_DIR}/../../../../distribution)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -v")
set(CMAKE_C_FLAGS "-DMAGICKCORE_PNG_DELEGATE=1")
set( CMAKE_VERBOSE_MAKEFILE on )

add_library(lib_assimp SHARED IMPORTED)
set_target_properties(lib_assimp PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/assimp/lib/${ANDROID_ABI}/libassimp.so)

add_library(lib_stlport SHARED IMPORTED)
set_target_properties(lib_stlport PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/system/lib/${ANDROID_ABI}/libstlport_shared.so)

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

file(GLOB MagickCore_src
    "MagickCore/*.c"
)

file(GLOB filters_src
    "filters/*.c"
)

file(GLOB app_src
    "app/*.c"
)

file(GLOB app_xx_src
    "app/*.cpp"
)

file(GLOB FreeGlut_src
    "FreeGlut/*.c"
)

file(GLOB png_src
    "png/*.c"
)

add_library(engine SHARED
            ${MagickCore_src}
            ${filters_src}
            ${app_src}
            ${app_xx_src}
            ${FreeGlut_src}
            ${magickwand_src}
            ${Magick_src}
            ${coder_src}
            ${png_src}
             )

target_include_directories(engine PRIVATE
                           ${distribution_DIR}/assimp/include
                           ${distribution_DIR}/imagemagick/include
                           ${distribution_DIR}/imagemagick/include/coders
                           ${distribution_DIR}/../app/src/main/cpp
                           ${distribution_DIR}/imagemagick/include/ImageMagick-6
                           ${distribution_DIR}/freeglut/include
                           ${distribution_DIR}/png/include
                           ${CMAKE_SOURCE_DIR}/app
                           ${ANDROID_NDK}/sources/android/native_app_glue)

target_link_libraries(engine
                      android
                      lib_assimp
                      log
                      EGL
                      z
                      stdc++
                      lib_stlport
                      GLESv1_CM
                      GLESv2)
