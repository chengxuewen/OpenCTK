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

#include <unix/octk_screen_drawer_lock_posix.hpp>
#include <octk_logging.hpp>
#include <octk_checks.hpp>

#include <sys/stat.h>
#include <fcntl.h>

OCTK_BEGIN_NAMESPACE

namespace
{

// A uuid as the name of semaphore.
static constexpr char kSemaphoreName[] = "GSDL54fe5552804711e6a7253f429a";
}  // namespace

ScreenDrawerLockPosix::ScreenDrawerLockPosix()
    : ScreenDrawerLockPosix(kSemaphoreName) {}

ScreenDrawerLockPosix::ScreenDrawerLockPosix(const char *name)
{
    semaphore_ = sem_open(name, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);
    if (semaphore_ == SEM_FAILED)
    {
        OCTK_ERROR() << "Failed to create named semaphore with " << name;
        OCTK_DCHECK_NOTREACHED();
    }

    sem_wait(semaphore_);
}

ScreenDrawerLockPosix::~ScreenDrawerLockPosix()
{
    if (semaphore_ == SEM_FAILED)
    {
        return;
    }

    sem_post(semaphore_);
    sem_close(semaphore_);
    // sem_unlink a named semaphore won't wait until other clients to release the
    // sem_t. So if a new process starts, it will sem_open a different kernel
    // object with the same name and eventually breaks the cross-process lock.
}

// static
void ScreenDrawerLockPosix::Unlink(StringView name)
{
    sem_unlink(std::string(name).c_str());
}
OCTK_END_NAMESPACE
