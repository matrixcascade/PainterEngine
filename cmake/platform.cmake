# 编译平台指定

# 平台默认识别接口 或 外部参数注入
set(PLATFORM "" CACHE STRING "目标平台名称(windows/linux/macos/android/webassembly/harmonyos等)")
if(NOT PLATFORM) # 未指定时自动检测
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(PLATFORM_IMPL_DIR "windows")
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(PLATFORM_IMPL_DIR "linux")
    elseif(APPLE)
        set(PLATFORM_IMPL_DIR "macos")
    elseif(ANDROID)
        set(PLATFORM_IMPL_DIR "android")
    elseif(EMSCRIPTEN)
        set(PLATFORM_IMPL_DIR "webassembly")
    else()
        message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
    endif()
    message(STATUS "Building for ${PLATFORM_IMPL_DIR}")
else() # 用户显式指定平台
    set(PLATFORM_IMPL_DIR "${PLATFORM}")
    message(STATUS "Using custom platform: ${PLATFORM}")
endif()

set(PLATFORM_ROOT_DIR ${PAINTER_ENGINE_ROOT}/platform)

# 平台路径校验
if(NOT EXISTS ${PLATFORM_ROOT_DIR}/${PLATFORM_IMPL_DIR})
    message(FATAL_ERROR "Platform directory not found: ${PLATFORM_IMPL_DIR}")
endif()

# 查找必需依赖
find_package(OpenGL REQUIRED)

# 平台特定配置
if(WIN32)
    # Windows平台可能需要额外的库
    if(PLATFORM_IMPL_DIR STREQUAL "windows_gdi")
        # GDI特定配置
    endif()
elseif(UNIX AND NOT APPLE)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)  # 解决pthread依赖问题
elseif(APPLE)
    # APPLE特定配置
elseif(ANDROID)
    # Android特定配置
elseif(EMSCRIPTEN)
    # WebAssembly特定配置
endif()

# 平台特定编译选项
if(PLATFORM_IMPL_DIR STREQUAL "windows")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(PLATFORM_IMPL_DIR STREQUAL "linux_embedded")
    # 嵌入式Linux特定配置
    add_definitions(-DPX_PLATFORM_EMBEDDED)
endif()
