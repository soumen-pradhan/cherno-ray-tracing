Add these flags before running the executable.
Problem may occur due to Nvidia dGPU and AMD iGPU.

https://github.com/KhronosGroup/Vulkan-Loader/issues/552

* DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1 = 1
* DISABLE_LAYER_NV_OPTIMUS_1 = 1

```
$env:DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1="1"; $env:DISABLE_LAYER_NV_OPTIMUS_1="1"
```
