//
// Created by cxw on 25-8-25.
//

#ifndef _OCTK_SDL_P_HPP
#define _OCTK_SDL_P_HPP

#include <octk_imgui_global.hpp>
#include <octk_expected.hpp>
#include <octk_logging.hpp>

#include <SDL3/SDL.h>

#include <thread>

OCTK_BEGIN_NAMESPACE

class SDL
{
public:
    static Expected<bool, StringView> init()
    {
        static std::once_flag onceFlag;
        static std::string errstr;
        std::call_once(onceFlag,
                       [&]()
                       {
                           if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
                           {
                               errstr = std::string("SDL_Init failed:") + SDL_GetError();
                           }
                       });
        return errstr.empty() ? Expected<bool, StringView>{true} : utils::makeUnexpected(errstr);
    }
};

OCTK_END_NAMESPACE

#endif // _OCTK_SDL_P_HPP
