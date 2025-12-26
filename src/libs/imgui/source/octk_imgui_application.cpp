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
#include <octk_processor.hpp>
#include <octk_checks.hpp>

#include <libyuv.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

OCTK_BEGIN_NAMESPACE

void ImGuiImage::setFrameData(const uint8_t *data, int width, int height)
{
    SpinLock::Locker locker(mSpinLock);
    if (width != mWidth || height != mHeight)
    {
        if (Format::RGB24 == mFormat)
        {
            const auto bufferSize = (width * height * 4);
            Binary buffer(bufferSize);
            libyuv::RGB24ToARGB(data, 3 * width, buffer.data(), 4 * width, width, height);
            const auto scaledSize = (mWidth * mHeight * 4);
            Binary scaledBuffer(scaledSize);
            libyuv::ARGBScale(buffer.data(),
                              4 * width,
                              width,
                              height,
                              scaledBuffer.data(),
                              4 * mWidth,
                              mWidth,
                              mHeight,
                              libyuv::kFilterBox);
            libyuv::ARGBToRGB24(scaledBuffer.data(), 4 * mWidth, mFrameData.data(), 3 * mWidth, mWidth, mHeight);
        }
        else if (Format::RGBA32 == mFormat)
        {
            libyuv::ARGBScale(data,
                              4 * width,
                              width,
                              height,
                              mFrameData.data(),
                              this->pitchSize(),
                              mWidth,
                              mHeight,
                              libyuv::kFilterBox);
        }
    }
    else
    {
        std::memcpy(mFrameData.data(), data, mFrameData.size());
    }
    mChanged.store(true);
}

namespace detail
{
using ImGuiApplicationCreaterMap = std::unordered_map<std::string, ImGuiApplication::Factory::CreaterFunction>;
using ImGuiApplicationCreaterMapItem = std::pair<std::string, ImGuiApplication::Factory::CreaterFunction>;
static ImGuiApplicationCreaterMap &imGuiApplicationCreaterMap()
{
    static ImGuiApplicationCreaterMap map;
    return map;
}
} // namespace detail

std::vector<std::string> ImGuiApplication::Factory::registeredTypes()
{
    std::vector<std::string> keys;
    auto map = detail::imGuiApplicationCreaterMap();
    keys.reserve(map.size());
    std::transform(map.begin(),
                   map.end(),
                   std::back_inserter(keys),
                   [](const detail::ImGuiApplicationCreaterMapItem &pair) { return pair.first; });
    return keys;
}

ImGuiApplication::UniquePtr ImGuiApplication::Factory::create(StringView typeName, const Properties &properties)
{
    if ("" == typeName)
    {
        typeName = constants::kImGuiApplicationSDLGPU3;
    }
    auto map = detail::imGuiApplicationCreaterMap();
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
    detail::imGuiApplicationCreaterMap().insert(std::make_pair(typeName, func));
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

void ImGuiApplication::setInitFunction(Callback func)
{
    OCTK_D(ImGuiApplication);
    SpinLock::Locker locker(d->mCallbackSpinLock);
    d->mInitFunction = func;
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

Expected<ImGuiImage::SharedPtr, std::string> ImGuiApplication::loadImage(StringView path)
{
    int width, height, channels;
    auto expected = this->readImage(path.data(), &width, &height, &channels);
    if (expected.has_value())
    {
        return this->createImage(ImGuiImage::Format::RGBA32, expected.value(), width, height);
    }
    return utils::makeUnexpected(std::string("imreadBMP failed:") + expected.error());
}

ImGuiImage::SharedPtr ImGuiApplication::createImage(ImGuiImage::Format format, int width, int height)
{
    Binary binary(ImGuiImage::sizeInBytes(format, width, height));
    std::fill(binary.begin(), binary.end(), 0xFF);
    return this->createImage(format, binary, width, height);
}

ImGuiImage::SharedPtr ImGuiApplication::createImage(ImGuiImage::Format format,
                                                    const Binary &binary,
                                                    int width,
                                                    int height)
{
    OCTK_CHECK_NOTREACHED();
    return nullptr;
}

Expected<Binary, std::string> ImGuiApplication::readImage(const char *path, int *width, int *height, int *channels)
{
    unsigned char *imageData = stbi_load(path, width, height, channels, 4);
    if (imageData)
    {
        Binary binary(imageData, imageData + (*width) * (*height) * (*channels));
        stbi_image_free(imageData);
        return binary;
    }
    return utils::makeUnexpected(std::string("stbi_load failed:") + stbi_failure_reason());
}

OCTK_END_NAMESPACE