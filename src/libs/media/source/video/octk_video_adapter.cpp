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

#include <octk_video_adapter.hpp>
#include <octk_date_time.hpp>
#include <octk_logging.hpp>
#include <octk_numeric.hpp>
#include <octk_checks.hpp>
#include <octk_limits.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <limits>
#include <string>
#include <cmath>
//
// #include "api/video/resolution.h"
// #include "api/video/video_source_interface.h"
// #include "media/base/video_common.h"
// #include "rtc_base/checks.h"
// #include "rtc_base/logging.h"
// #include "rtc_base/strings/string_builder.h"
// #include "rtc_base/synchronization/mutex.h"
// #include "rtc_base/time_utils.h"

OCTK_BEGIN_NAMESPACE

namespace
{

struct Fraction
{
    int numerator;
    int denominator;

    void DivideByGcd()
    {
        int g = utils::gcd(numerator, denominator);
        numerator /= g;
        denominator /= g;
    }

    // Determines number of output pixels if both width and height of an input of
    // `input_pixels` pixels is scaled with the fraction numerator / denominator.
    int scale_pixel_count(int input_pixels)
    {
        return (numerator * numerator * static_cast<int64_t>(input_pixels)) / (denominator * denominator);
    }
};

// Round `value_to_round` to a multiple of `multiple`. Prefer rounding upwards,
// but never more than `max_value`.
int roundUp(int value_to_round, int multiple, int max_value)
{
    const int rounded_value = (value_to_round + multiple - 1) / multiple * multiple;
    return rounded_value <= max_value ? rounded_value : (max_value / multiple * multiple);
}

// Generates a scale factor that makes `input_pixels` close to `target_pixels`,
// but no higher than `max_pixels`.
Fraction FindScale(int input_width, int input_height, int target_pixels, int max_pixels)
{
    // This function only makes sense for a positive target.
    OCTK_DCHECK_GT(target_pixels, 0);
    OCTK_DCHECK_GT(max_pixels, 0);
    OCTK_DCHECK_GE(max_pixels, target_pixels);

    const int input_pixels = input_width * input_height;

    // Don't scale up original.
    if (target_pixels >= input_pixels)
    {
        return Fraction{1, 1};
    }

    Fraction current_scale = Fraction{1, 1};
    Fraction best_scale = Fraction{1, 1};

    // start scaling down by 2/3 depending on `input_width` and `input_height`.
    if (input_width % 3 == 0 && input_height % 3 == 0)
    {
        // 2/3 (then alternates 3/4, 2/3, 3/4,...).
        current_scale = Fraction{6, 6};
    }
    if (input_width % 9 == 0 && input_height % 9 == 0)
    {
        // 2/3, 2/3 (then alternates 3/4, 2/3, 3/4,...).
        current_scale = Fraction{36, 36};
    }

    // The minimum (absolute) difference between the number of output pixels and
    // the target pixel count.
    int min_pixel_diff = utils::numericMax<int>();
    if (input_pixels <= max_pixels)
    {
        // start condition for 1/1 case, if it is less than max.
        min_pixel_diff = std::abs(input_pixels - target_pixels);
    }

    // Alternately scale down by 3/4 and 2/3. This results in fractions which are
    // effectively scalable. For instance, starting at 1280x720 will result in
    // the series (3/4) => 960x540, (1/2) => 640x360, (3/8) => 480x270,
    // (1/4) => 320x180, (3/16) => 240x125, (1/8) => 160x90.
    while (current_scale.scale_pixel_count(input_pixels) > target_pixels)
    {
        if (current_scale.numerator % 3 == 0 && current_scale.denominator % 2 == 0)
        {
            // Multiply by 2/3.
            current_scale.numerator /= 3;
            current_scale.denominator /= 2;
        }
        else
        {
            // Multiply by 3/4.
            current_scale.numerator *= 3;
            current_scale.denominator *= 4;
        }

        int output_pixels = current_scale.scale_pixel_count(input_pixels);
        if (output_pixels <= max_pixels)
        {
            int diff = std::abs(target_pixels - output_pixels);
            if (diff < min_pixel_diff)
            {
                min_pixel_diff = diff;
                best_scale = current_scale;
            }
        }
    }
    best_scale.DivideByGcd();

    return best_scale;
}

Optional<std::pair<int, int>> Swap(const Optional<std::pair<int, int>> &in)
{
    if (!in)
    {
        return utils::nullopt;
    }
    return std::make_pair(in->second, in->first);
}
} // namespace

