cmake_minimum_required(VERSION 3.12)
project(PainterEngine C)

# 构建选项
option(BUILD_SHARED_LIBS "构建动态库而不是静态库" OFF)
option(BUILD_EXAMPLES "构建示例程序" OFF)
option(ENABLE_TESTING "启用测试" OFF)

# 核心配置参数
set(PAINTER_ENGINE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
# 添加编译输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# # 在 Unix-like 系统中使用 $HOME 环境变量  
# execute_process(COMMAND sh -c "echo $HOME" OUTPUT_VARIABLE USER_HOME_DIR)  
# string(STRIP ${USER_HOME_DIR} USER_HOME_DIR)  
# # 自定义安装目录
# set(CMAKE_INSTALL_PREFIX "${USER_HOME_DIR}/${PROJECT_NAME}")

# 通用编译选项
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 启用额外警告
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
endif()

# 包含平台配置
include(${PAINTER_ENGINE_ROOT}/cmake/platform.cmake)

# 添加并配置freeglut子模块
add_subdirectory(depend/freeglut)
set_target_properties(freeglut_static PROPERTIES POSITION_INDEPENDENT_CODE ON)

# 收集源代码文件
file(GLOB_RECURSE COMMON_SOURCES
    "${PAINTER_ENGINE_ROOT}/core/*.c"
    "${PAINTER_ENGINE_ROOT}/kernel/*.c"
    "${PAINTER_ENGINE_ROOT}/runtime/*.c"
)

file(GLOB PLATFORM_SOURCES
    "${PAINTER_ENGINE_ROOT}/platform/${PLATFORM_IMPL_DIR}/*.c"
    "${PAINTER_ENGINE_ROOT}/platform/${PLATFORM_IMPL_DIR}/*.cpp"
)

# 创建库目标
add_library(PainterEngine 
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
)

# 包含目录配置
target_include_directories(PainterEngine PUBLIC
    ${PAINTER_ENGINE_ROOT}
    ${PAINTER_ENGINE_ROOT}/core
    ${PAINTER_ENGINE_ROOT}/kernel
    ${PAINTER_ENGINE_ROOT}/runtime
    ${PAINTER_ENGINE_ROOT}/platform/${PLATFORM_IMPL_DIR}
    ${PAINTER_ENGINE_ROOT}/depend/freeglut/include
)

# 平台特定链接库
if(WIN32)
    target_link_libraries(PainterEngine PUBLIC
        OpenGL::GL
    )
    if(PLATFORM_IMPL_DIR STREQUAL "windows")
        target_link_libraries(PainterEngine PUBLIC
            glu32
            opengl32
            winmm
        )
    elseif(PLATFORM_IMPL_DIR STREQUAL "windows_gdi")
        target_link_libraries(PainterEngine PUBLIC
            gdi32
            user32
            winmm
        )
    endif()
elseif(UNIX AND NOT APPLE)
    target_link_libraries(PainterEngine PUBLIC
        OpenGL::GL
        freeglut_static  # 使用freeglut实际生成的目标名称
    )
    target_include_directories(PainterEngine PUBLIC
        ${PAINTER_ENGINE_ROOT}/depend/freeglut/include
        ${PAINTER_ENGINE_ROOT}/depend/freeglut/include/GL
        Threads::Threads
        m
        dl
    )
elseif(APPLE)
    target_link_libraries(PainterEngine PUBLIC
        OpenGL::GL
        "-framework Cocoa"
        "-framework IOKit"
    )
elseif(ANDROID)
    target_link_libraries(PainterEngine PUBLIC
        android
        log
        EGL
        GLESv2
    )
elseif(EMSCRIPTEN)
    set_target_properties(PainterEngine PROPERTIES
        LINK_FLAGS "-s USE_WEBGL2=1 -s USE_GLFW=3"
    )
endif()

# 导出符号定义
if(BUILD_SHARED_LIBS)
    target_compile_definitions(PainterEngine PRIVATE PX_EXPORTS)
    target_compile_definitions(PainterEngine INTERFACE PX_IMPORTS)
endif()

# 安装规则
install(TARGETS PainterEngine DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${PAINTER_ENGINE_ROOT}/PainterEngine.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(DIRECTORY ${PAINTER_ENGINE_ROOT}/core DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${PAINTER_ENGINE_ROOT}/kernel DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${PAINTER_ENGINE_ROOT}/runtime DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.h")
