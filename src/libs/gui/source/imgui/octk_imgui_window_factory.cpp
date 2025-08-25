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

#include <octk_imgui_window_factory.hpp>
#include "backends/octk_imgui_window_sdlgpu3.hpp"

#include <unordered_map>
#include <algorithm>

OCTK_BEGIN_NAMESPACE

using ImGuiWindowCreaterMap = std::unordered_map<std::string, ImGuiWindowCreaterFunction>;
using ImGuiWindowCreaterMapItem = std::pair<std::string, ImGuiWindowCreaterFunction>;

class ImGuiWindowFactoryPrivate
{
    OCTK_DEFINE_PPTR(ImGuiWindowFactory)
    OCTK_DECLARE_PUBLIC(ImGuiWindowFactory)
    OCTK_DISABLE_COPY_MOVE(ImGuiWindowFactoryPrivate)
public:
    explicit ImGuiWindowFactoryPrivate(ImGuiWindowFactory *p);
    virtual ~ImGuiWindowFactoryPrivate() { }

    mutable std::mutex mMutex;
    ImGuiWindowCreaterMap mImGuiWindowCreaterMap;
};

ImGuiWindowFactoryPrivate::ImGuiWindowFactoryPrivate(ImGuiWindowFactory *p)
    : mPPtr(p)
{
}

ImGuiWindowFactory::ImGuiWindowFactory()
    : mDPtr(new ImGuiWindowFactoryPrivate(this))
{
    this->registerImGuiWindow<ImGuiWindowSDLGPU3>(constants::kImGuiWindowSDLGPU3);
}

ImGuiWindowFactory::~ImGuiWindowFactory() { }

std::vector<std::string> ImGuiWindowFactory::imGuiWindowTypes() const
{
    OCTK_D(const ImGuiWindowFactory);
    std::vector<std::string> keys;
    keys.reserve(d->mImGuiWindowCreaterMap.size());
    std::transform(d->mImGuiWindowCreaterMap.begin(),
                   d->mImGuiWindowCreaterMap.end(),
                   std::back_inserter(keys),
                   [](const ImGuiWindowCreaterMapItem &pair) { return pair.first; });
    return keys;
}
ImGuiWindow::UniquePtr ImGuiWindowFactory::createImGuiWindow(StringView type) const
{
    OCTK_D(const ImGuiWindowFactory);
    std::lock_guard<std::mutex> locker(d->mMutex);
    const auto iter = d->mImGuiWindowCreaterMap.find(type.data());
    if (d->mImGuiWindowCreaterMap.cend() != iter)
    {
        auto func = iter->second;
        if (func)
        {
            return func();
        }
    }
    return nullptr;
}
void ImGuiWindowFactory::registerImGuiWindow(StringView type, ImGuiWindowCreaterFunction func)
{
    OCTK_D(ImGuiWindowFactory);
    std::lock_guard<std::mutex> locker(d->mMutex);
    d->mImGuiWindowCreaterMap.insert(std::make_pair(type, func));
}

OCTK_END_NAMESPACE
