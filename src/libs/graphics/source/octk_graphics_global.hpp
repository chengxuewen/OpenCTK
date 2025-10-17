#ifndef _OCTK_GRAPHICS_GLOBAL_HPP
#define _OCTK_GRAPHICS_GLOBAL_HPP

#include <octk_global.hpp>
#include <octk_graphics_config.hpp>

/***********************************************************************************************************************
   OpenCTK Compiler specific cmds for export and import code to DLL
***********************************************************************************************************************/
#ifdef OCTK_BUILD_SHARED_GRAPHICS     // compiled as a dynamic lib.
#    ifdef OCTK_BUILDING_GRAPHICS_LIB // defined if we are building the lib
#        define OCTK_GRAPHICS_API OCTK_DECLARE_EXPORT
#    else
#        define OCTK_GRAPHICS_API OCTK_DECLARE_IMPORT
#    endif
#    define OCTK_GRAPHICS_HIDDEN OCTK_DECLARE_HIDDEN
#else // compiled as a static lib.
#    define OCTK_GRAPHICS_API
#    define OCTK_GRAPHICS_HIDDEN
#endif

#endif // _OCTK_GRAPHICS_GLOBAL_HPP
