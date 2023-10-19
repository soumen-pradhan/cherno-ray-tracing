## TODO
* Add Unix support
* Use Win32 API and Libs correctly for WL_DIST/Release
* Restructure Walnut headers

## Build Modes

| Cherno     | CMake          |
|------------|----------------|
| Debug      | Debug          |
| Release    | RelWithDebInfo |
| Dist       | Release        |

## Libs
Walnut - https://github.com/StudioCherno/Walnut.git commit 3b8e414

## Note

Add these environment variables before running the executable.
Problem may occur due to Nvidia dGPU and AMD iGPU.

* [github issue thread](https://github.com/KhronosGroup/Vulkan-Loader/issues/552)
* [stackoverflow thread](https://stackoverflow.com/questions/68109171/vkenumeratephysicaldevices-not-finding-all-gpus)

```
DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1 = 1
DISABLE_LAYER_NV_OPTIMUS_1 = 1
```

## Resources
* [How to turn on dynamic runtime](https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_RUNTIME_LIBRARY.html#variable:CMAKE_MSVC_RUNTIME_LIBRARY)
* [CMake FindVulkan](https://cmake.org/cmake/help/latest/module/FindVulkan.html)
* [CMake Generator Expressions](
https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#genex:CONFIG)
