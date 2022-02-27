# the minimum version of CMake.
cmake_minimum_required(VERSION 3.10.2)
project(harmony)

message(${CMAKE_SYSTEM_NAME})   #OHOS


add_library(GlNative SHARED GlNative.cpp )
target_link_libraries(GlNative
                        libhilog_ndk.z.so
                        EGL
                        GLESv3
                         )