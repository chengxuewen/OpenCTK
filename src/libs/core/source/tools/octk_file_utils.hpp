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

#ifndef _OCTK_FILE_UTILS_HPP
#define _OCTK_FILE_UTILS_HPP

#include <octk_string_view.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

namespace detail
{
#ifdef OCTK_OS_MAC
OCTK_CORE_API void GetNSExecutablePath(std::string *path);
#else
{ OCTK_UNUSED(path); }
#endif
} // namespace detail

namespace utils
{
OCTK_CORE_API std::string DirName(StringView path);
OCTK_CORE_API bool CreateDir(StringView directory_name);

OCTK_CORE_API std::string OutputPath();
OCTK_CORE_API std::string WorkingDir();
OCTK_CORE_API std::string ProjectRootPath();

OCTK_CORE_API std::string ResourcePath(const char *name, const char *extension);
static OCTK_FORCE_INLINE std::string ResourcePath(StringView name, StringView extension)
{
    return ResourcePath(name.data(), extension.data());
}

// Generates an empty file with a unique name in the specified directory and
// returns the file name and path.
OCTK_CORE_API std::string TempFilename(const char *dir, const char *prefix);
static OCTK_FORCE_INLINE std::string TempFilename(StringView name, StringView extension)
{
    return TempFilename(name.data(), extension.data());
}
} // namespace utils
OCTK_END_NAMESPACE

#endif // _OCTK_FILE_UTILS_HPP
