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

#include <octk_create_frame_generator.hpp>
#include <octk_memory.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

namespace utils
{
std::unique_ptr<FrameGeneratorInterface> CreateSquareFrameGenerator(int width,
                                                                    int height,
                                                                    Optional<FrameGeneratorInterface::OutputType> type,
                                                                    Optional<int> num_squares)
{
    return utils::make_unique<SquareGenerator>(width, height,
                                             type.value_or(FrameGeneratorInterface::OutputType::kI420),
                                             num_squares.value_or(10));
}

std::unique_ptr<FrameGeneratorInterface> CreateFromYuvFileFrameGenerator(std::vector<std::string> filenames,
                                                                         size_t width,
                                                                         size_t height,
                                                                         int frame_repeat_count)
{
    OCTK_DCHECK(!filenames.empty());
    std::vector<FILE *> files;
    for (const std::string &filename: filenames)
    {
        FILE *file = fopen(filename.c_str(), "rb");
        OCTK_DCHECK(file != nullptr) << "Failed to open: '" << filename << "'\n";
        files.push_back(file);
    }

    return utils::make_unique<YuvFileGenerator>(files, width, height,
                                              frame_repeat_count);
}

std::unique_ptr<FrameGeneratorInterface> CreateFromNV12FileFrameGenerator(std::vector<std::string> filenames,
                                                                          size_t width,
                                                                          size_t height,
                                                                          int frame_repeat_count)
{
    OCTK_DCHECK(!filenames.empty());
    std::vector<FILE *> files;
    for (const std::string &filename: filenames)
    {
        FILE *file = fopen(filename.c_str(), "rb");
        OCTK_DCHECK(file != nullptr) << "Failed to open: '" << filename << "'\n";
        files.push_back(file);
    }

    return utils::make_unique<NV12FileGenerator>(files, width, height,
                                               frame_repeat_count);
}
//
// absl::Nonnull <std::unique_ptr<FrameGeneratorInterface>>
// CreateFromIvfFileFrameGenerator(const RtcContext &env,
//                                 StringView filename,
//                                 Optional<int> fps_hint)
// {
//     return utils::make_unique<IvfVideoFrameGenerator>(env, filename, fps_hint);
// }

std::unique_ptr<FrameGeneratorInterface>
CreateScrollingInputFromYuvFilesFrameGenerator(Clock *clock,
                                               std::vector<std::string> filenames,
                                               size_t source_width,
                                               size_t source_height,
                                               size_t target_width,
                                               size_t target_height,
                                               int64_t scroll_time_ms,
                                               int64_t pause_time_ms)
{
    OCTK_DCHECK(!filenames.empty());
    std::vector<FILE *> files;
    for (const std::string &filename: filenames)
    {
        FILE *file = fopen(filename.c_str(), "rb");
        OCTK_DCHECK(file != nullptr);
        files.push_back(file);
    }

    return utils::make_unique<ScrollingImageFrameGenerator>(clock, files,
                                                          source_width, source_height,
                                                          target_width, target_height,
                                                          scroll_time_ms, pause_time_ms);
}

std::unique_ptr<FrameGeneratorInterface> CreateSlideFrameGenerator(int width, int height, int frame_repeat_count)
{
    return utils::make_unique<SlideGenerator>(width, height, frame_repeat_count);
}
} // namespace utils
OCTK_END_NAMESPACE
