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

#include <octk_desktop_capturer_differ_wrapper.hpp>
#include <octk_desktop_geometry.hpp>
#include <octk_desktop_region.hpp>
#include <octk_differ_block.hpp>
#include <octk_date_time.hpp>
#include <octk_checks.hpp>

#include <cstdint>
#include <cstring>
#include <utility>

OCTK_BEGIN_NAMESPACE

namespace
{

// Returns true if (0, 0) - (`width`, `height`) vector in `old_buffer` and
// `new_buffer` are equal. `width` should be less than 32
// (defined by kBlockSize), otherwise BlockDifference() should be used.
bool PartialBlockDifference(const uint8_t *old_buffer,
                            const uint8_t *new_buffer,
                            int width,
                            int height,
                            int stride)
{
    OCTK_DCHECK_LT(width, kBlockSize);
    const int width_bytes = width * DesktopFrame::kBytesPerPixel;
    for (int i = 0; i < height; i++)
    {
        if (memcmp(old_buffer, new_buffer, width_bytes) != 0)
        {
            return true;
        }
        old_buffer += stride;
        new_buffer += stride;
    }
    return false;
}

// Compares columns in the range of [`left`, `right`), in a row in the
// range of [`top`, `top` + `height`), starts from `old_buffer` and
// `new_buffer`, and outputs updated regions into `output`. `stride` is the
// DesktopFrame::stride().
void CompareRow(const uint8_t *old_buffer,
                const uint8_t *new_buffer,
                const int left,
                const int right,
                const int top,
                const int bottom,
                const int stride,
                DesktopRegion *const output)
{
    const int block_x_offset = kBlockSize * DesktopFrame::kBytesPerPixel;
    const int width = right - left;
    const int height = bottom - top;
    const int block_count = (width - 1) / kBlockSize;
    const int last_block_width = width - block_count * kBlockSize;
    OCTK_DCHECK_GT(last_block_width, 0);
    OCTK_DCHECK_LE(last_block_width, kBlockSize);

    // The first block-column in a continuous dirty area in current block-row.
    int first_dirty_x_block = -1;

    // We always need to add dirty area into `output` in the last block, so handle
    // it separatedly.
    for (int x = 0; x < block_count; x++)
    {
        if (BlockDifference(old_buffer, new_buffer, height, stride))
        {
            if (first_dirty_x_block == -1)
            {
                // This is the first dirty block in a continuous dirty area.
                first_dirty_x_block = x;
            }
        }
        else if (first_dirty_x_block != -1)
        {
            // The block on the left is the last dirty block in a continuous
            // dirty area.
            output->AddRect(
                DesktopRect::MakeLTRB(first_dirty_x_block * kBlockSize + left, top,
                                      x * kBlockSize + left, bottom));
            first_dirty_x_block = -1;
        }
        old_buffer += block_x_offset;
        new_buffer += block_x_offset;
    }

    bool last_block_diff;
    if (last_block_width < kBlockSize)
    {
        // The last one is a partial vector.
        last_block_diff = PartialBlockDifference(old_buffer, new_buffer,
                                                 last_block_width, height, stride);
    }
    else
    {
        last_block_diff = BlockDifference(old_buffer, new_buffer, height, stride);
    }
    if (last_block_diff)
    {
        if (first_dirty_x_block == -1)
        {
            first_dirty_x_block = block_count;
        }
        output->AddRect(DesktopRect::MakeLTRB(
            first_dirty_x_block * kBlockSize + left, top, right, bottom));
    }
    else if (first_dirty_x_block != -1)
    {
        output->AddRect(
            DesktopRect::MakeLTRB(first_dirty_x_block * kBlockSize + left, top,
                                  block_count * kBlockSize + left, bottom));
    }
}

// Compares `rect` area in `old_frame` and `new_frame`, and outputs dirty
// regions into `output`.
void CompareFrames(const DesktopFrame &old_frame,
                   const DesktopFrame &new_frame,
                   DesktopRect rect,
                   DesktopRegion *const output)
{
    OCTK_DCHECK(old_frame.size().equals(new_frame.size()));
    OCTK_DCHECK_EQ(old_frame.stride(), new_frame.stride());
    rect.IntersectWith(DesktopRect::MakeSize(old_frame.size()));

    const int y_block_count = (rect.height() - 1) / kBlockSize;
    const int last_y_block_height = rect.height() - y_block_count * kBlockSize;
    // Offset from the start of one block-row to the next.
    const int block_y_stride = old_frame.stride() * kBlockSize;
    const uint8_t *prev_block_row_start =
        old_frame.GetFrameDataAtPos(rect.top_left());
    const uint8_t *curr_block_row_start =
        new_frame.GetFrameDataAtPos(rect.top_left());

    int top = rect.top();
    // The last row may have a different height, so we handle it separately.
    for (int y = 0; y < y_block_count; y++)
    {
        CompareRow(prev_block_row_start, curr_block_row_start, rect.left(),
                   rect.right(), top, top + kBlockSize, old_frame.stride(), output);
        top += kBlockSize;
        prev_block_row_start += block_y_stride;
        curr_block_row_start += block_y_stride;
    }
    CompareRow(prev_block_row_start, curr_block_row_start, rect.left(),
               rect.right(), top, top + last_y_block_height, old_frame.stride(),
               output);
}
}  // namespace

