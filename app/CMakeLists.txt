cmake_minimum_required(VERSION 3.24)
project(ray-app VERSION 0.1.0 LANGUAGES CXX)

# Disable static runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

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

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Walnut::Walnut
)

# Ask compiler to be more demanding
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /WX /permissive->
    $<$<CXX_COMPILER_ID:GCC>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:CLANG>:-Wall -Wextra -Wpedantic -Werror>
)