VideoAdapter::VideoAdapter(int source_resolution_alignment)
    : mFramesIn(0)
    , mFramesOut(0)
    , mFramesScaled(0)
    , adaptionChanges(0)
    , mPreviousWidth(0)
    , mPreviousHeight(0)
    , mSourceResolutionAlignment(source_resolution_alignment)
    , mResolutionAlignment(source_resolution_alignment)
    , mResolutionRequestTargetPixelCount(utils::numericMax<int>())
    , mResolutionRequestMaxPixelCount(utils::numericMax<int>())
    , mMaxFramerateRequest(utils::numericMax<int>())
{
}

VideoAdapter::VideoAdapter()
    : VideoAdapter(1)
{
}

VideoAdapter::~VideoAdapter() { }

bool VideoAdapter::isDropFrame(int64_t inTimestampNSecs)
{
    int maxFps = mMaxFramerateRequest;
    if (mOutputFormatRequest.maxFps)
    {
        maxFps = utils::mathMin(maxFps, *mOutputFormatRequest.maxFps);
    }

    mFramerateController.SetMaxFramerate(maxFps);
    return mFramerateController.ShouldDropFrame(inTimestampNSecs);
}

bool VideoAdapter::adaptFrameResolution(int inWidth,
                                        int inHeight,
                                        int64_t inTimestampNSecs,
                                        int *croppedWidth,
                                        int *croppedHeight,
                                        int *outWidth,
                                        int *outHeight)
{
    std::lock_guard<std::mutex> lock(mMutex);
    ++mFramesIn;

    // The max output pixel count is the minimum of the requests from
    // onOutputFormatRequest and OnResolutionFramerateRequest.
    int maxPixelCount = mResolutionRequestMaxPixelCount;

    // Select target aspect ratio and max pixel count depending on input frame
    // orientation.
    Optional<std::pair<int, int>> targetAspectRatio;
    if (inWidth > inHeight)
    {
        targetAspectRatio = mOutputFormatRequest.targetLandscapeAspectRatio;
        if (mOutputFormatRequest.maxLandscapePixelCount)
        {
            maxPixelCount = utils::mathMin(maxPixelCount, *mOutputFormatRequest.maxLandscapePixelCount);
        }
    }
    else
    {
        targetAspectRatio = mOutputFormatRequest.targetPortraitAspectRatio;
        if (mOutputFormatRequest.maxPortraitPixelCount)
        {
            maxPixelCount = utils::mathMin(maxPixelCount, *mOutputFormatRequest.maxPortraitPixelCount);
        }
    }

    int target_pixel_count = utils::mathMin(mResolutionRequestTargetPixelCount, maxPixelCount);

    // Drop the input frame if necessary.
    if (maxPixelCount <= 0 || isDropFrame(inTimestampNSecs))
    {
        // Show VAdapt log every 90 frames dropped. (3 seconds)
        if ((mFramesIn - mFramesOut) % 90 == 0)
        {
            // TODO(fbarchard): Reduce to LS_VERBOSE when adapter info is not needed
            // in default calls.
            OCTK_INFO() << "VAdapt Drop Frame: scaled " << mFramesScaled << " / out " << mFramesOut << " / in "
                        << mFramesIn << " Changes: " << adaptionChanges << " Input: " << inWidth << "x" << inHeight
                        << " timestamp: " << inTimestampNSecs << " Output fps: " << mMaxFramerateRequest << "/"
                        << mOutputFormatRequest.maxFps.value_or(-1) << " alignment: " << mResolutionAlignment;
        }

        // Drop frame.
        return false;
    }

    // Calculate how the input should be cropped.
    if (!targetAspectRatio || targetAspectRatio->first <= 0 || targetAspectRatio->second <= 0)
    {
        *croppedWidth = inWidth;
        *croppedHeight = inHeight;
    }
    else
    {
        const float requested_aspect = targetAspectRatio->first / static_cast<float>(targetAspectRatio->second);
        *croppedWidth = utils::mathMin(inWidth, static_cast<int>(inHeight * requested_aspect));
        *croppedHeight = utils::mathMin(inHeight, static_cast<int>(inWidth / requested_aspect));
    }
    const Fraction scale = FindScale(*croppedWidth, *croppedHeight, target_pixel_count, maxPixelCount);
    // Adjust cropping slightly to get correctly aligned output size and a perfect scale factor.
    *croppedWidth = roundUp(*croppedWidth, scale.denominator * mResolutionAlignment, inWidth);
    *croppedHeight = roundUp(*croppedHeight, scale.denominator * mResolutionAlignment, inHeight);
    OCTK_DCHECK_EQ(0, *croppedWidth % scale.denominator);
    OCTK_DCHECK_EQ(0, *croppedHeight % scale.denominator);

    // Calculate output size.
    *outWidth = *croppedWidth / scale.denominator * scale.numerator;
    *outHeight = *croppedHeight / scale.denominator * scale.numerator;
    OCTK_DCHECK_EQ(0, *outWidth % mResolutionAlignment);
    OCTK_DCHECK_EQ(0, *outHeight % mResolutionAlignment);

    // Lastly, make the output size fit within the resolution restrictions as specified by `mScaleResolutionDownTo`.
    // This does not modify aspect ratio or cropping, only `outWidth` and `outHeight`.
    if (mScaleResolutionDownTo.has_value())
    {
        // Make frame and "scale to" have matching orientation.
        Resolution scale_resolution_down_to = mScaleResolutionDownTo.value();
        if ((*outWidth < *outHeight) != (mScaleResolutionDownTo->width < mScaleResolutionDownTo->height))
        {
            scale_resolution_down_to = {mScaleResolutionDownTo->height, mScaleResolutionDownTo->width};
        }
        // Downscale by smallest scaling factor, if necessary.
        if (*outWidth > 0 && *outHeight > 0 &&
            (scale_resolution_down_to.width < *outWidth || scale_resolution_down_to.height < *outHeight))
        {
            double scale_factor = utils::mathMin(scale_resolution_down_to.width / static_cast<double>(*outWidth),
                                                 scale_resolution_down_to.height / static_cast<double>(*outHeight));
            *outWidth =
                roundUp(std::round(*outWidth * scale_factor), mResolutionAlignment, scale_resolution_down_to.width);
            *outHeight =
                roundUp(std::round(*outHeight * scale_factor), mResolutionAlignment, scale_resolution_down_to.height);
            OCTK_DCHECK_EQ(0, *outWidth % mResolutionAlignment);
            OCTK_DCHECK_EQ(0, *outHeight % mResolutionAlignment);
        }
    }

    ++mFramesOut;
    if (scale.numerator != scale.denominator)
    {
        ++mFramesScaled;
    }

    if (mPreviousWidth && (mPreviousWidth != *outWidth || mPreviousHeight != *outHeight))
    {
        ++adaptionChanges;
        OCTK_INFO() << "Frame size changed: scaled " << mFramesScaled << " / out " << mFramesOut << " / in "
                    << mFramesIn << " Changes: " << adaptionChanges << " Input: " << inWidth << "x" << inHeight
                    << " Scale: " << scale.numerator << "/" << scale.denominator << " Output: " << *outWidth << "x"
                    << *outHeight << " fps: " << mMaxFramerateRequest << "/" << mOutputFormatRequest.maxFps.value_or(-1)
                    << " alignment: " << mResolutionAlignment;
    }

    mPreviousWidth = *outWidth;
    mPreviousHeight = *outHeight;

    return true;
}
/*
void VideoAdapter::onOutputFormatRequest(const Optional<VideoFormat> &format)
{
    Optional<std::pair<int, int>> targetAspectRatio;
    Optional<int> maxPixelCount;
    Optional<int> maxFps;
    if (format)
    {
        targetAspectRatio = std::make_pair(format->width, format->height);
        maxPixelCount = format->width * format->height;
        if (format->interval > 0)
        {
            maxFps = time_utils::KNSecsPerSec / format->interval;
        }
    }
    onOutputFormatRequest(targetAspectRatio, maxPixelCount, maxFps);
}*/

