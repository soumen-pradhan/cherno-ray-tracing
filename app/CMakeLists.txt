cmake_minimum_required(VERSION 3.24)
project(ray-app VERSION 0.1.0 LANGUAGES CXX)

# Disable static runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

set(WEB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/web)
if(EMSCRIPTEN)
    message(STATUS "Emscripten detected")
else()
    message(FATAL_ERROR "Emscripten not found. Add the toolchain file.")
endif()

add_executable(${PROJECT_NAME} "")

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

target_sources(${PROJECT_NAME}
    PRIVATE
        src/main.cpp
)

set(WEB_RESOURCES
    ${WEB_DIR}/em-template.html
    ${WEB_DIR}/em-pre.js
    ${WEB_DIR}/em-post.js
    ${WEB_DIR}/em-extern-pre.js
    ${WEB_DIR}/em-extern-post.js
)

set_source_files_properties(src/main.cpp
    PROPERTIES
        OBJECT_DEPENDS "${WEB_RESOURCES}"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        imgui::imgui
        idbfs.js # persistent storage
)

target_compile_options(${PROJECT_NAME} PRIVATE
    # -v
    -sDISABLE_EXCEPTION_CATCHING=1

    -Wall
    # may fail once for -Wextra. Just build again.
    -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
    # -Wpedantic
    # -Werror
    -Wformat

    # -Os
)

# from settings.js
target_link_options(${PROJECT_NAME} PRIVATE
    -sASSERTIONS=1
    -sUSE_CLOSURE_COMPILER=1
    -sENVIRONMENT=web
    -sDISABLE_EXCEPTION_CATCHING=1
    # -sASYNCIFY
    -sMODULARIZE=1
    # -sEXPORT_ES6=1
    -sEXPORT_NAME=initEmModule # requires a custom shell-file
    # -sWASM=1
    # -sALLOW_MEMORY_GROWTH=1
    -sEXIT_RUNTIME=0
    -sFILESYSTEM=1
)

# from emcc flags
target_link_options(${PROJECT_NAME} PRIVATE
    -v
    # --closure=1 "SHELL:--closure-args='-O ADVANCED'" # Unstable. Only for Release.
    --emrun # when using emrun to serve the site.
    --shell-file ${WEB_DIR}/em-template.html
    --extern-pre-js ${WEB_DIR}/em-extern-pre.js
    --pre-js ${WEB_DIR}/em-pre.js
    --post-js ${WEB_DIR}/em-post.js
    --extern-post-js ${WEB_DIR}/em-extern-post.js
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
       SUFFIX ".html"
)
