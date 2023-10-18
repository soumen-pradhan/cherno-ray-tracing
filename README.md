## Build Modes

| Cherno     | CMake          |
|------------|----------------|
| Debug      | Debug          |
| Production | RelWithDebInfo |
| Dist       | Release        |

## Note

Add these flags before running the executable.
Problem may occur due to Nvidia dGPU and AMD iGPU.

[github issue thread](https://github.com/KhronosGroup/Vulkan-Loader/issues/552)

[stackoverflow thread](https://stackoverflow.com/questions/68109171/vkenumeratephysicaldevices-not-finding-all-gpus)

* DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1 = 1
* DISABLE_LAYER_NV_OPTIMUS_1 = 1

```
$env:DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1="1"; $env:DISABLE_LAYER_NV_OPTIMUS_1="1"
```

## Resources
https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_RUNTIME_LIBRARY.html#variable:CMAKE_MSVC_RUNTIME_LIBRARY
https://cmake.org/cmake/help/latest/module/FindVulkan.html
