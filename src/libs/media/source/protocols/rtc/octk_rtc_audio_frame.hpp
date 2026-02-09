/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#include <octk_rtc_types.hpp>

OCTK_BEGIN_NAMESPACE

class RtcAudioFrame
{
public:
    /**
   * @brief Creates a new instance of RtcAudioFrame.
   * @return RtcAudioFrame*: a pointer to the newly created RtcAudioFrame.
   */
    static RtcAudioFrame *Create();

    /**
   * @brief Creates a new instance of RtcAudioFrame with specified parameters.
   * @param id: the unique identifier of the frame.
   * @param timestamp: the timestamp of the frame.
   * @param data: a pointer to the audio data buffer.
   * @param samples_per_channel: the number of samples per channel.
   * @param sample_rate_hz: the sample rate in Hz.
   * @param num_channels: the number of audio channels.
   * @return RtcAudioFrame*: a pointer to the newly created RtcAudioFrame.
   */
    static SharedPointer<RtcAudioFrame> create(int id,
                                               uint32_t timestamp,
                                               const int16_t *data,
                                               size_t samples_per_channel,
                                               int sample_rate_hz,
                                               size_t num_channels = 1);

public:
    /**
   * @brief Updates the audio frame with specified parameters.
   * @param id: the unique identifier of the frame.
   * @param timestamp: the timestamp of the frame.
   * @param data: a pointer to the audio data buffer.
   * @param samples_per_channel: the number of samples per channel.
   * @param sample_rate_hz: the sample rate in Hz.
   * @param num_channels: the number of audio channels.
   */
    virtual void UpdateFrame(int id,
                             uint32_t timestamp,
                             const int16_t *data,
                             size_t samples_per_channel,
                             int sample_rate_hz,
                             size_t num_channels = 1) = 0;

    /**
   * @brief Copies the contents of another RtcAudioFrame.
   * @param src: the source RtcAudioFrame to copy from.
   */
    virtual void CopyFrom(const RtcAudioFrame &src) = 0;

    /**
   * @brief Adds another RtcAudioFrame to this one.
   * @param frame_to_add: the RtcAudioFrame to add.
   */
    virtual void Add(const RtcAudioFrame &frame_to_add) = 0;

    /**
   * @brief Mutes the audio data in this RtcAudioFrame.
   */
    virtual void Mute() = 0;

    /**
   * @brief Returns a pointer to the audio data buffer.
   * @return const int16_t*: a pointer to the audio data buffer.
   */
    virtual const int16_t *data() = 0;

    /**
   * @brief Returns the number of samples per channel.
   * @return size_t: the number of samples per channel.
   */
    virtual size_t samples_per_channel() = 0;

    /**
   * @brief Returns the sample rate in Hz.
   * @return int: the sample rate in Hz.
   */
    virtual int sample_rate_hz() = 0;

    /**
   * @brief Returns the number of audio channels.
   * @return size_t: the number of audio channels.
   */
    virtual size_t num_channels() = 0;

    /**
   * @brief Returns the timestamp of the RtcAudioFrame.
   * @return uint32_t: the timestamp of the RtcAudioFrame.
   */
    virtual uint32_t timestamp() = 0;

    /**
   * @brief Returns the unique identifier of the RtcAudioFrame.
   * @return int: the unique identifier of the RtcAudioFrame.
   */

    virtual int id() = 0;
};
using RtcAudioSink = Sink<SharedPointer<RtcAudioFrame>>;
using RtcAudioSource = Source<SharedPointer<RtcAudioFrame>>;
using RtcAudioSourceProvider = SourceProvider<SharedPointer<RtcAudioFrame>>;

/*class RtcAudioSink
{
public:
    virtual void onData(const void *audioData, int bitsPerSample, int sampleRate, size_t nChannels, size_t nFrames) = 0;

protected:
    virtual ~RtcAudioSink() = default;
};*/

OCTK_END_NAMESPACE