DesktopCapturerDifferWrapper::DesktopCapturerDifferWrapper(
    std::unique_ptr<DesktopCapturer> base_capturer)
    : base_capturer_(std::move(base_capturer))
{
    OCTK_DCHECK(base_capturer_);
}

DesktopCapturerDifferWrapper::~DesktopCapturerDifferWrapper() {}

void DesktopCapturerDifferWrapper::start(DesktopCapturer::Callback *callback)
{
    callback_ = callback;
    base_capturer_->start(this);
}

void DesktopCapturerDifferWrapper::SetSharedMemoryFactory(
    std::unique_ptr<SharedMemoryFactory> shared_memory_factory)
{
    base_capturer_->SetSharedMemoryFactory(std::move(shared_memory_factory));
}

void DesktopCapturerDifferWrapper::CaptureFrame()
{
    base_capturer_->CaptureFrame();
}

void DesktopCapturerDifferWrapper::SetExcludedWindow(WindowId window)
{
    base_capturer_->SetExcludedWindow(window);
}

bool DesktopCapturerDifferWrapper::GetSourceList(SourceList *sources)
{
    return base_capturer_->GetSourceList(sources);
}

bool DesktopCapturerDifferWrapper::SelectSource(SourceId id)
{
    return base_capturer_->SelectSource(id);
}

bool DesktopCapturerDifferWrapper::FocusOnSelectedSource()
{
    return base_capturer_->FocusOnSelectedSource();
}

bool DesktopCapturerDifferWrapper::IsOccluded(const DesktopVector &pos)
{
    return base_capturer_->IsOccluded(pos);
}

#if defined(OCTK_USE_GIO)
DesktopCaptureMetadata DesktopCapturerDifferWrapper::GetMetadata() {
  return base_capturer_->GetMetadata();
}
#endif  // defined(OCTK_USE_GIO)

void DesktopCapturerDifferWrapper::OnCaptureResult(
    Result result,
    std::unique_ptr<DesktopFrame> input_frame)
{
    int64_t start_time_nanos = DateTime::TimeNanos();
    if (!input_frame)
    {
        callback_->OnCaptureResult(result, nullptr);
        return;
    }
    OCTK_DCHECK(result == Result::SUCCESS);

    std::unique_ptr<SharedDesktopFrame> frame = SharedDesktopFrame::Wrap(std::move(input_frame));
    if (last_frame_ && (last_frame_->size().width() != frame->size().width() ||
                        last_frame_->size().height() != frame->size().height() ||
                        last_frame_->stride() != frame->stride()))
    {
        last_frame_.reset();
    }

    if (last_frame_)
    {
        DesktopRegion hints;
        hints.Swap(frame->mutable_updated_region());
        for (DesktopRegion::Iterator it(hints); !it.IsAtEnd(); it.Advance())
        {
            CompareFrames(*last_frame_, *frame, it.rect(),
                          frame->mutable_updated_region());
        }
    }
    else
    {
        frame->mutable_updated_region()->SetRect(
            DesktopRect::MakeSize(frame->size()));
    }
    last_frame_ = frame->Share();

    frame->set_capture_time_ms(frame->capture_time_ms() +
                               (DateTime::TimeNanos() - start_time_nanos) /
                               DateTime::kNSecsPerMSec);
    callback_->OnCaptureResult(result, std::move(frame));
}
OCTK_END_NAMESPACE
