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
#include "octk_imgui_application_sdlopengl3.hpp"

#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#    include <SDL3/SDL_opengles2.h>
#else
#    include <SDL3/SDL_opengl.h>
#endif

#ifdef __EMSCRIPTEN__
#    include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

OCTK_BEGIN_NAMESPACE

OCTK_IMGUI_REGISTER_APPLICATION(ImguiApplicationSDLOpenGL3, constants::kImguiApplicationSDLOpenGL3)

class ImguiApplicationSDLOpenGL3Private : public ImGuiApplicationPrivate
{
    OCTK_DECLARE_PUBLIC(ImguiApplicationSDLOpenGL3)
    OCTK_DISABLE_COPY_MOVE(ImguiApplicationSDLOpenGL3Private)
public:
    explicit ImguiApplicationSDLOpenGL3Private(ImguiApplicationSDLOpenGL3 *p);
    virtual ~ImguiApplicationSDLOpenGL3Private();

    SDL_FColor mSDLClearColor{mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w};

    ImGuiIO *mImGuiIO{nullptr};
    SDL_Window *mSDLWindow{nullptr};
    SDL_GLContext mSDLGLContext{nullptr};
};

ImguiApplicationSDLOpenGL3Private::ImguiApplicationSDLOpenGL3Private(ImguiApplicationSDLOpenGL3 *p)
    : ImGuiApplicationPrivate(p)
{
}
ImguiApplicationSDLOpenGL3Private::~ImguiApplicationSDLOpenGL3Private() { }

ImguiApplicationSDLOpenGL3::ImguiApplicationSDLOpenGL3(const Properties &properties)
    : ImGuiApplication(new ImguiApplicationSDLOpenGL3Private(this), properties)
{
}

ImguiApplicationSDLOpenGL3::~ImguiApplicationSDLOpenGL3() { }

bool ImguiApplicationSDLOpenGL3::init()
{
    OCTK_D(ImguiApplicationSDLOpenGL3);
    if (d->mInitOnceFlag.enter())
    {
        auto initScopeGuard = utils::makeScopeGuard([&]() { d->mInitOnceFlag.leave(); });

        const auto result = SDL::init();
        if (!result.has_value())
        {
            d->setError(result.error().data());
            return false;
        }
        // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100 (WebGL 1.0)
        const char *glsl_version = "#version 100";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
        // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
        const char *glsl_version = "#version 300 es";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
        // GL 3.2 Core + GLSL 150
        const char *glsl_version = "#version 150";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
        // GL 3.0 + GLSL 130
        const char *glsl_version = "#version 130";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

        // Create window with graphics context
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
        SDL_WindowFlags window_flags =
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        d->mSDLWindow = SDL_CreateWindow("Dear ImGui SDL3+OpenGL3 example",
                                         (int)(1280 * main_scale),
                                         (int)(720 * main_scale),
                                         window_flags);
        if (!d->mSDLWindow)
        {
            printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
            return -1;
        }
        d->mSDLGLContext = SDL_GL_CreateContext(d->mSDLWindow);
        if (!d->mSDLGLContext)
        {
            printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
            return -1;
        }

        SDL_GL_MakeCurrent(d->mSDLWindow, d->mSDLGLContext);
        SDL_GL_SetSwapInterval(1); // Enable vsync
        SDL_SetWindowPosition(d->mSDLWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(d->mSDLWindow);

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
        ImGui_ImplSDL3_InitForOpenGL(d->mSDLWindow, d->mSDLGLContext);
        ImGui_ImplOpenGL3_Init(glsl_version);

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

bool ImguiApplicationSDLOpenGL3::exec()
{
    OCTK_D(ImguiApplicationSDLOpenGL3);
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
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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
        ImGui_ImplOpenGL3_NewFrame();
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
        glViewport(0, 0, (int)d->mImGuiIO->DisplaySize.x, (int)d->mImGuiIO->DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w,
                     clear_color.y * clear_color.w,
                     clear_color.z * clear_color.w,
                     clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(d->mSDLWindow);
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

void ImguiApplicationSDLOpenGL3::destroy()
{
    OCTK_D(ImguiApplicationSDLOpenGL3);
    if (d->mInitOnceFlag.isDone() && d->mInitSuccess.load())
    {
        if (d->mDestroyOnceFlag.enter())
        {
            // Cleanup
            // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();

            SDL_GL_DestroyContext(d->mSDLGLContext);
            SDL_DestroyWindow(d->mSDLWindow);
            SDL_Quit();

            ImGuiApplication::destroy();
            d->mDestroyOnceFlag.leave();
        }
    }
}

StringView ImguiApplicationSDLOpenGL3::typeName() const { return constants::kImguiApplicationSDLOpenGL3; }

OCTK_END_NAMESPACE