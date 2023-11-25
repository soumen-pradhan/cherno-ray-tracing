cmake_minimum_required(VERSION 3.22)
project(cherno-ray-tracing VERSION 0.1.0 LANGUAGES CXX)

add_subdirectory(deps/glm)
add_subdirectory(deps/stb)
add_subdirectory(deps/imgui)

# add_subdirectory(deps/walnut/Walnut)

add_subdirectory(app)
