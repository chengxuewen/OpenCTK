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

#pragma once

#include <octk_video_source_interface.hpp>
#include <octk_framerate_controller.hpp>
#include <octk_size_base.hpp>
#include <octk_optional.hpp>

#include <utility>
#include <cstdint>
#include <string>
#include <mutex>

OCTK_BEGIN_NAMESPACE

// VideoAdapter adapts an input video frame to an output frame based on the
// specified input and output formats. The adaptation includes dropping frames
// to reduce frame rate and scaling frames.
// VideoAdapter is thread safe.
class OCTK_MEDIA_API VideoAdapter
{
public:
    VideoAdapter();
    // The source requests output frames whose width and height are divisible
    // by `source_resolution_alignment`.
    explicit VideoAdapter(int source_resolution_alignment);
    virtual ~VideoAdapter();

    VideoAdapter(const VideoAdapter &) = delete;
    VideoAdapter &operator=(const VideoAdapter &) = delete;

    // Return the adapted resolution and cropping parameters given the
    // input resolution. The input frame should first be cropped, then
    // scaled to the final output resolution. Returns true if the frame
    // should be adapted, and false if it should be dropped.
    bool adaptFrameResolution(int inWidth,
                              int inHeight,
                              int64_t inTimestampNSecs,
                              int *croppedWidth,
                              int *croppedHeight,
                              int *outWidth,
                              int *outHeight);
    //    RTC_LOCKS_EXCLUDED(mMutex);

    // DEPRECATED. Please use onOutputFormatRequest below.
    // TODO(asapersson): Remove this once it is no longer used.
    // Requests the output frame size and frame interval from
    // `adaptFrameResolution` to not be larger than `format`. Also, the input
    // frame size will be cropped to match the requested aspect ratio. The
    // requested aspect ratio is orientation agnostic and will be adjusted to
    // maintain the input orientation, so it doesn't matter if e.g. 1280x720 or
    // 720x1280 is requested.
    // Note: Should be called from the source only.
    //    void onOutputFormatRequest(const Optional<VideoFormat> &format);
    //    RTC_LOCKS_EXCLUDED(mMutex);

    // Requests output frame size and frame interval from `adaptFrameResolution`.
    // `targetAspectRatio`: The input frame size will be cropped to match the
    // requested aspect ratio. The aspect ratio is orientation agnostic and will
    // be adjusted to maintain the input orientation (i.e. it doesn't matter if
    // e.g. <1280,720> or <720,1280> is requested).
    // `maxPixelCount`: The maximum output frame size.
    // `maxFps`: The maximum output framerate.
    // Note: Should be called from the source only.
    void onOutputFormatRequest(const Optional<std::pair<int, int>> &targetAspectRatio,
                               const Optional<int> &maxPixelCount,
                               const Optional<int> &maxFps);
    //    RTC_LOCKS_EXCLUDED(mMutex);

    // Same as above, but allows setting two different target aspect ratios
    // depending on incoming frame orientation. This gives more fine-grained
    // control and can e.g. be used to force landscape video to be cropped to
    // portrait video.
    void onOutputFormatRequest(const Optional<std::pair<int, int>> &targetLandscapeAspectRatio,
                               const Optional<int> &maxLandscapePixelCount,
                               const Optional<std::pair<int, int>> &targetPortraitAspectRatio,
                               const Optional<int> &maxPortraitPixelCount,
                               const Optional<int> &maxFps);
    //    RTC_LOCKS_EXCLUDED(mMutex);

    // Requests the output frame size from `adaptFrameResolution` to have as close
    // as possible to `sink_wants.target_pixel_count` pixels (if set)
    // but no more than `sink_wants.maxPixelCount`.
    // `sink_wants.max_framerate_fps` is essentially analogous to
    // `sink_wants.maxPixelCount`, but for framerate rather than resolution.
    // Set `sink_wants.maxPixelCount` and/or `sink_wants.max_framerate_fps` to
    // std::numeric_limit<int>::max() if no upper limit is desired.
    // The sink resolution alignment requirement is given by
    // `sink_wants.resolution_alignment`.
    // Note: Should be called from the sink only.
    void OnSinkWants(const VideoSinkWants &sink_wants);
    //    RTC_LOCKS_EXCLUDED(mMutex);

