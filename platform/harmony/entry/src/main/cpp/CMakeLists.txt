
cmake_minimum_required(VERSION 3.6.0)

project("PainterEngineForHarmony")

#################################################################
#PainterEngine Directory
set(PAINTERENGINE_DIR ${PROJECT_SOURCE_DIR}/../../../../../../)
#Project Directory
set(PAINTERENGINE_PROJECT_DIR ${PROJECT_SOURCE_DIR}/../../../../../../examples/PainterEngineConsole)
#################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

ADD_DEFINITIONS(-DHARMONY )
# 包含头文件目录

file(GLOB_RECURSE JNI_FILE_H ${PROJECT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE JNI_FILE_CPP ${PROJECT_SOURCE_DIR}/*.cpp)
set(JNI_FILE
        ${JNI_FILE_H}
        ${JNI_FILE_CPP}
)

include_directories(${PAINTERENGINE_DIR})
include_directories(${PAINTERENGINE_DIR}/platform/modules)
include_directories(${PROJECT_SOURCE_DIR}/jni)
include_directories(${PAINTERENGINE_PROJECT_DIR})

aux_source_directory(${PAINTERENGINE_PROJECT_DIR} PROJECT_DIR_SRC)

aux_source_directory(${PROJECT_SOURCE_DIR}/jni PAINTERENGINE_PLATFORM_HARMONY_SOURCES)
aux_source_directory(${PAINTERENGINE_DIR}/core PAINTERENGINE_CORE_SOURCES)
aux_source_directory(${PAINTERENGINE_DIR}/kernel PAINTERENGINE_KERNEL_SOURCES)
aux_source_directory(${PAINTERENGINE_DIR}/architecture PAINTERENGINE_ARCH_SOURCES)

#add_library(PainterEngineForHarmony SHARED ${PROJECT_DIR_SRC})

add_library(PainterEngineForHarmony
        SHARED
        ${JNI_FILE}
        ${PAINTERENGINE_PLATFORM_HARMONY_SOURCES}
        ${PROJECT_DIR_SRC}
        ${PAINTERENGINE_ARCH_SOURCES}
        ${PAINTERENGINE_KERNEL_SOURCES}
        ${PAINTERENGINE_CORE_SOURCES}
        )

target_link_libraries(PainterEngineForHarmony
        #PAINTERENGINELIB
        #OpenSLES
        rawfile.z
        zgraphic.z
        hilog_ndk.z
        EGL
        GLESv3)
