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
#include "octk_imgui_application_sdlrenderer3.hpp"

#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <absl/strings/str_format.h>
#include <absl/strings/internal/str_format/parser.h>

#ifdef __EMSCRIPTEN__
#    include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

OCTK_BEGIN_NAMESPACE

OCTK_IMGUI_REGISTER_APPLICATION(ImGuiApplicationSDLRenderer3, constants::kImGuiApplicationSDLRenderer3)

class ImGuiApplicationSDLRenderer3Image : public ImGuiImage
{
public:
    ImGuiApplicationSDLRenderer3Image(SDL_PixelFormat pixelFormat, Format format, float width, float height)
        : ImGuiImage(format, width, height)
        , mSDLPixelFormat(pixelFormat)
    {
    }
    ~ImGuiApplicationSDLRenderer3Image() override { }

    void init(void *data) override
    {
        if (!mSDLTexture)
        {
            auto texture = SDL_CreateTexture(static_cast<SDL_Renderer *>(data),
                                             mSDLPixelFormat,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             this->width(),
                                             this->height());
            if (texture)
            {
                mSDLTexture = texture;
            }
            else
            {
                mInitResult = std::string("SDL_CreateTexture failed:") + SDL_GetError();
            }
        }
    }
    void destroy() override
    {
        if (mSDLTexture)
        {
            SDL_DestroyTexture(mSDLTexture);
        }
    }
    size_t textureId() override { return reinterpret_cast<size_t>(mSDLTexture); }
    void updateTexture() override
    {
        if (mSDLTexture)
        {
            SDL_UpdateTexture(mSDLTexture, nullptr, this->frameData(), this->pitchSize());
        }
    }

private:
    SDL_Texture *mSDLTexture{nullptr};
    SDL_PixelFormat mSDLPixelFormat{SDL_PIXELFORMAT_UNKNOWN};
};

class ImGuiApplicationSDLRenderer3Private : public ImGuiApplicationPrivate
{
    OCTK_DECLARE_PUBLIC(ImGuiApplicationSDLRenderer3)
    OCTK_DISABLE_COPY_MOVE(ImGuiApplicationSDLRenderer3Private)
public:
    explicit ImGuiApplicationSDLRenderer3Private(ImGuiApplicationSDLRenderer3 *p);
    virtual ~ImGuiApplicationSDLRenderer3Private();

    SDL_FColor mSDLClearColor{mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w};

    ImGuiIO *mImGuiIO{nullptr};
    SDL_Window *mSDLWindow{nullptr};
    SDL_Renderer *mSDLRenderer{nullptr};
};

ImGuiApplicationSDLRenderer3Private::ImGuiApplicationSDLRenderer3Private(ImGuiApplicationSDLRenderer3 *p)
    : ImGuiApplicationPrivate(p)
{
}
ImGuiApplicationSDLRenderer3Private::~ImGuiApplicationSDLRenderer3Private() { }

ImGuiApplicationSDLRenderer3::ImGuiApplicationSDLRenderer3(const Properties &properties)
    : ImGuiApplication(new ImGuiApplicationSDLRenderer3Private(this), properties)
{
}

ImGuiApplicationSDLRenderer3::~ImGuiApplicationSDLRenderer3() { }

bool ImGuiApplicationSDLRenderer3::init()
{
    OCTK_D(ImGuiApplicationSDLRenderer3);
    if (d->mInitOnceFlag.enter())
    {
        auto initScopeGuard = utils::makeScopeGuard([&]() { d->mInitOnceFlag.leave(); });

        const auto result = SDL::init();
        if (!result.has_value())
        {
            d->setError(result.error().data());
            return false;
        }

        // Create window with SDL_Renderer graphics context
        float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
        SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        d->mSDLWindow = SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example",
                                         (int)(1280 * main_scale),
                                         (int)(720 * main_scale),
                                         window_flags);
        if (!d->mSDLWindow)
        {
            printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
            return -1;
        }
        d->mSDLRenderer = SDL_CreateRenderer(d->mSDLWindow, nullptr);
        SDL_SetRenderVSync(d->mSDLRenderer, 1);
        if (!d->mSDLRenderer)
        {
            SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
            return -1;
        }
        SDL_SetWindowPosition(d->mSDLWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(d->mSDLWindow);
        d->initImages(d->mSDLRenderer);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        d->mImGuiIO = &ImGui::GetIO();
        d->mImGuiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        d->mImGuiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        d->mImGuiIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Setup Dear ImGui style
        ImGui::StyleColorsLight();

        // Setup scaling
        ImGuiStyle &style = ImGui::GetStyle();
        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.ScaleAllSizes(main_scale);
        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
        style.FontScaleDpi = main_scale;

        // Setup Platform/Renderer backends
        ImGui_ImplSDL3_InitForSDLRenderer(d->mSDLWindow, d->mSDLRenderer);
        ImGui_ImplSDLRenderer3_Init(d->mSDLRenderer);

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
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

bool ImGuiApplicationSDLRenderer3::exec()
{
    OCTK_D(ImGuiApplicationSDLRenderer3);
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
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!d->mFinished.load())
#endif
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
        ImGui_ImplSDLRenderer3_NewFrame();
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
        SDL_SetRenderScale(d->mSDLRenderer,
                           d->mImGuiIO->DisplayFramebufferScale.x,
                           d->mImGuiIO->DisplayFramebufferScale.y);
        SDL_SetRenderDrawColorFloat(d->mSDLRenderer,
                                    d->mClearColor.x,
                                    d->mClearColor.y,
                                    d->mClearColor.z,
                                    d->mClearColor.w);
        SDL_RenderClear(d->mSDLRenderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), d->mSDLRenderer);
        SDL_RenderPresent(d->mSDLRenderer);
    }
    // Quit custom
    {
        SpinLock::Locker locker(d->mCallbackSpinLock);
        if (d->mQuitFunction)
        {
            d->mQuitFunction();
        }
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif
    return true;
}

void ImGuiApplicationSDLRenderer3::destroy()
{
    OCTK_D(ImGuiApplicationSDLRenderer3);
    if (d->mInitOnceFlag.isDone() && d->mInitSuccess.load())
    {
        if (d->mDestroyOnceFlag.enter())
        {
            // Cleanup
            d->destroyImages();
            // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
            ImGui_ImplSDLRenderer3_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();

            SDL_DestroyRenderer(d->mSDLRenderer);
            SDL_DestroyWindow(d->mSDLWindow);
            SDL_Quit();

            ImGuiApplication::destroy();
            d->mDestroyOnceFlag.leave();
        }
    }
}

StringView ImGuiApplicationSDLRenderer3::typeName() const { return constants::kImGuiApplicationSDLRenderer3; }

ImGuiImage::SharedPtr
ImGuiApplicationSDLRenderer3::createImage(ImGuiImage::Format format, const Binary &binary, int width, int height)
{
    OCTK_D(ImGuiApplicationSDLRenderer3);
    SDL_PixelFormat pixelFormat = SDL_PIXELFORMAT_UNKNOWN;
    switch (format)
    {
        case ImGuiImage::Format::RGB24: pixelFormat = SDL_PIXELFORMAT_RGB24; break;
        case ImGuiImage::Format::RGBA32: pixelFormat = SDL_PIXELFORMAT_RGBA32; break;
        default: break;
    }
    auto image = std::make_shared<ImGuiApplicationSDLRenderer3Image>(pixelFormat, format, width, height);
    image->setFrameData(binary.data());
    d->mImagesSet.insert(image);
    return image;
}

OCTK_END_NAMESPACE