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

#include <octk_create_frame_generator_capturer.hpp>
#include <octk_create_frame_generator.hpp>
#include <octk_string_utils.hpp>
#include <octk_time_delta.hpp>
#include <octk_file_utils.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

namespace
{
std::string TransformFilePath(std::string path)
{
    static const std::string resource_prefix = "res://";
    int ext_pos = path.rfind('.');
    if (ext_pos < 0)
    {
        return utils::ResourcePath(path, "yuv");
    }
    else if (utils::stringStartsWith(path, resource_prefix))
    {
        std::string name = path.substr(resource_prefix.length(), ext_pos);
        std::string ext = path.substr(ext_pos, path.size());
        return utils::ResourcePath(name, ext);
    }
    return path;
}
} // namespace
namespace utils
{
std::unique_ptr<FrameGeneratorCapturer> createFrameGeneratorCapturer(Clock *clock,
                                                                     FrameGeneratorCapturerConfig::SquaresVideo config,
                                                                     const TaskQueueBase::SharedPtr &taskQueue)
{
    return utils::make_unique<FrameGeneratorCapturer>(
        clock,
        utils::CreateSquareFrameGenerator(config.width, config.height, config.pixel_format, config.num_squares),
        config.framerate,
        taskQueue);
}
std::unique_ptr<FrameGeneratorCapturer> createFrameGeneratorCapturer(Clock *clock,
                                                                     FrameGeneratorCapturerConfig::SquareSlides config,
                                                                     const TaskQueueBase::SharedPtr &taskQueue)
{
    return utils::make_unique<FrameGeneratorCapturer>(
        clock,
        utils::CreateSlideFrameGenerator(config.width,
                                         config.height,
                                         /*frame_repeat_count*/ config.change_interval.seconds<double>() *
                                             config.framerate),
        config.framerate,
        taskQueue);
}
std::unique_ptr<FrameGeneratorCapturer> createFrameGeneratorCapturer(Clock *clock,
                                                                     FrameGeneratorCapturerConfig::VideoFile config,
                                                                     const TaskQueueBase::SharedPtr &taskQueue)
{
    OCTK_CHECK(config.width && config.height);
    return utils::make_unique<FrameGeneratorCapturer>(
        clock,
        utils::CreateFromYuvFileFrameGenerator({TransformFilePath(config.name)},
                                               config.width,
                                               config.height,
                                               /*frame_repeat_count*/ 1),
        config.framerate,
        taskQueue);
}

std::unique_ptr<FrameGeneratorCapturer> createFrameGeneratorCapturer(Clock *clock,
                                                                     FrameGeneratorCapturerConfig::ImageSlides config,
                                                                     const TaskQueueBase::SharedPtr &taskQueue)
{
    std::unique_ptr<FrameGeneratorInterface> slides_generator;
    std::vector<std::string> paths = config.paths;
    for (std::string &path : paths)
    {
        path = TransformFilePath(path);
    }

    if (config.crop.width || config.crop.height)
    {
        TimeDelta pause_duration = config.change_interval - config.crop.scroll_duration;
        OCTK_CHECK_GE(pause_duration, TimeDelta::Zero());
        int crop_width = config.crop.width.value_or(config.width);
        int crop_height = config.crop.height.value_or(config.height);
        OCTK_CHECK_LE(crop_width, config.width);
        OCTK_CHECK_LE(crop_height, config.height);
        slides_generator = utils::CreateScrollingInputFromYuvFilesFrameGenerator(clock,
                                                                                 paths,
                                                                                 config.width,
                                                                                 config.height,
                                                                                 crop_width,
                                                                                 crop_height,
                                                                                 config.crop.scroll_duration.ms(),
                                                                                 pause_duration.ms());
    }
    else
    {
        slides_generator = utils::CreateFromYuvFileFrameGenerator(
            paths,
            config.width,
            config.height,
            /*frame_repeat_count*/ config.change_interval.seconds<double>() * config.framerate);
    }
    return utils::make_unique<FrameGeneratorCapturer>(clock, std::move(slides_generator), config.framerate, taskQueue);
}

std::unique_ptr<FrameGeneratorCapturer> createFrameGeneratorCapturer(Clock *clock,
                                                                     const FrameGeneratorCapturerConfig &config,
                                                                     const TaskQueueBase::SharedPtr &taskQueue)
{
    if (config.videoFile)
    {
        return createFrameGeneratorCapturer(clock, *config.videoFile, taskQueue);
    }
    else if (config.imageSlides)
    {
        return createFrameGeneratorCapturer(clock, *config.imageSlides, taskQueue);
    }
    else if (config.squaresSlides)
    {
        return createFrameGeneratorCapturer(clock, *config.squaresSlides, taskQueue);
    }
    else
    {
        return createFrameGeneratorCapturer(clock,
                                            config.squaresVideo.value_or(FrameGeneratorCapturerConfig::SquaresVideo()),
                                            taskQueue);
    }
}

} // namespace utils

OCTK_END_NAMESPACE
