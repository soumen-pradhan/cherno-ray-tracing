cmake_minimum_required(VERSION 3.14)
project(cherno-walnut VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(fmt CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

include(CTest)
enable_testing()

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt::fmt
    imgui::imgui
)

# Ask a compiler to be more demanding
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
