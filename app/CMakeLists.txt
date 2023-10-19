cmake_minimum_required(VERSION 3.22)
project(cherno-raytracer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(fmt CONFIG REQUIRED)

# Disable static runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

include(CTest)
enable_testing()

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt::fmt
    Walnut
)

# Ask a compiler to be more demanding
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W3 /WX /permissive->
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)