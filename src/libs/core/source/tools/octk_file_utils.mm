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

#include <octk_file_utils.hpp>
#include <octk_checks.hpp>

#import <Foundation/Foundation.h>

#include <mach-o/dyld.h>
#include <dlfcn.h>
#include <stdint.h>
#include <stdlib.h>

OCTK_BEGIN_NAMESPACE

namespace detail
{
void GetNSExecutablePath(std::string *path)
{
    OCTK_DCHECK(path);
    // Executable path can have relative references ("..") depending on
    // how the app was launched.
    uint32_t executable_length = 0;
    _NSGetExecutablePath(NULL, &executable_length);
    OCTK_DCHECK_GT(executable_length, 1u);
    char executable_path[PATH_MAX + 1];
    int rv = _NSGetExecutablePath(executable_path, &executable_length);
    OCTK_DCHECK_EQ(rv, 0);

    char full_path[PATH_MAX];
    if (realpath(executable_path, full_path) == nullptr)
    {
        *path = "";
        return;
    }

    *path = full_path;
}
} // namespace detail
OCTK_END_NAMESPACE
