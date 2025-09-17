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

#ifndef _OCTK_IMGUI_APPLICATION_P_HPP
#define _OCTK_IMGUI_APPLICATION_P_HPP

#include <octk_imgui_application.hpp>
#include <octk_imgui_constants.hpp>
#include <private/octk_sdl_p.hpp>
#include <octk_scope_guard.hpp>
#include <octk_once_flag.hpp>
#include <octk_spinlock.hpp>
#include <unordered_set>

OCTK_BEGIN_NAMESPACE

class ImGuiApplicationPrivate
{
protected:
    OCTK_DEFINE_PPTR(ImGuiApplication)
    OCTK_DECLARE_PUBLIC(ImGuiApplication)
    OCTK_DISABLE_COPY_MOVE(ImGuiApplicationPrivate)
public:
    OCTK_STATIC_CONSTANT_NUMBER(kDefaultWidth, 1280)
    OCTK_STATIC_CONSTANT_NUMBER(kDefaultHeight, 720)

    using Properties = ImGuiApplication::Properties;
    using Callback = ImGuiApplication::Callback;

    explicit ImGuiApplicationPrivate(ImGuiApplication *p);
    virtual ~ImGuiApplicationPrivate();

    int height() const { return mProperties.height.has_value() ? mProperties.height.value() : kDefaultHeight; }
    int width() const { return mProperties.width.has_value() ? mProperties.width.value() : kDefaultWidth; }
    std::string title(const std::string &title) const
    {
        return mProperties.title.has_value() ? mProperties.title.value() : title;
    }

    void setError(const std::string &error) { mLastError = error; }
    void quit() { mFinished.store(true); }

    void initImages(void *data = nullptr)
    {
        auto images = std::move(mImagesSet);
        for (auto &image : images)
        {
            image->init(data);
            mInitedImagesSet.insert(image);
        }
    }
    void destroyImages()
    {
        for (auto &image : mInitedImagesSet)
        {
            image->destroy();
        }
    }

    OnceFlag mInitOnceFlag;
    OnceFlag mDestroyOnceFlag;

    std::atomic_bool mFinished{false};
    std::atomic_bool mInitSuccess{false};
    const ImVec4 mClearColor{0.45f, 0.55f, 0.60f, 1.00f};
    std::unordered_set<ImGuiImage::SharedPtr> mImagesSet;
    std::unordered_set<ImGuiImage::SharedPtr> mInitedImagesSet;

    Properties mProperties;
    std::string mLastError;
    Callback mInitFunction;
    Callback mDrawFunction;
    Callback mQuitFunction;
    mutable SpinLock mCallbackSpinLock;
};

OCTK_END_NAMESPACE

#endif // _OCTK_IMGUI_APPLICATION_P_HPP
