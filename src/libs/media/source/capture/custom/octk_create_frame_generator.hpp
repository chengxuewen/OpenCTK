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

#ifndef _OCTK_CREATE_FRAME_GENERATOR_HPP
#define _OCTK_CREATE_FRAME_GENERATOR_HPP

#include <octk_frame_generator.hpp>
#include <octk_string_view.hpp>
#include <octk_nullability.hpp>
#include <octk_clock.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// #include "api/environment/environment.h"


OCTK_BEGIN_NAMESPACE

namespace utils
{

// Creates a frame generator that produces frames with small squares that
// move randomly towards the lower right corner.
// `type` has the default value FrameGeneratorInterface::OutputType::I420.
// `num_squares` has the default value 10.
OCTK_MEDIA_API std::unique_ptr<FrameGeneratorInterface>
CreateSquareFrameGenerator(int width,
                           int height,
                           Optional<FrameGeneratorInterface::OutputType> type,
                           Optional<int> num_squares);

// Creates a frame generator that repeatedly plays a set of yuv files.
// The frame_repeat_count determines how many times each frame is shown,
// with 1 = show each frame once, etc.
OCTK_MEDIA_API std::unique_ptr<FrameGeneratorInterface>
CreateFromYuvFileFrameGenerator(std::vector<std::string> filenames,
                                size_t width,
                                size_t height,
                                int frame_repeat_count);

// Creates a frame generator that repeatedly plays a set of nv12 files.
// The frame_repeat_count determines how many times each frame is shown,
// with 1 = show each frame once, etc.
OCTK_MEDIA_API std::unique_ptr<FrameGeneratorInterface>
CreateFromNV12FileFrameGenerator(std::vector<std::string> filenames,
                                 size_t width,
                                 size_t height,
                                 int frame_repeat_count = 1);

// absl::Nonnull <std::unique_ptr<FrameGeneratorInterface>>
// CreateFromIvfFileFrameGenerator(const RtcContext &env,
//                                 absl::string_view filename,
//                                 Optional<int> fps_hint = std::nullopt);

// Creates a frame generator which takes a set of yuv files (wrapping a
// frame generator created by CreateFromYuvFile() above), but outputs frames
// that have been cropped to specified resolution: source_width/source_height
// is the size of the source images, target_width/target_height is the size of
// the cropped output. For each source image read, the cropped viewport will
// be scrolled top to bottom/left to right for scroll_tim_ms milliseconds.
// After that the image will stay in place for pause_time_ms milliseconds,
// and then this will be repeated with the next file from the input set.
OCTK_MEDIA_API std::unique_ptr<FrameGeneratorInterface>
CreateScrollingInputFromYuvFilesFrameGenerator(Clock *clock,
                                               std::vector<std::string> filenames,
                                               size_t source_width,
                                               size_t source_height,
                                               size_t target_width,
                                               size_t target_height,
                                               int64_t scroll_time_ms,
                                               int64_t pause_time_ms);

// Creates a frame generator that produces randomly generated slides. It fills
// the frames with randomly sized and colored squares.
// `frame_repeat_count` determines how many times each slide is shown.
OCTK_MEDIA_API std::unique_ptr<FrameGeneratorInterface>
CreateSlideFrameGenerator(int width, int height, int frame_repeat_count);
} // namespace utils

OCTK_END_NAMESPACE

#endif // _OCTK_CREATE_FRAME_GENERATOR_HPP
