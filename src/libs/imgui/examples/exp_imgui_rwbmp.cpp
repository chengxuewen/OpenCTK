#include <iostream>

#include <SDL3/SDL.h>

#undef main
int main()
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Surface *surface = nullptr;
    SDL_Texture *texture = nullptr;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL初始化失败: " << SDL_GetError() << std::endl;
        return -1;
    }

    int windowWidth = 640;
    int windowHeight = 480;
    window = SDL_CreateWindow("SDL3 BMP示例", windowWidth, windowHeight, SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window)
    {
        std::cerr << "窗口创建失败: " << SDL_GetError() << std::endl;
        return -1;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        std::cerr << "渲染器创建失败: " << SDL_GetError() << std::endl;
        return -1;
    }

    surface = SDL_LoadBMP("test.bmp");
    if (!surface)
    {
        std::cerr << "BMP加载失败: " << SDL_GetError() << std::endl;
        return -1;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        std::cerr << "纹理创建失败: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    bool looping = true;
    while (looping)
    {
        SDL_WaitEvent(&event);
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
            {
                looping = false;
                break;
            }
            case SDL_EVENT_KEY_DOWN:
            {
                if (SDLK_Q == event.key.key)
                {
                    SDL_Event quit_event;
                    quit_event.type = SDL_EVENT_QUIT;
                    SDL_PushEvent(&quit_event);
                }
                break;
            }
            case SDL_EVENT_WINDOW_RESIZED:
            {
                SDL_GetWindowSize(window, &windowWidth, &windowHeight);
                break;
            }
            default: break;
        }
    }

cleanup:
    if (texture)
    {
        SDL_DestroyTexture(texture);
    }
    if (surface)
    {
        SDL_DestroySurface(surface);
    }
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
    if (window)
    {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
    return 0;
}