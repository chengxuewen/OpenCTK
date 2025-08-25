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

#ifndef _OCTK_DESKTOP_CAPTURE_SCREEN_DRAWER_LOCK_POSIX_HPP
#define _OCTK_DESKTOP_CAPTURE_SCREEN_DRAWER_LOCK_POSIX_HPP

#include <octk_string_view.hpp>
#include <octk_screen_drawer.hpp>

#include <semaphore.h>

OCTK_BEGIN_NAMESPACE

class ScreenDrawerLockPosix final : public ScreenDrawerLock
{
public:
    ScreenDrawerLockPosix();
    // Provides a name other than the default one for test only.
    explicit ScreenDrawerLockPosix(const char *name);
    ~ScreenDrawerLockPosix() override;

    // Unlinks the named semaphore actively. This will remove the sem_t object in
    // the system and allow others to create a different sem_t object with the
    // same/ name.
    static void Unlink(StringView name);

private:
    sem_t *semaphore_;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_DESKTOP_CAPTURE_SCREEN_DRAWER_LOCK_POSIX_HPP
