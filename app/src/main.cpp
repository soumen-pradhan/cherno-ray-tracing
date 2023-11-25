#include <fstream>
#include <iostream>
#include <stdio.h>

#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>
#include <webgpu/webgpu_cpp.h>

#ifdef __EMSCRIPTEN__ // should be defined always

#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>

namespace em = emscripten;

#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"

// globals. Set only once.
WGPUInstance g_Instance = nullptr;
WGPUDevice g_Device = nullptr;
WGPUTextureFormat g_PreferredFmt = WGPUTextureFormat_RGBA8Unorm;

// Forward declarations
static void errorGLFW(int error, const char* description);
static void errorWGPU(WGPUErrorType error_type, const char* message, void* userdata);

typedef void (*DeviceCallback)(WGPUDevice);
static void initWGPU(DeviceCallback);
static void useDevice(WGPUDevice);
static void mainLoopStep(void* window);

struct MainLoopData {
    GLFWwindow* window = nullptr;
    WGPUSurface surface = nullptr;
};

#define LOG(x) printf("[dbg] " #x "\n")

EM_JS(void, _download, (char const* filename, char const* mime_type, void const* buffer, size_t buffer_size), {
    const data = new Blob(
        [new Uint8Array(Module["HEAPU8"].buffer, buffer, buffer_size)],
        { type: UTF8ToString(mime_type) });

    const link = document.createElement("a");
    link.download = UTF8ToString(filename);
    link.href = URL.createObjectURL(data);
    link.click();
});

void download(const std::string& filename, const std::string& mime_type, std::string_view buffer)
{
    _download(filename.c_str(), mime_type.c_str(), buffer.data(), buffer.size());
}

void append_file()
{
    LOG(write_file);
    std::string data;
    std::ofstream fs("offline/test.txt", std::ios::app);
    if (fs) {
        data = "O Goddess! Sing the wrath of Peleus' son,\n";
        fs << data;
    }
    EM_ASM(
        FS.syncfs(function(err) {
            console.log("FS.syncFs: Write synced.", err);
        }););
}

void read_file()
{
    LOG(read_file);
    std::string data;
    std::ifstream fs("offline/test.txt", std::ios::in);
    if (fs) {
        for (int i = 0; fs; i++) {
            std::getline(fs, data);
            std::cout << i << ": " << data << "\n";
        }
    }
}

int main()
{
    LOG(start_ap);

    glfwSetErrorCallback(errorGLFW);
    if (!glfwInit()) {
        return 1;
    }

    LOG(glfw_init);

    g_Instance = wgpuCreateInstance(nullptr);
    if (!g_Instance) {
        return 1;
    }

    LOG(wgpu_instance);

    initWGPU(useDevice);
}

static void errorGLFW(int error, const char* description)
{
    printf("GLFW Error %d: %s\n", error, description);
}

static void errorWGPU(WGPUErrorType error_type, const char* message, void* userdata)
{
    // clang-format off
    const char* error_type_label = [&] {
        switch (error_type) {
            case WGPUErrorType_Validation:  return "Validation";
            case WGPUErrorType_OutOfMemory: return "Out of memory";
            case WGPUErrorType_Unknown:     return "Unknown";
            case WGPUErrorType_DeviceLost:  return "Device lost";
            default:                        return "Unknown";
        };
    }();
    // clang-format on

    printf("%s error: %s\n", error_type_label, message);
}

void initWGPU(DeviceCallback callback)
{
    wgpuInstanceRequestAdapter(
        g_Instance,
        nullptr,
        [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, void* userdata) {
            if (status != WGPURequestAdapterStatus_Success) {
                exit(1);
            }

            wgpuAdapterRequestDevice(
                adapter,
                nullptr,
                [](WGPURequestDeviceStatus status, WGPUDevice device, const char* message, void* userdata) {
                    if (status != WGPURequestDeviceStatus_Success) {
                        exit(1);
                    }

                    reinterpret_cast<DeviceCallback>(userdata)(device);
                },
                userdata);
        },
        reinterpret_cast<void*>(callback));
}

