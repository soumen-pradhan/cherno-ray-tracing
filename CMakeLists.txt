cmake_minimum_required(VERSION 3.22)
project(Walnut VERSION 0.1.0 LANGUAGES CXX)

# Works without this as well. Maybe because, I have VK_SDK_PATH, VULKAN_SDK
# env var defined. Also imgui built with vulkan-bindings.
find_package(Vulkan REQUIRED)

add_subdirectory(deps/Walnut)
add_subdirectory(app)