void VideoAdapter::onOutputFormatRequest(const Optional<std::pair<int, int>> &targetAspectRatio,
                                         const Optional<int> &maxPixelCount,
                                         const Optional<int> &maxFps)
{
    Optional<std::pair<int, int>> targetLandscapeAspectRatio;
    Optional<std::pair<int, int>> targetPortraitAspectRatio;
    if (targetAspectRatio && targetAspectRatio->first > 0 && targetAspectRatio->second > 0)
    {
        // Maintain input orientation.
        const int max_side = utils::mathMax(targetAspectRatio->first, targetAspectRatio->second);
        const int min_side = utils::mathMin(targetAspectRatio->first, targetAspectRatio->second);
        targetLandscapeAspectRatio = std::make_pair(max_side, min_side);
        targetPortraitAspectRatio = std::make_pair(min_side, max_side);
    }
    onOutputFormatRequest(targetLandscapeAspectRatio, maxPixelCount, targetPortraitAspectRatio, maxPixelCount, maxFps);
}

void VideoAdapter::onOutputFormatRequest(const Optional<std::pair<int, int>> &targetLandscapeAspectRatio,
                                         const Optional<int> &maxLandscapePixelCount,
                                         const Optional<std::pair<int, int>> &targetPortraitAspectRatio,
                                         const Optional<int> &maxPortraitPixelCount,
                                         const Optional<int> &maxFps)
{
    std::lock_guard<std::mutex> lock(mMutex);

    // modified by chengxuewen, 20241216
#if 0
    OutputFormatRequest request = {
        .targetLandscapeAspectRatio = targetLandscapeAspectRatio,
        .maxLandscapePixelCount = maxLandscapePixelCount,
        .targetPortraitAspectRatio = targetPortraitAspectRatio,
        .maxPortraitPixelCount = maxPortraitPixelCount,
        .maxFps = maxFps};
#else
    OutputFormatRequest request = {};
    request.targetLandscapeAspectRatio = targetLandscapeAspectRatio;
    request.maxLandscapePixelCount = maxLandscapePixelCount;
    request.targetPortraitAspectRatio = targetPortraitAspectRatio;
    request.maxPortraitPixelCount = maxPortraitPixelCount;
    request.maxFps = maxFps;
#endif
    // end modified by chengxuewen, 20241216

    if (mStashedOutputFormatRequest)
    {
        // Save the output format request for later use in case the encoder making
        // this call would become active, because currently all active encoders use
        // scale_resolution_down_to instead.
        mStashedOutputFormatRequest = request;
        OCTK_INFO() << "Stashing onOutputFormatRequest: " << mStashedOutputFormatRequest->toString();
    }
    else
    {
        mOutputFormatRequest = request;
        OCTK_INFO() << "Setting mOutputFormatRequest: " << mOutputFormatRequest.toString();
    }

    mFramerateController.Reset();
}

