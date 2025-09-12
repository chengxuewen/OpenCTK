/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
**
** License: MIT License
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the "Software"), to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
** and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial portions
** of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
** TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
** CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
** IN THE SOFTWARE.
**
***********************************************************************************************************************/

#include <private/octk_imgui_application_p.hpp>
#include "octk_imgui_application_sdlgpu3.hpp"

#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

OCTK_BEGIN_NAMESPACE

OCTK_IMGUI_REGISTER_APPLICATION(ImGuiApplicationSDLGPU3, constants::kImGuiApplicationSDLGPU3)

class ImGuiApplicationSDLGPU3Private : public ImGuiApplicationPrivate
{
    OCTK_DECLARE_PUBLIC(ImGuiApplicationSDLGPU3)
    OCTK_DISABLE_COPY_MOVE(ImGuiApplicationSDLGPU3Private)
public:
    explicit ImGuiApplicationSDLGPU3Private(ImGuiApplicationSDLGPU3 *p);
    virtual ~ImGuiApplicationSDLGPU3Private();

    SDL_FColor mSDLClearColor{mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w};

    ImGuiIO *mImGuiIO{nullptr};
    SDL_Window *mSDLWindow{nullptr};
    SDL_GPUDevice *mGPUDevice{nullptr};
};

ImGuiApplicationSDLGPU3Private::ImGuiApplicationSDLGPU3Private(ImGuiApplicationSDLGPU3 *p)
    : ImGuiApplicationPrivate(p)
{
}
ImGuiApplicationSDLGPU3Private::~ImGuiApplicationSDLGPU3Private() { }

ImGuiApplicationSDLGPU3::ImGuiApplicationSDLGPU3(const Properties &properties)
    : ImGuiApplication(new ImGuiApplicationSDLGPU3Private(this), properties)
{
}

ImGuiApplicationSDLGPU3::~ImGuiApplicationSDLGPU3() { }

bool ImGuiApplicationSDLGPU3::init()
{
    OCTK_D(ImGuiApplicationSDLGPU3);
    if (d->mInitOnceFlag.enter())
    {
        auto initScopeGuard = utils::makeScopeGuard([&]() { d->mInitOnceFlag.leave(); });

        const auto result = SDL::init();
        if (!result.has_value())
        {
            d->setError(result.error().data());
            return false;
        }
        // Create SDL window graphics context
        float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
        SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        d->mSDLWindow = SDL_CreateWindow(d->title("Dear ImGui SDL3+SDL_GPU example").c_str(),
                                         (int)(d->width() * main_scale),
                                         (int)(d->height() * main_scale),
                                         window_flags);
        if (!d->mSDLWindow)
        {
            d->setError(std::string("SDL_CreateWindow() failed:") + SDL_GetError());
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
            d->setError(std::string("SDL_CreateGPUDevice() failed:") + SDL_GetError());
            return false;
        }

        // Claim window for GPU Device
        if (!SDL_ClaimWindowForGPUDevice(d->mGPUDevice, d->mSDLWindow))
        {
            d->setError(std::string("SDL_ClaimWindowForGPUDevice() failed:") + SDL_GetError());
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
        d->mImGuiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        d->mImGuiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        d->mImGuiIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        d->mImGuiIO->ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;

        // Setup Dear ImGui style
        ImGui::StyleColorsLight();

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

        return ImGuiApplication::init();
    }
    return this->isReady();
}

bool ImGuiApplicationSDLGPU3::exec()
{
    OCTK_D(ImGuiApplicationSDLGPU3);
    if (!this->init())
    {
        return false;
    }

    // Init callback
    {
        SpinLock::Locker locker(d->mCallbackSpinLock);
        if (d->mInitFunction)
        {
            d->mInitFunction();
        }
    }

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;

    // Main loop
    d->mFinished.store(false);
    while (!d->mFinished.load())
    {
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
                d->quit();
            }
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(d->mSDLWindow))
            {
                d->quit();
            }
        }

        // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
        if (SDL_GetWindowFlags(d->mSDLWindow) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // Draw custom
        {
            SpinLock::Locker locker(d->mCallbackSpinLock);
            if (d->mDrawFunction)
            {
                d->mDrawFunction();
            }
        }

        // Rendering
        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();
        const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

        // Acquire a GPU command buffer
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
    }
    // Quit custom
    {
        SpinLock::Locker locker(d->mCallbackSpinLock);
        if (d->mQuitFunction)
        {
            d->mQuitFunction();
        }
    }
    return true;
}

void ImGuiApplicationSDLGPU3::destroy()
{
    OCTK_D(ImGuiApplicationSDLGPU3);
    if (d->mInitOnceFlag.isDone() && d->mInitSuccess.load())
    {
        if (d->mDestroyOnceFlag.enter())
        {
            // Cleanup
            // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
            SDL_WaitForGPUIdle(d->mGPUDevice);
            ImGui_ImplSDL3_Shutdown();
            ImGui_ImplSDLGPU3_Shutdown();
            ImGui::DestroyContext();

            SDL_ReleaseWindowFromGPUDevice(d->mGPUDevice, d->mSDLWindow);
            SDL_DestroyGPUDevice(d->mGPUDevice);
            SDL_DestroyWindow(d->mSDLWindow);

            ImGuiApplication::destroy();
            d->mDestroyOnceFlag.leave();
        }
    }
}

StringView ImGuiApplicationSDLGPU3::typeName() const { return constants::kImGuiApplicationSDLGPU3; }

OCTK_END_NAMESPACE