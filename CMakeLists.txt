cmake_minimum_required(VERSION 3.19)

project(PainterEngine LANGUAGES C CXX)

set(TARGET_NAME "PainterEngine")
set(PROJECT_PATH "project")
set(PAINTERENGINE_PATH ".")

if(WIN32)
    set(PLATFORM "windows")
    message("Windows platform detected")
elseif(ANDROID)
    set(PLATFORM "android")
    message("Android platform detected")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    set(PLATFORM "webassembly")
    message("Web platform detected")
elseif(UNIX)
    set(PLATFORM "linux")
    message("Linux platform detected")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

aux_source_directory(${PROJECT_PATH} PROJECT_SRC)
aux_source_directory(${PAINTERENGINE_PATH}/core PAINTERENGINE_SRC_0)
aux_source_directory(${PAINTERENGINE_PATH}/kernel PAINTERENGINE_SRC_1)
aux_source_directory(${PAINTERENGINE_PATH}/architecture PAINTERENGINE_SRC_2)
aux_source_directory(${PAINTERENGINE_PATH}/platform/${PLATFORM} PAINTERENGINE_SRC_3)

include_directories(
    "${PAINTERENGINE_PATH}"
    "${PROJECT_PATH}"
    "${PAINTERENGINE_PATH}/platform/${PLATFORM}"
)

set(CMAKE_C_STANDARD 11)

set(COMMON_SRC
    ${PAINTERENGINE_SRC_0}
    ${PAINTERENGINE_SRC_1}
    ${PAINTERENGINE_SRC_2}
    ${PAINTERENGINE_SRC_3}
)

########################################################
if(PLATFORM STREQUAL "android")
    set(ANDROID_STL c++_static)
    include_directories(${ANDROID_NDK}/sources/android/native_app_glue)
    find_library(log-lib log)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
    set(ANDROID_NATIVE_APP_GLUE ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

    add_library(PAINTERENGINELIB
        STATIC
        ${ANDROID_NATIVE_APP_GLUE}
        ${COMMON_SRC}
    )

    add_library(PainterEngineForAndroid SHARED ${PROJECT_SRC})

    target_link_libraries(PainterEngineForAndroid
        PAINTERENGINELIB
        android
        ${log-lib}
        EGL
        GLESv3
        OpenSLES
        log)
elseif(PLATFORM STREQUAL "webassembly")
    # >>> HOW TO BUILD
    # source ~/emsdk/emsdk_env.sh
    # em++ --version
    # mkdir -p build/web
    # cp platform/webassembly/index.html build/web
    # cp -r platform/webassembly/assets build/web
    # cd build/web
    # emcmake cmake ../../CMakeLists.txt
    # emmake make

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -sUSE_SDL -sUSE_SDL_IMAGE -sUSE_SDL_TTF -sUSE_SDL_MIXER")
    set(EMCC_LINKER_FLAGS "--preload-file=assets -s TOTAL_MEMORY=512MB -s EXPORTED_RUNTIME_METHODS='[\"ccall\", \"cwrap\"]' -s ALLOW_MEMORY_GROWTH=1")

    add_executable(${TARGET_NAME}
        ${COMMON_SRC}
        ${PROJECT_SRC}
    )

    set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS ${EMCC_LINKER_FLAGS})
else()
    add_executable(${TARGET_NAME}
        ${COMMON_SRC}
        ${PROJECT_SRC}
    )

    find_package(Threads REQUIRED)

    if(PLATFORM STREQUAL "windows")
        target_link_libraries(${TARGET_NAME}
            Threads::Threads
        )
    elseif(PLATFORM STREQUAL "linux")
        find_package(OpenGL REQUIRED)
        find_package(GLUT REQUIRED)
        find_package(X11 REQUIRED)

        target_link_libraries(${TARGET_NAME}
            OpenGL::GL
            GLUT::GLUT
            Threads::Threads
            ${X11_LIBRARIES}
        )
    endif()
endif()