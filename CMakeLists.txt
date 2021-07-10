cmake_minimum_required(VERSION 2.8)
project(PainterEnginel)


aux_source_directory(${PROJECT_SOURCE_DIR}/core CORE_SOUERCE_DIR)
aux_source_directory(${PROJECT_SOURCE_DIR}/kernel KERNEL_SOURCE_DIR)
aux_source_directory(${PROJECT_SOURCE_DIR}/architecture ARCHIVE_SOURCE_CODE)
add_library(PainterEnginel SHARED ${CORE_SOUERCE_DIR} ${KERNEL_SOURCE_DIR} ${ARCHIVE_SOURCE_CODE})

if(NOT DEFINED INSTALL_LIB_DIR)
    set(INSTALL_LIB_DIR lib)
endif()
if(NOT DEFINED INSTALL_BIN_DIR)
    set(INSTALL_BIN_DIR bin)
endif()
if(NOT DEFINED INSTALL_INCLUDE_DIR)
    set(INSTALL_INCLUDE_DIR include)
endif()

set(LIBRARY_OUTPUT_DIRECTORY ${INSTALL_LIB_DIR})
set(EXECUTABLE_OUTPUT_PATH ${INSTALL_BIN_DIR})