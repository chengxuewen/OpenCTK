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

#ifndef _OCTK_CREATE_FRAME_GENERATOR_CAPTURER_HPP
#define _OCTK_CREATE_FRAME_GENERATOR_CAPTURER_HPP

#include <octk_frame_generator_capturer.hpp>
#include <octk_custom_video_capturer.hpp>
#include <octk_frame_generator.hpp>
#include <octk_optional.hpp>
#include <octk_clock.hpp>

OCTK_BEGIN_NAMESPACE

namespace frame_gen_cap_impl
{
template <typename T>
class AutoOpt : public Optional<T>
{
public:
    using Optional<T>::optional;
    T *operator->()
    {
        if (!Optional<T>::has_value())
        {
            this->emplace(T());
        }
        return Optional<T>::operator->();
    }
};
}  // namespace frame_gen_cap_impl

struct FrameGeneratorCapturerConfig
{
    struct SquaresVideo
    {
        int framerate = 30;
        FrameGeneratorInterface::OutputType pixel_format = FrameGeneratorInterface::OutputType::kI420;
        int width = 320;
        int height = 180;
        int num_squares = 10;
    };

    struct SquareSlides
    {
        int framerate = 30;
        TimeDelta change_interval = TimeDelta::Seconds(10);
        int width = 1600;
        int height = 1200;
    };

    struct VideoFile
    {
        int framerate = 30;
        std::string name;
        // Must be set to width and height of the source video file.
        int width = 0;
        int height = 0;
    };

    struct ImageSlides
    {
        int framerate = 30;
        TimeDelta change_interval = TimeDelta::Seconds(10);
        struct Crop
        {
            TimeDelta scroll_duration = TimeDelta::Seconds(0);
            Optional<int> width;
            Optional<int> height;
        } crop;
        int width = 1850;
        int height = 1110;
        std::vector<std::string> paths = {
            "web_screenshot_1850_1110",
            "presentation_1850_1110",
            "photo_1850_1110",
            "difficult_photo_1850_1110",
        };
    };

    frame_gen_cap_impl::AutoOpt<SquaresVideo> squares_video;
    frame_gen_cap_impl::AutoOpt<SquareSlides> squares_slides;
    frame_gen_cap_impl::AutoOpt<VideoFile> video_file;
    frame_gen_cap_impl::AutoOpt<ImageSlides> image_slides;
};

namespace utils
{
OCTK_MEDIA_API std::unique_ptr<FrameGeneratorCapturer> CreateFrameGeneratorCapturer(
    Clock *clock,
    TaskQueueFactory &task_queue_factory,
    FrameGeneratorCapturerConfig::SquaresVideo config);

OCTK_MEDIA_API std::unique_ptr<FrameGeneratorCapturer> CreateFrameGeneratorCapturer(
    Clock *clock,
    TaskQueueFactory &task_queue_factory,
    FrameGeneratorCapturerConfig::SquareSlides config);

OCTK_MEDIA_API std::unique_ptr<FrameGeneratorCapturer> CreateFrameGeneratorCapturer(
    Clock *clock,
    TaskQueueFactory &task_queue_factory,
    FrameGeneratorCapturerConfig::VideoFile config);

OCTK_MEDIA_API std::unique_ptr<FrameGeneratorCapturer> CreateFrameGeneratorCapturer(
    Clock *clock,
    TaskQueueFactory &task_queue_factory,
    FrameGeneratorCapturerConfig::ImageSlides config);

OCTK_MEDIA_API std::unique_ptr<FrameGeneratorCapturer> CreateFrameGeneratorCapturer(
    Clock *clock,
    TaskQueueFactory &task_queue_factory,
    const FrameGeneratorCapturerConfig &config);
} // namespace utils
OCTK_END_NAMESPACE

#endif // _OCTK_CREATE_FRAME_GENERATOR_CAPTURER_HPP
