/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#include <private/octk_pipewire_utils_p.hpp>
#include <octk_sanitizer.hpp>

#include <pipewire/pipewire.h>

OCTK_BEGIN_NAMESPACE

OCTK_NO_SANITIZE("cfi-icall")
bool InitializePipeWire()
{
#if defined(WEBRTC_DLOPEN_PIPEWIRE)
    static constexpr char kPipeWireLib[] = "libpipewire-0.3.so.0";

    using modules_portal::InitializeStubs;
    using modules_portal::kModulePipewire;

    modules_portal::StubPathMap paths;

    // Check if the PipeWire library is available.
    paths[kModulePipewire].push_back(kPipeWireLib);

    static bool result = InitializeStubs(paths);

    return result;
#else
    return true;
#endif // defined(WEBRTC_DLOPEN_PIPEWIRE)
}

PipeWireThreadLoopLock::PipeWireThreadLoopLock(pw_thread_loop *loop)
    : loop_(loop)
{
    pw_thread_loop_lock(loop_);
}

PipeWireThreadLoopLock::~PipeWireThreadLoopLock()
{
    pw_thread_loop_unlock(loop_);
}

OCTK_END_NAMESPACE