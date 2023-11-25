cmake_minimum_required(VERSION 3.22)
project(cherno-ray-tracing VERSION 0.1.0 LANGUAGES CXX)

# Disable building glfw docs, tests, examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/glfw)

add_subdirectory(deps/glm)
add_subdirectory(deps/stb)
add_subdirectory(deps/imgui)

add_subdirectory(deps/walnut/Walnut)

add_subdirectory(app)
