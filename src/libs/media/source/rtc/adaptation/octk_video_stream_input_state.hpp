//
// Created by cxw on 25-8-15.
//

#ifndef _OCTK_VIDEO_STREAM_INPUT_STATE_HPP
#define _OCTK_VIDEO_STREAM_INPUT_STATE_HPP

#include <octk_video_codec_types.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

// The source resolution, frame rate and other properties of a
// VideoStreamEncoder.
class VideoStreamInputState
{
public:
    VideoStreamInputState();

    void set_has_input(bool has_input);
    void set_frame_size_pixels(Optional<int> frame_size_pixels);
    void set_frames_per_second(int frames_per_second);
    void set_video_codec_type(VideoCodecType video_codec_type);
    void set_min_pixels_per_frame(int min_pixels_per_frame);
    void set_single_active_stream_pixels(Optional<int> single_active_stream_pixels);

    bool has_input() const;
    Optional<int> frame_size_pixels() const;
    int frames_per_second() const;
    VideoCodecType video_codec_type() const;
    int min_pixels_per_frame() const;
    Optional<int> single_active_stream_pixels() const;

    bool HasInputFrameSizeAndFramesPerSecond() const;

private:
    bool has_input_;
    Optional<int> frame_size_pixels_;
    int frames_per_second_;
    VideoCodecType video_codec_type_;
    int min_pixels_per_frame_;
    Optional<int> single_active_stream_pixels_;
};

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_STREAM_INPUT_STATE_HPP
