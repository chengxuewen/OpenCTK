//
// Created by cxw on 25-8-25.
//

#include <private/octk_imgui_window_p.hpp>
#include "octk_imgui_window_sdlgpu3.hpp"
#include <octk_scope_guard.hpp>

#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

OCTK_BEGIN_NAMESPACE

class ImGuiWindowSDLGPU3Private : public ImGuiWindowPrivate
{
    OCTK_DECLARE_PUBLIC(ImGuiWindowSDLGPU3)
    OCTK_DISABLE_COPY_MOVE(ImGuiWindowSDLGPU3Private)
public:
    explicit ImGuiWindowSDLGPU3Private(ImGuiWindowSDLGPU3 *p);
    virtual ~ImGuiWindowSDLGPU3Private();

    OnceFlag mInitOnceFlag;
    OnceFlag mDestroyOnceFlag;
    SDL_FColor mSDLClearColor{mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w};

    ImGuiIO *mImGuiIO{nullptr};
    SDL_Window *mSDLWindow{nullptr};
    SDL_GPUDevice *mGPUDevice{nullptr};
};

ImGuiWindowSDLGPU3Private::ImGuiWindowSDLGPU3Private(ImGuiWindowSDLGPU3 *p)
    : ImGuiWindowPrivate(p)
{
}
ImGuiWindowSDLGPU3Private::~ImGuiWindowSDLGPU3Private() { }

ImGuiWindowSDLGPU3::ImGuiWindowSDLGPU3()
    : ImGuiWindow(new ImGuiWindowSDLGPU3Private(this))
{
}

ImGuiWindowSDLGPU3::~ImGuiWindowSDLGPU3() { this->destroy(); }

bool ImGuiWindowSDLGPU3::init()
{
    OCTK_D(ImGuiWindowSDLGPU3);
    if (!d->mInitOnceFlag.enter())
    {
        return d->mLastError.empty();
    }
    auto initOnceFlagScopeGuard = utils::makeScopeGuard([&]() { d->mInitOnceFlag.leave(); });

    const auto result = SDL::init();
    if (!result.has_value())
    {
        this->setError(result.error().data());
        return false;
    }
    // Create SDL window graphics context
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    d->mSDLWindow = SDL_CreateWindow("Dear ImGui SDL3+SDL_GPU example",
                                     (int)(1280 * main_scale),
                                     (int)(720 * main_scale),
                                     window_flags);
    if (!d->mSDLWindow)
    {
        this->setError(std::string("SDL_CreateWindow() failed:") + SDL_GetError());
        return false;
    }
    SDL_SetWindowPosition(d->mSDLWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(d->mSDLWindow);


    // Create GPU Device
    d->mGPUDevice =
        SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB,
                            true,
                            nullptr);
    if (!d->mGPUDevice)
    {
        this->setError(std::string("SDL_CreateGPUDevice() failed:") + SDL_GetError());
        return false;
    }

    // Claim window for GPU Device
    if (!SDL_ClaimWindowForGPUDevice(d->mGPUDevice, d->mSDLWindow))
    {
        this->setError(std::string("SDL_ClaimWindowForGPUDevice() failed:") + SDL_GetError());
        return false;
    }
    SDL_SetGPUSwapchainParameters(d->mGPUDevice,
                                  d->mSDLWindow,
                                  SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                  SDL_GPU_PRESENTMODE_VSYNC);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    d->mImGuiIO = &ImGui::GetIO();
    // ImGuiIO &io = ImGui::GetIO(); //TODO::DEL
    // (void)io;
    d->mImGuiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    d->mImGuiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle &style = ImGui::GetStyle();
    /* Bake a fixed style scale. (until we have a solution for dynamic style scaling,
     * changing this requires resetting Style + calling this again) */
    style.ScaleAllSizes(main_scale);
    /* Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary.
     * We leave both here for documentation purpose) */
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLGPU(d->mSDLWindow);
    ImGui_ImplSDLGPU3_InitInfo init_info = {};
    init_info.Device = d->mGPUDevice;
    init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(d->mGPUDevice, d->mSDLWindow);
    init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
    ImGui_ImplSDLGPU3_Init(&init_info);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);
    return true;
}

bool ImGuiWindowSDLGPU3::render()
{
    OCTK_D(ImGuiWindowSDLGPU3);
    if (!this->init())
    {
        return false;
    }

    // Main loop
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    // [If using SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your SDL_AppEvent() function]
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
        {
            d->mLooping.store(false);
        }

        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(d->mSDLWindow))
        {
            d->mLooping.store(false);
        }
    }

    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
    if (SDL_GetWindowFlags(d->mSDLWindow) & SDL_WINDOW_MINIMIZED)
    {
        SDL_Delay(10);
        return true;
        // continue;
    }

    // Start the Dear ImGui frame
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // Draw function call
    {
        SpinLock::Locker locker(d->mDrawFunctionSpinLock);
        if (d->mDrawFunction)
        {
            d->mDrawFunction();
        }
    }

    // Rendering
    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

    /* Acquire a GPU command buffer */
    SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(d->mGPUDevice);

    SDL_GPUTexture *swapchain_texture;
    SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer,
                                          d->mSDLWindow,
                                          &swapchain_texture,
                                          nullptr,
                                          nullptr); // Acquire a swapchain texture

    if (swapchain_texture != nullptr && !is_minimized)
    {
        // This is mandatory: call ImGui_ImplSDLGPU3_PrepareDrawData() to upload the vertex/index buffer!
        ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, command_buffer);

        // Setup and start a render pass
        SDL_GPUColorTargetInfo target_info = {};
        target_info.texture = swapchain_texture;
        target_info.clear_color = d->mSDLClearColor;
        target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        target_info.store_op = SDL_GPU_STOREOP_STORE;
        target_info.mip_level = 0;
        target_info.layer_or_depth_plane = 0;
        target_info.cycle = false;
        SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);

        // Render ImGui
        ImGui_ImplSDLGPU3_RenderDrawData(draw_data, command_buffer, render_pass);

        SDL_EndGPURenderPass(render_pass);
    }

    // Submit the command buffer
    SDL_SubmitGPUCommandBuffer(command_buffer);
    return true;
}

bool ImGuiWindowSDLGPU3::destroy()
{
    OCTK_D(ImGuiWindowSDLGPU3);
    if (!d->mInitOnceFlag.isDone())
    {
        return false;
    }
    if (d->mLooping.load())
    {
        return false;
    }

    if (!d->mDestroyOnceFlag.enter())
    {
        return d->mLastError.empty();
    }
    auto destroyOnceFlagScopeGuard = utils::makeScopeGuard([&]() { d->mDestroyOnceFlag.leave(); });

    // Cleanup
    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
    SDL_WaitForGPUIdle(d->mGPUDevice);
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui::DestroyContext();

    SDL_ReleaseWindowFromGPUDevice(d->mGPUDevice, d->mSDLWindow);
    SDL_DestroyGPUDevice(d->mGPUDevice);
    SDL_DestroyWindow(d->mSDLWindow);

    return true;
}

std::string ImGuiWindowSDLGPU3::typeName() const { return constants::kImGuiWindowSDLGPU3; }

OCTK_END_NAMESPACE