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

#include <private/octk_imgui_application_p.hpp>

OCTK_BEGIN_NAMESPACE

namespace internal
{
using ImGuiApplicationCreaterMap = std::unordered_map<std::string, ImGuiApplication::Factory::CreaterFunction>;
using ImGuiApplicationCreaterMapItem = std::pair<std::string, ImGuiApplication::Factory::CreaterFunction>;
static ImGuiApplicationCreaterMap &imGuiApplicationCreaterMap()
{
    static ImGuiApplicationCreaterMap map;
    return map;
}
} // namespace internal

std::vector<std::string> ImGuiApplication::Factory::registeredTypes()
{
    std::vector<std::string> keys;
    auto map = internal::imGuiApplicationCreaterMap();
    keys.reserve(map.size());
    std::transform(map.begin(),
                   map.end(),
                   std::back_inserter(keys),
                   [](const internal::ImGuiApplicationCreaterMapItem &pair) { return pair.first; });
    return keys;
}

ImGuiApplication::UniquePtr ImGuiApplication::Factory::create(StringView typeName, const Properties &properties)
{
    if ("" == typeName)
    {
        typeName = constants::kImGuiApplicationSDLGPU3;
    }
    auto map = internal::imGuiApplicationCreaterMap();
    const auto iter = map.find(typeName.data());
    if (map.cend() != iter)
    {
        auto func = iter->second;
        if (func)
        {
            return func(properties);
        }
    }
    return nullptr;
}

void ImGuiApplication::Factory::registerApplication(StringView typeName, CreaterFunction func)
{
    internal::imGuiApplicationCreaterMap().insert(std::make_pair(typeName, func));
}

ImGuiApplicationPrivate::ImGuiApplicationPrivate(ImGuiApplication *p)
    : mPPtr(p)
{
}

ImGuiApplicationPrivate::~ImGuiApplicationPrivate() { }

ImGuiApplication::ImGuiApplication(const Properties &properties)
    : ImGuiApplication(new ImGuiApplicationPrivate(this), properties)
{
}
ImGuiApplication::ImGuiApplication(ImGuiApplicationPrivate *d, const Properties &properties)
    : mDPtr(d)
{
    mDPtr->mProperties = properties;
}

ImGuiApplication::~ImGuiApplication() { this->destroy(); }

bool ImGuiApplication::isReady() const
{
    OCTK_D(const ImGuiApplication);
    return d->mInitSuccess.load();
}

bool ImGuiApplication::isFinished() const
{
    OCTK_D(const ImGuiApplication);
    return d->mFinished.load();
}

std::string ImGuiApplication::lastError() const
{
    OCTK_D(const ImGuiApplication);
    return d->mLastError;
}

void ImGuiApplication::setDrawFunction(Callback func)
{
    OCTK_D(ImGuiApplication);
    SpinLock::Locker locker(d->mCallbackSpinLock);
    d->mDrawFunction = func;
}

void ImGuiApplication::setQuitFunction(Callback func)
{
    OCTK_D(ImGuiApplication);
    SpinLock::Locker locker(d->mCallbackSpinLock);
    d->mQuitFunction = func;
}

bool ImGuiApplication::init()
{
    OCTK_D(ImGuiApplication);
    d->mInitSuccess.store(true);
    return d->mInitSuccess.load();
}

bool ImGuiApplication::exec() { return true; }

void ImGuiApplication::destroy() { }

OCTK_END_NAMESPACE