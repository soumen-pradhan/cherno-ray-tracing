cmake_minimum_required(VERSION 3.22)
project(Walnut VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(Vulkan REQUIRED)

# Disable static runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Add your executable or library
add_library(${PROJECT_NAME} STATIC
    src/Walnut/ImGui/ImGuiBuild.cpp
    src/Walnut/Input/input.cpp
    src/Walnut/Application.cpp
    src/Walnut/Image.cpp
    src/Walnut/Random.cpp
)

# # Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    src
    ${Stb_INCLUDE_DIR}
)

# # Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        imgui::imgui
        glfw
        glm::glm
    PUBLIC
        Vulkan::Vulkan
)

# Windows-specific settings
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        WL_PLATFORM_WINDOWS
    )
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Debug>:WL_DEBUG>
    $<$<CONFIG:RelWithDebInfo>:WL_RELEASE>
    $<$<CONFIG:Release>:WL_DIST>
)

# Ask a compiler to be more demanding
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W3 /WX /permissive->
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
