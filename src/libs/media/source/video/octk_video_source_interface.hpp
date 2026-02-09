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

#ifndef _OCTK_VIDEO_SOURCE_INTERFACE_HPP
#define _OCTK_VIDEO_SOURCE_INTERFACE_HPP

#include <octk_video_sink_interface.hpp>
#include <octk_media_global.hpp>
#include <octk_optional.hpp>
#include <octk_limits.hpp>

#include <vector>

OCTK_BEGIN_NAMESPACE

// VideoSinkWants is used for notifying the source of properties a video frame
// should have when it is delivered to a certain sink.
struct OCTK_MEDIA_API VideoSinkWants
{
    struct FrameSize
    {
        FrameSize(int width, int height)
            : width(width)
            , height(height)
        {
        }
        FrameSize(const FrameSize &) = default;
        ~FrameSize() = default;

        int width;
        int height;
    };

    VideoSinkWants();
    VideoSinkWants(const VideoSinkWants &);
    ~VideoSinkWants();

    // Tells the source whether the sink wants frames with rotation applied.
    // By default, any rotation must be applied by the sink.
    bool rotationApplied = false;

    // Tells the source that the sink only wants black frames.
    bool blackFrames = false;

    // Tells the source the maximum number of pixels the sink wants.
    int maxPixelCount = utils::numericMax<int>();
    // Tells the source the desired number of pixels the sinks wants. This will
    // typically be used when stepping the resolution up again when conditions
    // have improved after an earlier downgrade. The source should select the
    // closest resolution to this pixel count, but if maxPixelCount is set, it
    // still sets the absolute upper bound.
    Optional<int> targetPixelCount;
    // Tells the source the maximum framerate the sink wants.
    int maxFramerateFps = utils::numericMax<int>();

    // Tells the source that the sink wants width and height of the video frames
    // to be divisible by `resolution_alignment`.
    // For example: With I420, this value would be a multiple of 2.
    // Note that this field is unrelated to any horizontal or vertical stride
    // requirements the encoder has on the incoming video frame buffers.
    int resolutionAlignment = 1;

    // The resolutions that sink is configured to consume. If the sink is an
    // encoder this is what the encoder is configured to encode. In singlecast we
    // only encode one resolution, but in simulcast and SVC this can mean multiple
    // resolutions per frame.
    //
    // The sink is always configured to consume a subset of the
    // webrtc::VideoFrame's resolution. In the case of encoding, we usually encode
    // at webrtc::VideoFrame's resolution but this may not always be the case due
    // to scaleResolutionDownBy or turning off simulcast or SVC layers.
    //
    // For example, we may capture at 720p and due to adaptation (e.g. applying
    // `maxPixelCount` constraints) create webrtc::VideoFrames of size 480p, but
    // if we do scaleResolutionDownBy:2 then the only resolution we end up
    // encoding is 240p. In this case we still need to provide webrtc::VideoFrames
    // of size 480p but we can optimize internal buffers for 240p, avoiding
    // downsampling to 480p if possible.
    //
    // Note that the `resolutions` can change while frames are in flight and
    // should only be used as a hint when constructing the webrtc::VideoFrame.
    std::vector<FrameSize> resolutions;

    // This is the resolution requested by the user using RtpEncodingParameters,
    // which is the maximum `scale_resolution_down_by` value of any encoding.
    Optional<FrameSize> requestedResolution;

    // `isActive` : Is this VideoSinkWants from an encoder that is encoding any
    // layer. IF YES, it will affect how the VideoAdapter will choose to
    // prioritize the onOutputFormatRequest vs. requestedResolution. IF NO,
    // VideoAdapter consider this VideoSinkWants as a passive listener (e.g a
    // VideoRenderer or a VideoEncoder that is not currently actively encoding).
    bool isActive = false;

    // This sub-struct contains information computed by VideoBroadcaster
    // that aggregates several VideoSinkWants (and sends them to
    // AdaptedVideoTrackSource).
    struct Aggregates
    {
        // `any_active_without_requested_resolution` is set by VideoBroadcaster
        // when aggregating sink wants if there exists any sink (encoder) that is
        // active but has not set the `requestedResolution`, i.e is relying on
        // onOutputFormatRequest to handle encode resolution.
        bool anyActiveWithoutRequestedResolution = false;
    };
    Optional<Aggregates> aggregates;
};

inline bool operator==(const VideoSinkWants::FrameSize &a, const VideoSinkWants::FrameSize &b)
{
    return a.width == b.width && a.height == b.height;
}

inline bool operator!=(const VideoSinkWants::FrameSize &a, const VideoSinkWants::FrameSize &b)
{
    return !(a == b);
}

template <typename T>
class VideoSourceInterface
{
public:
    virtual ~VideoSourceInterface() = default;

    virtual void addOrUpdateSink(VideoSinkInterface<T> *sink, const VideoSinkWants &wants) = 0;
    // RemoveSink must guarantee that at the time the method returns,
    // there is no current and no future calls to VideoSinkInterface::OnFrame.
    virtual void removeSink(VideoSinkInterface<T> *sink) = 0;

    // Request underlying source to capture a new frame.
    // TODO: make pure virtual once downstream projects adapt.
    virtual void requestRefreshFrame() { }
};
OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_SOURCE_INTERFACE_HPP
