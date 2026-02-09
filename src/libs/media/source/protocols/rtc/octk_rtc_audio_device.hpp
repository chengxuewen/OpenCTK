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
#include <octk_unique_function.hpp>

OCTK_BEGIN_NAMESPACE

/**
 * The RtcAudioDevice class is an abstract class used for managing the audio
 * devices used by WebRTC. It provides methods for device enumeration and
 * selection.
 */
class RtcAudioDevice
{
public:
    using SharedPtr = SharedPointer<RtcAudioDevice>;

    using OnDeviceChangeCallback = UniqueFunction<void()>;

    OCTK_STATIC_CONSTANT_NUMBER(kAdmMaxDeviceNameSize, 128)
    OCTK_STATIC_CONSTANT_NUMBER(kAdmMaxFileNameSize, 512)
    OCTK_STATIC_CONSTANT_NUMBER(kAdmMaxGuidSize, 128)

    /**
     * Returns the number of playout devices available.
     *
     * @return int16_t - The number of playout devices available.
     */
    virtual int16_t playoutDevices() = 0;

    /**
     * Returns the number of recording devices available.
     *
     * @return int16_t - The number of recording devices available.
     */
    virtual int16_t recordingDevices() = 0;

    /**
     * Retrieves the name and GUID of the specified playout device.
     *
     * @param index - The index of the device.
     * @param name - The device name.
     * @param guid - The device GUID.
     * @return int32_t - 0 if successful, otherwise an error code.
     */
    virtual int32_t playoutDeviceName(uint16_t index, char name[kAdmMaxDeviceNameSize], char guid[kAdmMaxGuidSize]) = 0;

    /**
     * Retrieves the name and GUID of the specified recording device.
     *
     * @param index - The index of the device.
     * @param name - The device name.
     * @param guid - The device GUID.
     * @return int32_t - 0 if successful, otherwise an error code.
     */
    virtual int32_t recordingDeviceName(uint16_t index,
                                        char name[kAdmMaxDeviceNameSize],
                                        char guid[kAdmMaxGuidSize]) = 0;

    /**
     * Sets the playout device to use.
     *
     * @param index - The index of the device.
     * @return int32_t - 0 if successful, otherwise an error code.
     */
    virtual int32_t setPlayoutDevice(uint16_t index) = 0;

    /**
     * Sets the recording device to use.
     *
     * @param index - The index of the device.
     * @return int32_t - 0 if successful, otherwise an error code.
     */
    virtual int32_t setRecordingDevice(uint16_t index) = 0;

    /**
     * Registers a listener to be called when audio devices are added or removed.
     *
     * @param listener - The callback function to register.
     * @return int32_t - 0 if successful, otherwise an error code.
     */
    virtual int32_t onDeviceChange(OnDeviceChangeCallback listener) = 0;

    virtual int32_t setMicrophoneVolume(uint32_t volume) = 0;

    virtual int32_t microphoneVolume(uint32_t &volume) = 0;

    virtual int32_t setSpeakerVolume(uint32_t volume) = 0;

    virtual int32_t speakerVolume(uint32_t &volume) = 0;

protected:
    virtual ~RtcAudioDevice() = default;
};

OCTK_END_NAMESPACE
