#ifndef _OCTK_NETWORK_GLOBAL_HPP
#define _OCTK_NETWORK_GLOBAL_HPP

#include <octk_global.hpp>
#include <octk_network_config.hpp>

/***********************************************************************************************************************
   OpenCTK Compiler specific cmds for export and import code to DLL
***********************************************************************************************************************/
#ifdef OCTK_BUILD_SHARED_NETWORK // compiled as a dynamic lib.
#   ifdef OCTK_BUILDING_NETWORK_LIB // defined if we are building the lib
#       define OCTK_NETWORK_API OCTK_DECLARE_EXPORT
#   else
#       define OCTK_NETWORK_API OCTK_DECLARE_IMPORT
#   endif
#   define OCTK_NETWORK_HIDDEN OCTK_DECLARE_HIDDEN
#else // compiled as a static lib.
#   define OCTK_NETWORK_API
#   define OCTK_NETWORK_HIDDEN
#endif

#endif // _OCTK_NETWORK_GLOBAL_HPP
