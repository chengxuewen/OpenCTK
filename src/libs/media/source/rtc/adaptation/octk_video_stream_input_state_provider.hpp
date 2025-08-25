//
// Created by cxw on 25-8-15.
//

#ifndef _OCTK_VIDEO_STREAM_INPUT_STATE_PROVIDER_HPP
#define _OCTK_VIDEO_STREAM_INPUT_STATE_PROVIDER_HPP

#include <octk_video_stream_encoder_observer.hpp>
#include <octk_video_stream_input_state.hpp>
#include <octk_encoder_settings.hpp>
#include <octk_mutex.hpp>

OCTK_BEGIN_NAMESPACE

class VideoStreamInputStateProvider
{
public:
    VideoStreamInputStateProvider(VideoStreamEncoderObserver *frame_rate_provider);
    virtual ~VideoStreamInputStateProvider();

    void OnHasInputChanged(bool has_input);
    void OnFrameSizeObserved(int frame_size_pixels);
    void OnEncoderSettingsChanged(EncoderSettings encoder_settings);

    virtual VideoStreamInputState InputState();

private:
    Mutex mutex_;
    VideoStreamEncoderObserver *const frame_rate_provider_;
    VideoStreamInputState input_state_ OCTK_ATTRIBUTE_GUARDED_BY(mutex_);
};

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_STREAM_INPUT_STATE_PROVIDER_HPP
