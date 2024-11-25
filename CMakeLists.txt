# 设置最低CMake版本要求
cmake_minimum_required(VERSION 3.10)

# 定义项目名称和使用的语言, 设置项目名称“PainterEngine”
project(PainterEngine LANGUAGES C CXX)

# 定义是否编译样例
option(BUILD_EXAMPLES "Build the examples." OFF)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 定义源文件目录, 在这里添加包含的所有源文件的子文件夹
set(SRC_DIRS core kernel platform/windows runtime)

# 初始化源文件列表
set(SOURCES)

# 遍历每个源文件目录，添加所有的C和C++源文件到SOURCES列表
foreach(SRC_DIR ${SRC_DIRS})
    file(GLOB_RECURSE DIR_SOURCES
        "${SRC_DIR}/*.c"
        "${SRC_DIR}/*.cpp"
    )
    list(APPEND SOURCES ${DIR_SOURCES})
endforeach()

# 打印所有找到的源文件（可选）
# message("Source files: ${SOURCES}")

# 添加静态库目标
add_library(PainterEngine STATIC ${SOURCES})

# 设置输出目录（可选）此处的目标名称要和静态库的目标一致
set_target_properties(PainterEngine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# 包含头文件目录（如果有）
include_directories(include)