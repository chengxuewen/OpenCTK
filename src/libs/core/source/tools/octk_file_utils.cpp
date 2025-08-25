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

#if defined(OCTK_OS_WIN)
#   include <direct.h>
#   include <tchar.h>
#   include <windows.h>
#   include <algorithm>
#   include <locale>
#   include "Shlwapi.h"
#   include "WinDef.h"
#   include "rtc_base/win32.h"
#define GET_CURRENT_DIR _getcwd
#else
#   include <dirent.h>
#   include <unistd.h>
#   define GET_CURRENT_DIR getcwd
#endif

#ifdef OCTK_OS_MAC
#   include <CoreFoundation/CoreFoundation.h>
#endif

#include <sys/stat.h>  // To check for directory existence.

#ifndef S_ISDIR        // Not defined in stat.h on Windows.
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

#include <stdio.h>
#include <stdlib.h>

#include <type_traits>
#include <utility>
#include <memory>

OCTK_BEGIN_NAMESPACE

#if defined(OCTK_OS_WIN)
const StringView kPathDelimiter = "\\";
#elif !defined(OCTK_OS_IOS)
const StringView kPathDelimiter = "/";
#endif

#if defined(OCTK_OS_ANDROID)
// This is a special case in Chrome infrastructure. See base/test/test_support_android.cc.
const StringView kAndroidChromiumTestsRoot = "/sdcard/chromium_tests_root/";
#endif
#if !defined(OCTK_OS_IOS)
const StringView kResourcesDirName = "resources";
#endif

namespace utils
{

bool CreateDir(StringView directory_name)
{
    std::string directory_name_str(directory_name);
    struct stat path_info = {0};
    // Check if the path exists already:
    if (stat(directory_name_str.c_str(), &path_info) == 0)
    {
        if (!S_ISDIR(path_info.st_mode))
        {
            fprintf(stderr,
                    "Path %s exists but is not a directory! Remove this "
                    "file and re-run to create the directory.\n",
                    directory_name_str.c_str());
            return false;
        }
    }
    else
    {
#ifdef WIN32
        return _mkdir(directory_name_str.c_str()) == 0;
#else
        return mkdir(directory_name_str.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0;
#endif
    }
    return true;
}

std::string DirName(StringView path)
{
    if (path.empty())
    {
        return "";
    }
    if (path == kPathDelimiter)
    {
        return std::string(path);
    }

    if (path.back() == kPathDelimiter[0])
    {
        path.remove_suffix(1);
    }  // Remove trailing separator.

    return std::string(path.substr(0, path.find_last_of(kPathDelimiter)));
}

std::string OutputPath()
{
#if defined(OCTK_OS_IOS)
    return IOSOutputPath();
#elif defined(OCTK_OS_ANDROID)
    return std::string(kAndroidChromiumTestsRoot);
#else
    Optional<std::string> path_opt = ProjectRootPath();
    OCTK_DCHECK(path_opt);
    std::string path = *path_opt + "out";
    if (!CreateDir(path))
    {
        return "./";
    }
    return path + std::string(kPathDelimiter);
#endif
}

std::string WorkingDir()
{
#if defined(OCTK_OS_ANDROID)
    return std::string(kAndroidChromiumTestsRoot);
#else
    char path_buffer[FILENAME_MAX];
    if (!GET_CURRENT_DIR(path_buffer, sizeof(path_buffer)))
    {
        fprintf(stderr, "Cannot get current directory!\n");
        return "./";
    }
    else
    {
        return std::string(path_buffer);
    }
#endif
}

std::string ProjectRootPath()
{
#if defined(OCTK_OS_WIN)
    wchar_t buf[MAX_PATH];
  buf[0] = 0;
  if (GetModuleFileNameW(NULL, buf, MAX_PATH) == 0)
    return "";

  std::string exe_path = rtc::ToUtf8(std::wstring(buf));
  std::string exe_dir = DirName(exe_path);
  return DirName(DirName(exe_dir)) + std::string(kPathDelimiter);
#elif defined(OCTK_OS_ANDROID)
    return std::string(kAndroidChromiumTestsRoot);
#elif defined OCTK_OS_IOS
    return IOSRootPath();
#elif defined(OCTK_OS_MAC)
    std::string path;
    // internal::GetNSExecutablePath(&path);
    std::string exe_dir = DirName(path);
    // On Mac, tests execute in out/Whatever, so src is two levels up except if
    // the test is bundled (which our tests are not), in which case it's 5 levels.
    return DirName(DirName(exe_dir)) + std::string(kPathDelimiter);
#else
    char buf[PATH_MAX];
    ssize_t count = ::readlink("/proc/self/exe", buf, arraysize(buf));
    if (count <= 0) {
      OCTK_DCHECK_NOTREACHED() << "Unable to resolve /proc/self/exe.";
      return "";
    }
    // On POSIX, tests execute in out/Whatever, so src is two levels up.
    std::string exe_dir = DirName(absl::string_view(buf, count));
    return DirName(DirName(exe_dir)) + std::string(kPathDelimiter);
#endif
}

std::string ResourcePath(const char *name, const char *extension)
{
#if defined(OCTK_OS_IOS)
    return IOSResourcePath(name, extension);
#else
    Optional<std::string> path_opt = ProjectRootPath();
    OCTK_DCHECK(path_opt);
    std::stringstream ss;
    ss << kResourcesDirName << kPathDelimiter << name << "." << extension;
    path_opt.value() += ss.str();
    return path_opt.value();
#endif
}

std::string TempFilename(const char *dir, const char *prefix)
{
#ifdef WIN32
    wchar_t filename[MAX_PATH];
    if (::GetTempFileNameW(utils::ToUtf16(dir).c_str(),
                           utils::ToUtf16(prefix).c_str(), 0, filename) != 0)
    return utils::ToUtf8(filename);
    OCTK_DCHECK_NOTREACHED();
    return "";
#else
    std::stringstream ss;
    ss << dir << "/" << prefix << "XXXXXX";
    std::string tempname = ss.str();

    int fd = ::mkstemp(const_cast<char *>(tempname.data()));
    if (fd == -1)
    {
        OCTK_DCHECK_NOTREACHED();
        return "";
    }
    else
    {
        ::close(fd);
    }
    return tempname;
#endif
}
}
OCTK_END_NAMESPACE
