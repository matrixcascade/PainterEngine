# Copyright (C) 2023 Advanced Micro Devices, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.16)

#################################################################
#PainterEngine Directory
set(PAINTERENGINE_DIR c:/working/PainterEngine)
#################################################################

include_directories(${PAINTERENGINE_DIR})
include_directories(${PAINTERENGINE_DIR}/runtime)
#include_directories(${PAINTERENGINE_PROJECT_DIR})
#aux_source_directory(${PAINTERENGINE_PROJECT_DIR} PROJECT_DIR_SRC)
#add_library(PainterEngineForAndroid SHARED ${PROJECT_DIR_SRC})


include(${CMAKE_SOURCE_DIR}/Empty_applicationExample.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/UserConfig.cmake)
set(APP_NAME app_component)
project(${APP_NAME})

find_package(common)
enable_language(C ASM CXX)
collect(PROJECT_LIB_DEPS xilstandalone)
collect(PROJECT_LIB_DEPS xil)
collect(PROJECT_LIB_DEPS xiltimer)
collect(PROJECT_LIB_DEPS gcc)
collect(PROJECT_LIB_DEPS c)

aux_source_directory(${CMAKE_SOURCE_DIR} _sources)
#aux_source_directory(${PAINTERENGINE_DIR}/platform/android PAINTERENGINE_PLATFORM_ANDROID_SOURCES)
aux_source_directory(${PAINTERENGINE_DIR}/core _sources)
aux_source_directory(${PAINTERENGINE_DIR}/kernel _sources)
#aux_source_directory(${PAINTERENGINE_DIR}/runtime PAINTERENGINE_ARCH_SOURCES)

foreach (source ${_sources})
    get_filename_component(ext ${source} EXT)
    list(APPEND src_ext ${ext})
endforeach()

find_project_type ("${src_ext}" PROJECT_TYPE)

if("${PROJECT_TYPE}" STREQUAL "c++")
collect(PROJECT_LIB_DEPS stdc++)
endif()
collector_list (_deps PROJECT_LIB_DEPS)
list (APPEND _deps ${USER_LINK_LIBRARIES})

if("${PROJECT_TYPE}" STREQUAL "c++")
string (REPLACE ";" ",-l" _deps "${_deps}")
endif()
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
    set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
endif()
linker_gen("${CMAKE_SOURCE_DIR}/linker_files/")
string(APPEND CMAKE_C_FLAGS ${USER_COMPILE_OPTIONS})
string(APPEND CMAKE_CXX_FLAGS ${USER_COMPILE_OPTIONS})
string(APPEND CMAKE_C_LINK_FLAGS ${USER_LINK_OPTIONS})
string(APPEND CMAKE_CXX_LINK_FLAGS ${USER_LINK_OPTIONS})
if(NOT "${_sources}" STREQUAL "")
set_source_files_properties(${_sources} OBJECT_DEPENDS "${CMAKE_LIBRARY_PATH}/*.a")
add_executable(${APP_NAME}.elf ${_sources})
set_target_properties(${APP_NAME}.elf PROPERTIES LINK_DEPENDS ${CMAKE_SOURCE_DIR}/lscript.ld)
target_link_libraries(${APP_NAME}.elf -Wl,-T -Wl,\"${CMAKE_SOURCE_DIR}/lscript.ld\" -L\"${CMAKE_SOURCE_DIR}/\" -L\"${CMAKE_LIBRARY_PATH}/\" -L\"${USER_LINK_DIRECTORIES}/\" -Wl,--start-group,-l${_deps} -Wl,--end-group)
target_compile_definitions(${APP_NAME}.elf PUBLIC ${USER_COMPILE_DEFINITIONS})
target_include_directories(${APP_NAME}.elf PUBLIC ${USER_INCLUDE_DIRECTORIES})
print_elf_size(CMAKE_SIZE ${APP_NAME})
endif()
