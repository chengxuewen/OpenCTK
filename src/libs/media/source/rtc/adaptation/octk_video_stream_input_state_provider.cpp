//
// Created by cxw on 25-8-15.
//

#include <octk_video_stream_input_state_provider.hpp>
#include <octk_video_stream_adapter.hpp>

OCTK_BEGIN_NAMESPACE

VideoStreamInputStateProvider::VideoStreamInputStateProvider(VideoStreamEncoderObserver *frame_rate_provider)
    : frame_rate_provider_(frame_rate_provider)
{
}

VideoStreamInputStateProvider::~VideoStreamInputStateProvider() { }

void VideoStreamInputStateProvider::OnHasInputChanged(bool has_input)
{
    Mutex::Locker locker(&mutex_);
    input_state_.set_has_input(has_input);
}

void VideoStreamInputStateProvider::OnFrameSizeObserved(int frame_size_pixels)
{
    OCTK_DCHECK_GT(frame_size_pixels, 0);
    Mutex::Locker locker(&mutex_);
    input_state_.set_frame_size_pixels(frame_size_pixels);
}

void VideoStreamInputStateProvider::OnEncoderSettingsChanged(EncoderSettings encoder_settings)
{
    Mutex::Locker locker(&mutex_);
    input_state_.set_video_codec_type(encoder_settings.encoder_config().codec_type);
    input_state_.set_min_pixels_per_frame(encoder_settings.encoder_info().scaling_settings.min_pixels_per_frame);
    input_state_.set_single_active_stream_pixels(
        VideoStreamAdapter::GetSingleActiveLayerPixels(encoder_settings.video_codec()));
}

VideoStreamInputState VideoStreamInputStateProvider::InputState()
{
    // GetInputFrameRate() is thread-safe.
    int input_fps = frame_rate_provider_->GetInputFrameRate();
    Mutex::Locker locker(&mutex_);
    input_state_.set_frames_per_second(input_fps);
    return input_state_;
}


OCTK_END_NAMESPACE