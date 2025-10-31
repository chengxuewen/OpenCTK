#ifndef _OCTK_SERVICE_GLOBAL_HPP
#define _OCTK_SERVICE_GLOBAL_HPP

#include <octk_global.hpp>
#include <octk_service_config.hpp>

/***********************************************************************************************************************
   OpenCTK Compiler specific cmds for export and import code to DLL
***********************************************************************************************************************/
#ifdef OCTK_BUILD_SHARED_SERVICE     // compiled as a dynamic lib.
#    ifdef OCTK_BUILDING_SERVICE_LIB // defined if we are building the lib
#        define OCTK_SERVICE_API OCTK_DECLARE_EXPORT
#    else
#        define OCTK_SERVICE_API OCTK_DECLARE_IMPORT
#    endif
#    define OCTK_SERVICE_HIDDEN OCTK_DECLARE_HIDDEN
#else // compiled as a static lib.
#    define OCTK_SERVICE_API
#    define OCTK_SERVICE_HIDDEN
#endif

#endif // _OCTK_SERVICE_GLOBAL_HPP
