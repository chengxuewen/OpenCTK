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

#ifndef _OCTK_FRAME_GENERATOR_HPP
#define _OCTK_FRAME_GENERATOR_HPP

#include <octk_video_source_interface.hpp>
#include <octk_video_frame_buffer.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_nv12_buffer.hpp>
#include <octk_video_frame.hpp>
#include <octk_optional.hpp>
#include <octk_logging.hpp>
#include <octk_random.hpp>
#include <octk_mutex.hpp>
#include <octk_clock.hpp>

#include <memory>
#include <string>
#include <vector>

OCTK_BEGIN_NAMESPACE

class FrameGeneratorInterface
{
public:
    using UniquePtr = std::unique_ptr<FrameGeneratorInterface>;

    struct Resolution
    {
        size_t width;
        size_t height;
    };

    struct VideoFrameData
    {
        VideoFrameData(std::shared_ptr<VideoFrameBuffer> buff, Optional<VideoFrame::UpdateRect> rect)
            : buffer(std::move(buff))
            , updateRect(rect)
        {
        }

        std::shared_ptr<VideoFrameBuffer> buffer;
        Optional<VideoFrame::UpdateRect> updateRect;
    };

    enum class OutputType
    {
        kI420,
        kI420A,
        kI010,
        kNV12
    };
    static const char *outputTypeToString(OutputType type);

    virtual ~FrameGeneratorInterface() = default;

    /**
     * @return Returns VideoFrameBuffer and area where most of update was done to set them on the VideoFrame object.
     */
    virtual VideoFrameData nextFrame() = 0;

    /**
     * @brief Skips the next frame in case it doesn't need to be encoded.
     * @details Default implementation is to call nextFrame and ignore the returned value.
     */
    virtual void skipnextFrame() { this->nextFrame(); }

    /**
     * @brief Change the capture resolution.
     */
    virtual void changeResolution(size_t width, size_t height) = 0;

    virtual Resolution getResolution() const = 0;

    virtual StringView typeString() const = 0;
    /**
     * @return Returns the frames per second this generator is supposed to provide according to its data source.
     * Not all frame generators know the frames per second of the data source, in such case this method
     * returns utils::nullopt.
     */
    virtual Optional<int> fps() const = 0;

    virtual std::string name() const
    {
        std::stringstream ss;
        const auto resolution = this->getResolution();
        ss << this->typeString() << "-" << resolution.width << "x" << resolution.height;
        return ss.str();
    }
};

/**
 * @details SquareGenerator is a FrameGenerator that draws a given amount of randomly sized and colored squares.
 * Between each new generated frame, the squares are moved slightly towards the lower right corner.
 */
// class SquareGeneratorPrivate;
class OCTK_MEDIA_API SquareGenerator : public FrameGeneratorInterface
{
public:
    using UniquePtr = std::unique_ptr<SquareGenerator>;

    SquareGenerator(int width, int height, OutputType type, int num_squares);

    void changeResolution(size_t width, size_t height) override;
    VideoFrameData nextFrame() override;
    Resolution getResolution() const override;

    StringView typeString() const override { return "SquareGenerator"; }
    Optional<int> fps() const override { return utils::nullopt; }

private:
    std::shared_ptr<I420Buffer> createI420Buffer(int width, int height);

    class Square
    {
    public:
        Square(int width, int height, int seed);

        void draw(std::shared_ptr<VideoFrameBuffer> &frame_buffer);

    private:
        Random random_generator_;
        int x_;
        int y_;
        const int mLength;
        const uint8_t yuv_y_;
        const uint8_t yuv_u_;
        const uint8_t yuv_v_;
        const uint8_t yuv_a_;
    };

    mutable Mutex mMutex;
    const OutputType mType;
    int mWidth OCTK_ATTRIBUTE_GUARDED_BY(&mMutex);
    int mHeight OCTK_ATTRIBUTE_GUARDED_BY(&mMutex);
    std::vector<std::unique_ptr<Square>> mSquares OCTK_ATTRIBUTE_GUARDED_BY(&mMutex);
};

class OCTK_MEDIA_API YuvFileGenerator : public FrameGeneratorInterface
{
public:
    YuvFileGenerator(std::vector<FILE *> files, size_t width, size_t height, int frame_repeat_count);

    ~YuvFileGenerator();