    // Returns maximum image area, which shouldn't impose any adaptations.
    // Can return `numeric_limits<int>::max()` if no limit is set.
    int GetTargetPixels() const;

    // Returns current frame-rate limit.
    // Can return `numeric_limits<float>::infinity()` if no limit is set.
    float GetMaxFramerate() const;

private:
    // Determine if frame should be dropped based on input fps and requested fps.
    bool isDropFrame(int64_t inTimestampNSecs);
    //    RTC_EXCLUSIVE_LOCKS_REQUIRED(mMutex);

    int mFramesIn OCTK_ATTRIBUTE_GUARDED_BY(mMutex);       // Number of input frames.
    int mFramesOut OCTK_ATTRIBUTE_GUARDED_BY(mMutex);      // Number of output frames.
    int mFramesScaled OCTK_ATTRIBUTE_GUARDED_BY(mMutex);   // Number of frames scaled.
    int adaptionChanges OCTK_ATTRIBUTE_GUARDED_BY(mMutex); // Number of changes in scale factor.
    int mPreviousWidth OCTK_ATTRIBUTE_GUARDED_BY(mMutex);  // Previous adapter output width.
    int mPreviousHeight OCTK_ATTRIBUTE_GUARDED_BY(mMutex); // Previous adapter output height.

    // The fixed source resolution alignment requirement.
    const int mSourceResolutionAlignment;
    // The currently applied resolution alignment, as given by the requirements:
    //  - the fixed `mSourceResolutionAlignment`; and
    //  - the latest `sink_wants.resolution_alignment`.
    int mResolutionAlignment OCTK_ATTRIBUTE_GUARDED_BY(mMutex);

    // Max number of pixels/fps requested via calls to onOutputFormatRequest,
    // OnResolutionFramerateRequest respectively.
    // The adapted output format is the minimum of these.
    struct OutputFormatRequest
    {
        Optional<std::pair<int, int>> targetLandscapeAspectRatio;
        Optional<int> maxLandscapePixelCount;
        Optional<std::pair<int, int>> targetPortraitAspectRatio;
        Optional<int> maxPortraitPixelCount;
        Optional<int> maxFps;

        // For logging.
        std::string toString() const;
    };

    OutputFormatRequest mOutputFormatRequest OCTK_ATTRIBUTE_GUARDED_BY(mMutex);
    int mResolutionRequestTargetPixelCount OCTK_ATTRIBUTE_GUARDED_BY(mMutex);
    int mResolutionRequestMaxPixelCount OCTK_ATTRIBUTE_GUARDED_BY(mMutex);
    int mMaxFramerateRequest OCTK_ATTRIBUTE_GUARDED_BY(mMutex);
    Optional<Resolution> mScaleResolutionDownTo OCTK_ATTRIBUTE_GUARDED_BY(mMutex);

    // Stashed OutputFormatRequest that is used to save value of
    // onOutputFormatRequest in case all active encoders are using
    // scale_resolution_down_to. I.e when all active encoders are using
    // scale_resolution_down_to, the call to onOutputFormatRequest is ignored
    // and the value from scale_resolution_down_to is used instead (to scale/crop
    // frame). This allows for an application to only use
    // RtpEncodingParameters::request_resolution and get the same behavior as if
    // it had used VideoAdapter::onOutputFormatRequest.
    Optional<OutputFormatRequest> mStashedOutputFormatRequest OCTK_ATTRIBUTE_GUARDED_BY(mMutex);

    FramerateController mFramerateController OCTK_ATTRIBUTE_GUARDED_BY(mMutex);

    // The critical section to protect the above variables.
    mutable std::mutex mMutex;
};

OCTK_END_NAMESPACE