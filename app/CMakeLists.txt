cmake_minimum_required(VERSION 3.22)
project(cherno-raytracer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(fmt CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Disable static runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

include(CTest)
enable_testing()

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    ../deps/Walnut/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt::fmt
    imgui::imgui
    Walnut
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        WL_PLATFORM_WINDOWS
    )
endif()

# Ask a compiler to be more demanding
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