    VideoFrameData nextFrame() override;
    void changeResolution(size_t width, size_t height) override
    {
        OCTK_WARNING() << "YuvFileGenerator::changeResolution not implemented";
    }
    Resolution getResolution() const override;

    StringView typeString() const override { return "YuvFileGenerator"; }
    Optional<int> fps() const override { return utils::nullopt; }

private:
    // Returns true if the new frame was loaded.
    // False only in case of a single file with a single frame in it.
    bool readnextFrame();

    size_t file_index_;
    size_t frame_index_;
    const std::vector<FILE *> files_;
    const size_t width_;
    const size_t height_;
    const size_t frame_size_;
    const std::unique_ptr<uint8_t[]> frame_buffer_;
    const int frame_display_count_;
    int current_display_count_;
    std::shared_ptr<I420Buffer> last_read_buffer_;
};

class OCTK_MEDIA_API NV12FileGenerator : public FrameGeneratorInterface
{
public:
    NV12FileGenerator(std::vector<FILE *> files, size_t width, size_t height, int frame_repeat_count);

    ~NV12FileGenerator();

    VideoFrameData nextFrame() override;
    void changeResolution(size_t width, size_t height) override
    {
        OCTK_WARNING() << "NV12FileGenerator::changeResolution not implemented";
    }
    Resolution getResolution() const override;

    StringView typeString() const override { return "NV12FileGenerator"; }
    Optional<int> fps() const override { return utils::nullopt; }

private:
    // Returns true if the new frame was loaded.
    // False only in case of a single file with a single frame in it.
    bool readnextFrame();

    size_t file_index_;
    size_t frame_index_;
    const std::vector<FILE *> files_;
    const size_t width_;
    const size_t height_;
    const size_t frame_size_;
    const std::unique_ptr<uint8_t[]> frame_buffer_;
    const int frame_display_count_;
    int current_display_count_;
    std::shared_ptr<NV12Buffer> last_read_buffer_;
};

// SlideGenerator works similarly to YuvFileGenerator but it fills the frames
// with randomly sized and colored squares instead of reading their content
// from files.
class OCTK_MEDIA_API SlideGenerator : public FrameGeneratorInterface
{
public:
    SlideGenerator(int width, int height, int frame_repeat_count);

    VideoFrameData nextFrame() override;
    void changeResolution(size_t width, size_t height) override
    {
        OCTK_WARNING() << "SlideGenerator::changeResolution not implemented";
    }
    Resolution getResolution() const override;

    StringView typeString() const override { return "SlideGenerator"; }
    Optional<int> fps() const override { return utils::nullopt; }

private:
    // Generates some randomly sized and colored squares scattered
    // over the frame.
    void generateNewFrame();

    const int width_;
    const int height_;
    const int frame_display_count_;
    int current_display_count_;
    Random random_generator_;
    std::shared_ptr<I420Buffer> buffer_;
};

class OCTK_MEDIA_API ScrollingImageFrameGenerator : public FrameGeneratorInterface
{
public:
    ScrollingImageFrameGenerator(Clock *clock,
                                 const std::vector<FILE *> &files,
                                 size_t source_width,
                                 size_t source_height,
                                 size_t target_width,
                                 size_t target_height,
                                 int64_t scroll_time_ms,
                                 int64_t pause_time_ms);
    ~ScrollingImageFrameGenerator() override = default;

    VideoFrameData nextFrame() override;
    void changeResolution(size_t width, size_t height) override
    {
        OCTK_WARNING() << "ScrollingImageFrameGenerator::changeResolution not implemented";
    }
    Resolution getResolution() const override;

    StringView typeString() const override { return "ScrollingImageFrameGenerator"; }
    Optional<int> fps() const override { return utils::nullopt; }

private:
    void updateSourceFrame(size_t frame_num);
    void cropSourceToScrolledImage(double scroll_factor);

    Clock *const clock_;
    const int64_t start_time_;
    const int64_t scroll_time_;
    const int64_t pause_time_;
    const size_t num_frames_;
    const int target_width_;
    const int target_height_;

    size_t current_frame_num_;
    bool prev_frame_not_scrolled_;
    VideoFrameData current_source_frame_;
    VideoFrameData current_frame_;
    YuvFileGenerator file_generator_;
};

OCTK_END_NAMESPACE

#endif // _OCTK_FRAME_GENERATOR_HPP