void VideoAdapter::OnSinkWants(const VideoSinkWants &sink_wants)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mResolutionRequestMaxPixelCount = sink_wants.maxPixelCount;
    mResolutionRequestTargetPixelCount = sink_wants.target_pixel_count.value_or(mResolutionRequestMaxPixelCount);
    mMaxFramerateRequest = sink_wants.max_framerate_fps;
    mResolutionAlignment = utils::lcm(mSourceResolutionAlignment, sink_wants.resolution_alignment);
    // Convert from Optional<rtc::VideoSinkWants::FrameSize> to
    // Optional<webrtc::Resolution>. Both are {int,int}.
    mScaleResolutionDownTo = utils::nullopt;
    if (sink_wants.requestedResolution.has_value())
    {
        // modified by chengxuewen, 20241216
        Resolution resolution = {sink_wants.requestedResolution->width, sink_wants.requestedResolution->height};
        // resolution.width = sink_wants.requestedResolution->width;
        // resolution.height = sink_wants.requestedResolution->height;
        mScaleResolutionDownTo = resolution;
        // modified by chengxuewen, 20241216
    }

    // If scale_resolution_down_to is used, and there are no active encoders
    // that are NOT using scale_resolution_down_to (aka newapi), then override
    // calls to onOutputFormatRequest and use values from scale_resolution_down_to
    // instead (combined with qualityscaling based on pixel counts above).
    if (!sink_wants.requestedResolution)
    {
        if (mStashedOutputFormatRequest)
        {
            // because current active_output_format_request is based on
            // scale_resolution_down_to logic, while current encoder(s) doesn't want
            // that, we have to restore the stashed request.
            OCTK_INFO() << "Unstashing onOutputFormatRequest: " << mStashedOutputFormatRequest->toString();
            mOutputFormatRequest = *mStashedOutputFormatRequest;
            mStashedOutputFormatRequest.reset();
        }
        return;
    }

    // The code below is only needed when `scale_resolution_down_to` is signalled
    // back to the video source which only happens if
    // `VideoStreamEncoderSettings::use_standard_scale_resolution_down_to` is
    // false.
    // TODO(https://crbug.com/webrtc/366284861): Delete the code below as part of
    // deleting this flag and only supporting the standard behavior.

    if (sink_wants.aggregates.has_value() && sink_wants.aggregates->any_active_without_requested_resolution)
    {
        return;
    }

    if (!mStashedOutputFormatRequest)
    {
        // The active output format request is about to be cleared due to
        // request_resolution. We need to save it for later use in case the encoder
        // which doesn't use request_resolution logic become active in the future.
        mStashedOutputFormatRequest = mOutputFormatRequest;
        OCTK_INFO() << "Stashing onOutputFormatRequest: " << mStashedOutputFormatRequest->toString();
    }

    // Clear the output format request, `mScaleResolutionDownTo` will be
    // applied instead which happens inside adaptFrameResolution().
    mOutputFormatRequest = {};
}

