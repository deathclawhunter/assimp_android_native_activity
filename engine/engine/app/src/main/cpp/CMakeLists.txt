cmake_minimum_required(VERSION 3.4.1)

# configure import libs
set(distribution_DIR ${CMAKE_SOURCE_DIR}/../../../../distribution)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -frtti -v -DLOCAL_ARM_MODE=arm -DASSIMP_BUILD_NO_IFC_IMPORTER -DASSIMP_BUILD_NO_IRRMESH_IMPORTER  -DASSIMP_BUILD_NO_IRR_IMPORTER -DASSIMP_BUILD_NO_C4D_IMPORTER")
set(CMAKE_C_FLAGS "-DMAGICKCORE_PNG_DELEGATE=1 -frtti -DLOCAL_ARM_MODE=arm -DASSIMP_BUILD_NO_IFC_IMPORTER -DASSIMP_BUILD_NO_IRRMESH_IMPORTER  -DASSIMP_BUILD_NO_IRR_IMPORTER -DASSIMP_BUILD_NO_C4D_IMPORTER")
set( CMAKE_VERBOSE_MAKEFILE on )

add_library(lib_assimp SHARED IMPORTED)
set_target_properties(lib_assimp PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/assimp/lib/${ANDROID_ABI}/libassimp.so)

add_library(lib_stlport SHARED IMPORTED)
set_target_properties(lib_stlport PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/system/lib/${ANDROID_ABI}/libstlport_shared.so)

add_library(lib_avutil SHARED IMPORTED)
set_target_properties(lib_avutil PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/ffmpeg/lib/${ANDROID_ABI}/libavutil-55.so)

add_library(lib_swresample SHARED IMPORTED)
set_target_properties(lib_swresample PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/ffmpeg/lib/${ANDROID_ABI}/libswresample-2.so)

add_library(lib_avcodec SHARED IMPORTED)
set_target_properties(lib_avcodec PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/ffmpeg/lib/${ANDROID_ABI}/libavcodec-57.so)

add_library(lib_avformat SHARED IMPORTED)
set_target_properties(lib_avformat PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/ffmpeg/lib/${ANDROID_ABI}/libavformat-57.so)

add_library(lib_swscale SHARED IMPORTED)
set_target_properties(lib_swscale PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/ffmpeg/lib/${ANDROID_ABI}/libswscale-4.so)

add_library(lib_avfilter SHARED IMPORTED)
set_target_properties(lib_avfilter PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/ffmpeg/lib/${ANDROID_ABI}/libavfilter-6.so)

# add_library(lib_freetype SHARED IMPORTED)
# set_target_properties(lib_freetype PROPERTIES IMPORTED_LOCATION
#    ${distribution_DIR}/freetype/lib/${ANDROID_ABI}/libft2.so)

add_library(lib_freetype STATIC IMPORTED)
set_target_properties(lib_freetype PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/freetype/lib/${ANDROID_ABI}/libfreetype2-static.a)

#add_library(lib_gcc STATIC IMPORTED)
#set_target_properties(lib_gcc PROPERTIES IMPORTED_LOCATION
#    ${distribution_DIR}/gcc/lib/${ANDROID_ABI}/libgcc.a)


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

file(GLOB ogldev_src
    "ogldev/*.c"
)

file(GLOB ogldev_xx_src
    "ogldev/*.cpp"
)

file(GLOB shaders_src
    "shaders/*.c"
)

file(GLOB shaders_xx_src
    "shaders/*.cpp"
)

file(GLOB meshes_src
    "meshes/*.c"
)

file(GLOB meshes_xx_src
    "meshes/*.cpp"
)

file(GLOB plugins_src
    "plugins/*.c"
)

file(GLOB plugins_xx_src
    "plugins/*.cpp"
)

file(GLOB FreeGlut_src
    "FreeGlut/*.c"
)

file(GLOB png_src
    "png/*.c"
)


#file(GLOB assimp_c_src
#    "assimp/*.c"
#)

#file(GLOB assimp_cxx_src
#    "assimp/*.cpp"
#)

#file(GLOB assimp_cc_src
#    "assimp/*.cc"
#)

file(GLOB utils_src
    "utils/*.c"
)

file(GLOB utils_xx_src
    "utils/*.cpp"
)

add_library(engine SHARED
            ${MagickCore_src}
            ${filters_src}
            ${app_src}
            ${app_xx_src}
            ${meshes_src}
            ${meshes_xx_src}
            ${plugins_src}
            ${plugins_xx_src}
            ${ogldev_src}
            ${ogldev_xx_src}
            ${shaders_src}
            ${shaders_xx_src}
            ${FreeGlut_src}
            ${magickwand_src}
            ${Magick_src}
            ${coder_src}
            ${png_src}
            ${utils_src}
            ${utils_xx_src}
            ${freetype_src}
#            ${assimp_c_src}
#            ${assimp_cxx_src}
#            ${assimp_cc_src}
             )

target_include_directories(engine PRIVATE
                           ${distribution_DIR}/assimp/include
                           ${distribution_DIR}/imagemagick/include
                           ${distribution_DIR}/imagemagick/include/coders
                           ${distribution_DIR}/../app/src/main/cpp
                           ${distribution_DIR}/imagemagick/include/ImageMagick-6
                           ${distribution_DIR}/freeglut/include
                           ${distribution_DIR}/png/include
                           ${distribution_DIR}/ffmpeg/include
                           ${distribution_DIR}/freetype/include
                           ${CMAKE_SOURCE_DIR}/app
                           ${CMAKE_SOURCE_DIR}/shaders
                           ${CMAKE_SOURCE_DIR}/plugins
                           ${CMAKE_SOURCE_DIR}/meshes
                           ${CMAKE_SOURCE_DIR}/ogldev
                           ${CMAKE_SOURCE_DIR}/utils
                           ${CMAKE_SOURCE_DIR}/assimp
                           ${CMAKE_SOURCE_DIR}/assimp/contrib
                           ${CMAKE_SOURCE_DIR}/assimp/contrib/unzip
                           ${CMAKE_SOURCE_DIR}/assimp/contrib/zlib
                           ${CMAKE_SOURCE_DIR}/assimp/contrib/Open3DGC
                           ${CMAKE_SOURCE_DIR}/assimp/contrib/irrXML
                           ${CMAKE_SOURCE_DIR}/assimp/contrib/openddlparser/include
                           ${CMAKE_SOURCE_DIR}/assimp/contrib/poly2tri/poly2tri
                           ${CMAKE_SOURCE_DIR}/assimp/contrib/poly2tri/poly2tri/common
                           ${CMAKE_SOURCE_DIR}/assimp/contrib/poly2tri/poly2tri/sweep
                           ${CMAKE_SOURCE_DIR}/assimp/contrib/rapidjson/include
                           ${CMAKE_SOURCE_DIR}/assimp/contrib/ConvertUTF
                           ${CMAKE_SOURCE_DIR}/assimp/contrib/clipper
                           ${ANDROID_NDK}/sources/android/native_app_glue)

target_link_libraries(engine
                      lib_avutil
                      lib_swresample
                      lib_avcodec
                      lib_avformat
                      lib_swscale
                      lib_avfilter
                      android
                      log
                      EGL
                      z
                      stdc++
                      lib_stlport
                      GLESv1_CM
                      lib_assimp
                      GLESv2
                      OpenSLES
                      gcc
                      lib_freetype)
