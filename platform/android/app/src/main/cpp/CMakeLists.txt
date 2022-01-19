cmake_minimum_required(VERSION 3.6.0)

#################################################################
#PainterEngine Directory
set(PAINTERENGINE_DIR ${PROJECT_SOURCE_DIR}/../../../../../../)
#Project Directory
set(PAINTERENGINE_PROJECT_DIR ${PROJECT_SOURCE_DIR}/../../../../../../examples/3D_bunny)

#################################################################
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11 -O3 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

include_directories(${ANDROID_NDK}/sources/android/native_app_glue/)
include_directories(${PAINTERENGINE_DIR})
include_directories(${PAINTERENGINE_DIR}/platform/modules)
include_directories(${PROJECT_SOURCE_DIR}/jni)
include_directories(${PAINTERENGINE_PROJECT_DIR})

find_library(log-lib log)
#暴露ANativeActivity_onCreate方法，否则在创建activity时，会crash
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

aux_source_directory(${PAINTERENGINE_PROJECT_DIR} PROJECT_DIR_SRC)
add_library(PainterEngineForAndroid SHARED ${PROJECT_DIR_SRC})

set(ANDROID_NATIVE_APP_GLUE ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
aux_source_directory(${PROJECT_SOURCE_DIR}/jni PAINTERENGINE_PLATFORM_ANDROID_SOURCES)
aux_source_directory(${PAINTERENGINE_DIR}/core PAINTERENGINE_CORE_SOURCES)
aux_source_directory(${PAINTERENGINE_DIR}/kernel PAINTERENGINE_KERNEL_SOURCES)
aux_source_directory(${PAINTERENGINE_DIR}/architecture PAINTERENGINE_ARCH_SOURCES)

add_library(PAINTERENGINELIB
        STATIC
        ${ANDROID_NATIVE_APP_GLUE}
        ${PAINTERENGINE_PLATFORM_ANDROID_SOURCES}
        ${PAINTERENGINE_ARCH_SOURCES}
        ${PAINTERENGINE_KERNEL_SOURCES}
        ${PAINTERENGINE_CORE_SOURCES})
#PainterEngineForAndroid将在Manifest中被指定
target_link_libraries(PainterEngineForAndroid
        PAINTERENGINELIB
        android
        ${log-lib}
        EGL
        GLESv3
        OpenSLES
        log)