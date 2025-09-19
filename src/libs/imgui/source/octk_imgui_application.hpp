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

#ifndef _OCTK_IMGUI_APPLICATION_HPP
#define _OCTK_IMGUI_APPLICATION_HPP

#include <octk_imgui_constants.hpp>
#include <octk_string_view.hpp>
#include <octk_optional.hpp>
#include <octk_spinlock.hpp>
#include <octk_expected.hpp>
#include <octk_result.hpp>
#include <octk_memory.hpp>
#include <octk_assert.hpp>
#include <octk_imgui.hpp>

#include <functional>

OCTK_BEGIN_NAMESPACE

struct ImGuiImage
{
    enum class Format
    {
        RGB24,
        RGBA32
    };
    using SharedPtr = std::shared_ptr<ImGuiImage>;

    ImGuiImage(Format format, int width, int height)
        : mFormat(format)
        , mWidth(width)
        , mHeight(height)
        , mFrameData(sizeInBytes(format, width, height))
    {
        std::memset(mFrameData.data(), 0XFF, mFrameData.size());
    }
    virtual ~ImGuiImage() { }

    virtual size_t textureId() = 0;
    virtual void updateTexture() = 0;

    void checkUpdateTexture()
    {
        if (mChanged.exchange(false))
        {
            this->updateTexture();
        }
    }

    bool valid() const { return mLastError.empty(); }
    std::string lastError() const { return mLastError; }

    Binary frameData() const
    {
        SpinLock::Locker locker(mSpinLock);
        return mFrameData;
    }
    void setFrameData(const uint8_t *data)
    {
        SpinLock::Locker locker(mSpinLock);
        std::memcpy(mFrameData.data(), data, mFrameData.size());
        mChanged.store(true);
    }
    void setFrameData(const uint8_t *data, int width, int height);

    int width() const { return mWidth; }
    int height() const { return mHeight; }
    ImVec2 scaledSize(float width, float height) const
    {
        const auto length = std::min(width, height);
        const auto aspectRatio = this->aspectRatio();
        return ImVec2{length * aspectRatio, length / aspectRatio};
    }
    float aspectRatio() const { return static_cast<float>(this->width()) / this->height(); }

    Format format() const { return mFormat; }
    int pixelSize() const { return pixelSize(mFormat); }
    int pitchSize() const { return mWidth * this->pixelSize(); }
    int bytesPerLine() const { return mWidth * this->pixelSize(); }
    int sizeInBytes() const { return mHeight * this->bytesPerLine(); }
    static int pixelSize(Format format) { return (Format::RGB24 == format) ? 3 : 4; }
    static int sizeInBytes(Format format, int width, int height) { return pixelSize(format) * width * height; }

protected:
    virtual void init(void *data = nullptr) = 0;
    virtual void destroy() = 0;

    Binary mFrameData;
    const int mWidth{0};
    const int mHeight{0};
    std::string mLastError;
    mutable SpinLock mSpinLock;
    std::atomic_bool mChanged{true};
    const Format mFormat{Format::RGBA32};
    friend class ImGuiApplicationPrivate;
};

class ImGuiApplicationPrivate;
class OCTK_IMGUI_API ImGuiApplication
{
public:
    using Callback = std::function<void()>;
    using UniquePtr = std::unique_ptr<ImGuiApplication>;

    struct Properties final
    {
        Properties() = default;

        Optional<uint_t> width;
        Optional<uint_t> height;
        Optional<std::string> title;
    };

    struct Factory
    {
        using CreaterFunction = std::function<ImGuiApplication::UniquePtr(const Properties &)>;
        template <typename T> static CreaterFunction makeCreaterFunction()
        {
            return [](const Properties &properties) { return utils::makeUnique<T>(properties); };
        }

        static ImGuiApplication::UniquePtr create(StringView typeName = "", const Properties &properties = {});
        static void registerApplication(StringView typeName, CreaterFunction func);
        template <typename T> static void registerApplication(StringView typeName)
        {
            enum
            {
                Valid = std::is_base_of<ImGuiApplication, T>::value
            };
            OCTK_STATIC_ASSERT_X(Valid, "type must base on ImGuiApplication.");
            Factory::registerApplication(typeName, makeCreaterFunction<T>());
        }
        static std::vector<std::string> registeredTypes();
    };
    template <typename T> struct Registrar final
    {
        explicit Registrar(StringView typeName) { Factory::registerApplication<T>(typeName); }
    };

    ImGuiApplication(const Properties &properties = {});
    ImGuiApplication(ImGuiApplicationPrivate *d, const Properties &properties = {});
    virtual ~ImGuiApplication();

    bool isReady() const;
    bool isFinished() const;
    std::string lastError() const;

    void setInitFunction(Callback func);
    void setDrawFunction(Callback func);
    void setQuitFunction(Callback func);

    virtual bool exec();
    virtual StringView typeName() const = 0;

    virtual Expected<ImGuiImage::SharedPtr, std::string> loadImage(StringView path);
    virtual ImGuiImage::SharedPtr createImage(ImGuiImage::Format format, int width, int height);
    virtual ImGuiImage::SharedPtr createImage(ImGuiImage::Format format, const Binary &binary, int width, int height);

    /**
     * @param path
     * @param width
     * @param height
     * @param channels
     * @return Return RGBA32 images data.
     */
    static Expected<Binary, std::string> readImage(const char *path, int *width, int *height, int *channels);

protected:
    virtual bool init();
    virtual void destroy();

protected:
    OCTK_DEFINE_DPTR(ImGuiApplication)
    OCTK_DECLARE_PRIVATE(ImGuiApplication)
    OCTK_DISABLE_COPY_MOVE(ImGuiApplication)
};

OCTK_END_NAMESPACE

#define OCTK_IMGUI_REGISTER_APPLICATION(Type, Name)                                                                    \
    namespace internal                                                                                                 \
    {                                                                                                                  \
    static octk::ImGuiApplication::Registrar<Type> imguiRegistrar##Type(Name);                                         \
    }

#endif // _OCTK_IMGUI_APPLICATION_HPP
