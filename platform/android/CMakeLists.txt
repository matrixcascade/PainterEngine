cmake_minimum_required(VERSION 3.6.0)

#################################################################
#PainterEngine Directory
set(PAINTERENGINE_DIR D:/Git/PainterEngine)
#Project Directory
set(PAINTERENGINE_PROJECT_DIR D:/Git/project)
#################################################################
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11 -O3 ")

include_directories(${ANDROID_NDK}/sources/android/native_app_glue/)
include_directories(${PAINTERENGINE_DIR})
include_directories(${PAINTERENGINE_DIR}/platform/android)
include_directories(${PAINTERENGINE_PROJECT_DIR})

find_library(log-lib log)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

aux_source_directory(${PAINTERENGINE_PROJECT_DIR} PROJECT_DIR_SRC)
add_library(PainterEngineForAndroid SHARED ${PROJECT_DIR_SRC})

set(ANDROID_NATIVE_APP_GLUE ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
aux_source_directory(${PAINTERENGINE_DIR}/platform/android PAINTERENGINE_PLATFORM_ANDROID_SOURCES)
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

target_link_libraries(PainterEngineForAndroid
        PAINTERENGINELIB
        android
        ${log-lib}
        EGL
        GLESv3
        OpenSLES
        log)