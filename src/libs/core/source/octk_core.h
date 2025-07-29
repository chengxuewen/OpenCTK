/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present chengxuewen.
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

#ifndef _OCTK_CORE_H
#define _OCTK_CORE_H

#include <stdint.h>

#if defined(__GNUC__) && ((100 * __GNUC__ + __GNUC_MINOR__) > 400)
#   define OCTK_CORE_EXPORT __attribute__((visibility("default")))
#   define OCTK_CORE_IMPORT __attribute__((visibility("default")))
#elif defined(__MINGW32__) || defined(_MSC_VER)
#   define OCTK_CORE_EXPORT __declspec(dllexport)
#   define OCTK_CORE_IMPORT __declspec(dllimport)
#elif defined(__clang__)
#   define OCTK_CORE_EXPORT __attribute__((visibility("default")))
#   define OCTK_CORE_IMPORT __attribute__((visibility("default")))
#endif

#ifndef OCTK_BUILD_STATIC // compiled as a dynamic lib.
#   ifdef OCTK_BUILDING_CORE_LIB    // defined if we are building the lib
#       define OCTK_CORE_C_API OCTK_CORE_EXPORT
#   else
#       define OCTK_CORE_C_API OCTK_CORE_IMPORT
#   endif
#else // compiled as a static lib.
#   define OCTK_CORE_C_API
#endif

#ifdef _WIN32
#   ifndef OCTK_NO_STDCALL
#       define OCTK_CORE_STDCALL __stdcall
#   else
#       define OCTK_CORE_STDCALL
#   endif
#else // not WIN32
#   define OCTK_CORE_STDCALL
#endif

#ifdef  __cplusplus
extern "C" {
#endif

typedef enum 
{
	OCTK_LOG_LEVEL_TRACE = 0,
	OCTK_LOG_LEVEL_DEBUG = 1,
	OCTK_LOG_LEVEL_INFO = 2,
	OCTK_LOG_LEVEL_WARNING = 3,
	OCTK_LOG_LEVEL_ERROR = 4,
	OCTK_LOG_LEVEL_CRITICAL = 5,
	OCTK_LOG_LEVEL_FATAL = 6
} octk_log_level_t;

typedef struct
{
    octk_log_level_t level;
    const char *filePath;
    const char *fileName;
    const char *funcName;
    int line;
} octk_log_context_t;

typedef void(OCTK_CORE_STDCALL *octk_log_callback_func)(const char *name, 
                                                          octk_log_context_t context, 
                                                          const char *message);

OCTK_CORE_C_API void octk_logger_set_level_enabled(int id, 
                                                     octk_log_level_t level, 
                                                     bool enable);

OCTK_CORE_C_API bool octk_logger_is_level_enabled(int id, 
                                                    octk_log_level_t level);
    
OCTK_CORE_C_API void octk_logger_switch_level(int id, 
                                                octk_log_level_t level);

OCTK_CORE_C_API void octk_init_all_loggers(octk_log_level_t level, 
                                             octk_log_callback_func func, 
                                             bool unique_ownership);

OCTK_CORE_C_API void octk_init_logger(int id, 
                                        octk_log_level_t level, 
                                        octk_log_callback_func func, 
                                        bool unique_ownership);

OCTK_CORE_C_API int octk_logger_id(const char *name);


OCTK_CORE_C_API const char *octk_core_version(void);

OCTK_CORE_C_API void octk_core_init(void);

#ifdef  __cplusplus
}
#endif

#endif // _OCTK_CORE_H
