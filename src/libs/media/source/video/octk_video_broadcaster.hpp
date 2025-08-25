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

#ifndef _OCTK_VIDEO_BROADCASTER_HPP
#define _OCTK_VIDEO_BROADCASTER_HPP

#include <octk_media_stream_interface.hpp>
#include <octk_video_source_interface.hpp>
#include <octk_video_frame_buffer.hpp>
#include <octk_video_source_base.hpp>
#include <octk_optional.hpp>

#include <mutex>

OCTK_BEGIN_NAMESPACE

/**
 * @brief VideoBroadcaster broadcast video frames to sinks and combines VideoSinkWants from its sinks.
 * @details It does that by implementing VideoSourceInterface and VideoSinkInterface.
 *      The class is threadsafe; methods may be called on any thread.
 *      This is needed because VideoStreamEncoder calls AddOrUpdateSink both on the worker thread and on the
 *      encoder task queue.
 */
class VideoBroadcaster : public VideoSourceBase, public VideoSinkInterface<VideoFrame>
{
public:
    VideoBroadcaster();
    ~VideoBroadcaster() override;

    /**
     * @brief Adds a new, or updates an already existing sink.
     * @details If the sink is new and processConstraints has been called previously, the new sink's
     *      onConstraintsCalled method will be invoked with the most recent constraints.
     * @param sink
     * @param wants
     */
    void addOrUpdateSink(VideoSinkInterface<VideoFrame> *sink, const VideoSinkWants &wants) override;
    void removeSink(VideoSinkInterface<VideoFrame> *sink) override;

    /**
     * @return Returns true if the next frame will be delivered to at least one sink.
     */
    bool frameWanted() const;

    /**
     * @return Returns VideoSinkWants a source is requested to fulfill.
     *      They are aggregated by all VideoSinkWants from all sinks.
     */
    VideoSinkWants wants() const;

    // This method ensures that if a sink sets rotationApplied == true,
    // it will never receive a frame with pending rotation. Our caller
    // may pass in frames without precise synchronization with changes
    // to the VideoSinkWants.
    void onFrame(const VideoFrame &frame) override;

    void onDiscardedFrame() override;

    // Called on the network thread when constraints change. Forwards the
    // constraints to sinks added with AddOrUpdateSink via OnConstraintsChanged.
    void processConstraints(const VideoTrackSourceConstraints &constraints);

protected:
    void UpdateWants() OCTK_ATTRIBUTE_EXCLUSIVE_LOCKS_REQUIRED(mSinksAndWantsMutex);
    const std::shared_ptr<VideoFrameBuffer> &GetBlackFrameBuffer(int width, int height)
        OCTK_ATTRIBUTE_EXCLUSIVE_LOCKS_REQUIRED(mSinksAndWantsMutex);

    mutable std::mutex mSinksAndWantsMutex;

    VideoSinkWants mCurrentWants OCTK_ATTRIBUTE_GUARDED_BY(mSinksAndWantsMutex);
    std::shared_ptr<VideoFrameBuffer> black_frame_buffer_;
    bool mPreviousFrameSentToAllSinks OCTK_ATTRIBUTE_GUARDED_BY(mSinksAndWantsMutex) = true;
    Optional<VideoTrackSourceConstraints> last_constraints_ OCTK_ATTRIBUTE_GUARDED_BY(mSinksAndWantsMutex);
};

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_BROADCASTER_HPP