static void useDevice(WGPUDevice device)
{
    if (!device) {
        exit(1);
    }

    g_Device = device;

    LOG(wgpu_device);

    wgpuDeviceSetUncapturedErrorCallback(g_Device, errorWGPU, nullptr);

    WGPUSurface surfaceRelease;

    {
        // Use C++ wrapper due to misbehavior in Emscripten.
        // Some offset computation for wgpuInstanceCreateSurface in JavaScript
        // seem to be inline with struct alignments in the C++ structure
        wgpu::SurfaceDescriptorFromCanvasHTMLSelector html_surface_desc = {};
        html_surface_desc.selector = "#canvas";

        wgpu::SurfaceDescriptor surface_desc = {};
        surface_desc.nextInChain = &html_surface_desc;

        wgpu::Instance instance = wgpuCreateInstance(nullptr);
        wgpu::Surface surface = instance.CreateSurface(&surface_desc);
        wgpu::Adapter adapter = {};
        g_PreferredFmt = (WGPUTextureFormat)surface.GetPreferredFormat(adapter);
        surfaceRelease = surface.Release();
    }

    int cWt = 1280, cHt = 720;
    emscripten_get_canvas_element_size("#canvas", &cWt, &cHt);
    printf("em-canvas %d x %d\n", cWt, cHt);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(cWt, cHt, "Dear ImGui GLFW+WebGPU example", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        exit(1);
    }

    glfwShowWindow(window);

    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        printf("glfwSetWindow %d x %d\n", width, height);
    });

    LOG(glfw_show_window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplWGPU_Init(device, 3, g_PreferredFmt, WGPUTextureFormat_Undefined);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Emscripten allows preloading a file or folder to be accessible at runtime. See Makefile for details.
    // io.Fonts->AddFontDefault();
#ifndef IMGUI_DISABLE_FILE_FUNCTIONS
    io.Fonts->AddFontFromFileTTF("fonts/segoeui.ttf", 18.0f);
    // io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("fonts/Cousine-Regular.ttf", 15.0f);
    // io.Fonts->AddFontFromFileTTF("fonts/ProggyTiny.ttf", 10.0f);
    // ImFont* font = io.Fonts->AddFontFromFileTTF("fonts/ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    // IM_ASSERT(font != nullptr);
#endif

    LOG(imgui_setup);

    // This function will directly return and exit the main function.
    // Make sure that no required objects get cleaned up.
    // This way we can use the browsers 'requestAnimationFrame' to control the rendering.

    MainLoopData* data = new MainLoopData {
        .window = window,
        .surface = surfaceRelease,
    };

    emscripten_set_main_loop_arg(mainLoopStep, data, 0, false);

    LOG(main_loop);
}

static void mainLoopStep(void* _data)
{
    MainLoopData* data = static_cast<MainLoopData*>(_data);

    ImGuiIO& io = ImGui::GetIO();

    glfwPollEvents();

    int width, height;
    glfwGetFramebufferSize(data->window, &width, &height);

    static WGPUSwapChain swapChain = nullptr;
    static int swapChainWt = 0, swapChainHt = 0;

    // React to changes in screen size
    if (width != swapChainWt && height != swapChainHt) {
        ImGui_ImplWGPU_InvalidateDeviceObjects();
        if (swapChain) {
            wgpuSwapChainRelease(swapChain);
        }
        swapChainWt = width;
        swapChainHt = height;

        WGPUSwapChainDescriptor swapChainDesc = {
            .label = "swapChain for surface",
            .usage = WGPUTextureUsage_RenderAttachment,
            .format = g_PreferredFmt,
            .width = (uint32_t)width,
            .height = (uint32_t)height,
            .presentMode = WGPUPresentMode_Fifo,
        };

        swapChain = wgpuDeviceCreateSwapChain(g_Device, data->surface, &swapChainDesc);
        ImGui_ImplWGPU_CreateDeviceObjects();
    }

    // Start the Dear ImGui frame
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    // Our state
    // (we use static, which essentially makes the variable globals, as a convenience to keep the example code easy to follow)
    static bool show_demo_window = true;
    static bool show_another_window = false;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button")) { // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        if (ImGui::Button("Save")) {
            download("download_text.txt", "text/plain",
                "O Goddess! Sing the wrath of Peleus' son,\n");
        }

        if (ImGui::Button("Append")) {
            append_file();
        }
        ImGui::SameLine();
        if (ImGui::Button("Read")) {
            read_file();
        }

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window) {
        ImGui::Begin("Another Window", &show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me")) {
            show_another_window = false;
        }
        ImGui::End();
    }

    // Rendering
    ImGui::Render();

    WGPURenderPassColorAttachment colorAttachments = {
        .view = wgpuSwapChainGetCurrentTextureView(swapChain),
        .loadOp = WGPULoadOp_Clear,
        .storeOp = WGPUStoreOp_Store,
        .clearValue = {
            clear_color.x * clear_color.w,
            clear_color.y * clear_color.w,
            clear_color.z * clear_color.w,
            clear_color.w },
    };

    WGPURenderPassDescriptor renderPassDesc = {
        .label = "renderPass",
        .colorAttachmentCount = 1,
        .colorAttachments = &colorAttachments,
        .depthStencilAttachment = nullptr,
    };

    WGPUCommandEncoderDescriptor enc_desc = {};
    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(g_Device, &enc_desc);

    WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass);
    wgpuRenderPassEncoderEnd(pass);

    WGPUCommandBufferDescriptor cmdBufferDesc = {};
    WGPUCommandBuffer cmdBuffer = wgpuCommandEncoderFinish(encoder, &cmdBufferDesc);
    WGPUQueue queue = wgpuDeviceGetQueue(g_Device);
    wgpuQueueSubmit(queue, 1, &cmdBuffer);
}