int VideoAdapter::GetTargetPixels() const
{
    std::lock_guard<std::mutex> lock(mMutex);
    return mResolutionRequestTargetPixelCount;
}

float VideoAdapter::GetMaxFramerate() const
{
    std::lock_guard<std::mutex> lock(mMutex);
    // Minimum of `mOutputFormatRequest.maxFps` and `mMaxFramerateRequest` is
    // used to throttle frame-rate.
    int framerate = utils::mathMin(mMaxFramerateRequest, mOutputFormatRequest.maxFps.value_or(mMaxFramerateRequest));
    if (framerate == utils::numericMax<int>())
    {
        return std::numeric_limits<float>::infinity();
    }
    else
    {
        return mMaxFramerateRequest;
    }
}

std::string VideoAdapter::OutputFormatRequest::toString() const
{
    std::stringstream ss;
    ss << "[ ";
    if (targetLandscapeAspectRatio == Swap(targetPortraitAspectRatio) &&
        maxLandscapePixelCount == maxPortraitPixelCount)
    {
        if (targetLandscapeAspectRatio)
        {
            ss << targetLandscapeAspectRatio->first << "x" << targetLandscapeAspectRatio->second;
        }
        else
        {
            ss << "unset-resolution";
        }
        if (maxLandscapePixelCount)
        {
            ss << " maxPixelCount: " << *maxLandscapePixelCount;
        }
    }
    else
    {
        ss << "[ landscape: ";
        if (targetLandscapeAspectRatio)
        {
            ss << targetLandscapeAspectRatio->first << "x" << targetLandscapeAspectRatio->second;
        }
        else
        {
            ss << "unset";
        }
        if (maxLandscapePixelCount)
        {
            ss << " maxPixelCount: " << *maxLandscapePixelCount;
        }
        ss << " ] [ portrait: ";
        if (targetPortraitAspectRatio)
        {
            ss << targetPortraitAspectRatio->first << "x" << targetPortraitAspectRatio->second;
        }
        if (maxPortraitPixelCount)
        {
            ss << " maxPixelCount: " << *maxPortraitPixelCount;
        }
        ss << " ]";
    }
    ss << " maxFps: ";
    if (maxFps)
    {
        ss << *maxFps;
    }
    else
    {
        ss << "unset";
    }
    ss << " ]";
    return ss.str();
}
OCTK_END_NAMESPACE
