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

#ifndef _OCTK_SCREEN_CAPTURE_FRAME_QUEUE_HPP
#define _OCTK_SCREEN_CAPTURE_FRAME_QUEUE_HPP

#include <octk_media_global.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

// Represents a queue of reusable video frames. Provides access to the 'current'
// frame - the frame that the caller is working with at the moment, and to the
// 'previous' frame - the predecessor of the current frame swapped by
// MoveTonextFrame() call, if any.
//
// The caller is expected to (re)allocate frames if current_frame() returns
// NULL. The caller can mark all frames in the queue for reallocation (when,
// say, frame dimensions change). The queue records which frames need updating
// which the caller can query.
//
// Frame consumer is expected to never hold more than kQueueLength frames
// created by this function and it should release the earliest one before trying
// to capture a new frame (i.e. before MoveTonextFrame() is called).
template <typename FrameType>
class ScreenCaptureFrameQueue
{
public:
    ScreenCaptureFrameQueue() = default;
    ~ScreenCaptureFrameQueue() = default;

    ScreenCaptureFrameQueue(const ScreenCaptureFrameQueue &) = delete;
    ScreenCaptureFrameQueue &operator=(const ScreenCaptureFrameQueue &) = delete;

    // Moves to the next frame in the queue, moving the 'current' frame to become
    // the 'previous' one.
    void MoveTonextFrame() { current_ = (current_ + 1) % kQueueLength; }

    // Replaces the current frame with a new one allocated by the caller. The
    // existing frame (if any) is destroyed. Takes ownership of `frame`.
    void ReplaceCurrentFrame(std::unique_ptr<FrameType> frame)
    {
        frames_[current_] = std::move(frame);
    }

    // Marks all frames obsolete and resets the previous frame pointer. No
    // frames are freed though as the caller can still access them.
    void Reset()
    {
        for (int i = 0; i < kQueueLength; i++)
        {
            frames_[i].reset();
        }
        current_ = 0;
    }

    FrameType *current_frame() const { return frames_[current_].get(); }

    FrameType *previous_frame() const
    {
        return frames_[(current_ + kQueueLength - 1) % kQueueLength].get();
    }

private:
    // Index of the current frame.
    int current_ = 0;

    static const int kQueueLength = 2;
    std::unique_ptr<FrameType> frames_[kQueueLength];
};
OCTK_END_NAMESPACE

#endif  // _OCTK_SCREEN_CAPTURE_FRAME_QUEUE_HPP
