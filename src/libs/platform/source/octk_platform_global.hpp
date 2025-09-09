#ifndef _OCTK_PLATFORM_GLOBAL_HPP
#define _OCTK_PLATFORM_GLOBAL_HPP

#include <octk_global.hpp>
#include <octk_platform_config.hpp>

/***********************************************************************************************************************
   OpenCTK Compiler specific cmds for export and import code to DLL
***********************************************************************************************************************/
#ifdef OCTK_BUILD_SHARED_PLATFORM     // compiled as a dynamic lib.
#    ifdef OCTK_BUILDING_PLATFORM_LIB // defined if we are building the lib
#        define OCTK_PLATFORM_API OCTK_DECLARE_EXPORT
#    else
#        define OCTK_PLATFORM_API OCTK_DECLARE_IMPORT
#    endif
#    define OCTK_PLATFORM_HIDDEN OCTK_DECLARE_HIDDEN
#else // compiled as a static lib.
#    define OCTK_PLATFORM_API
#    define OCTK_PLATFORM_HIDDEN
#endif

#endif // _OCTK_PLATFORM_GLOBAL_HPP
