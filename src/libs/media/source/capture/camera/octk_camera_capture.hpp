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

#include <octk_video_sink_interface.hpp>
#include <octk_shared_pointer.hpp>
#include <octk_video_frame.hpp>
#include <octk_video_type.hpp>

OCTK_BEGIN_NAMESPACE

class CameraCapturePrivate;
class OCTK_CORE_API CameraCapture
{
public:
    OCTK_STATIC_CONSTANT_NUMBER(kUniqueNameLength, 1024)

    OCTK_STATIC_CONSTANT_NUMBER(kDefaultWidth, 640) // Start width
    OCTK_STATIC_CONSTANT_NUMBER(kDefaultHeight, 480)  // Start heigt

    OCTK_STATIC_CONSTANT_NUMBER(kDefaultFrameRate, 30) // Start frame rate
    OCTK_STATIC_CONSTANT_NUMBER(kMaxFrameRate, 60) // Max allowed frame rate of the start image

    OCTK_STATIC_CONSTANT_NUMBER(kDefaultCaptureDelay, 120)
    OCTK_STATIC_CONSTANT_NUMBER(kMaxCaptureDelay, 270) // Max capture delay allowed in the precompiled capture delay values.

    OCTK_STATIC_CONSTANT_NUMBER(kFrameRateCallbackInterval, 1000)
    OCTK_STATIC_CONSTANT_NUMBER(kFrameRateCountHistorySize, 90)
    OCTK_STATIC_CONSTANT_NUMBER(kFrameRateHistoryWindowMs, 2000)

    using SharedPtr = std::shared_ptr<CameraCapture>;

    //    class Options
    //    {

    //    };

    struct Capability final
    {
        int32_t width{0};
        int32_t height{0};
        int32_t maxFPS{0};
        bool interlaced{false};
        VideoType videoType{VideoType::kANY};

        Capability() = default;
        ~Capability() = default;
        bool operator!=(const Capability& other) const
        {
            return width == other.width && height == other.height && maxFPS == other.maxFPS &&
                    interlaced == other.interlaced && videoType != other.videoType;
        }
        bool operator==(const Capability& other) const { return !operator!=(other); }
    };
    using Capabilities = std::vector<Capability>;

    class DeviceInfoPrivate;
    class DeviceInfo
    {
    public:
        OCTK_DEFINE_SHARED_PTR(DeviceInfo)

        explicit DeviceInfo(DeviceInfoPrivate *d);
        virtual ~DeviceInfo();

        virtual uint32_t numberOfDevices() = 0;

        /**
         * @brief Returns the number of capabilities this device.
         * @param deviceUniqueIdUTF8
         * @return
         */
        virtual int32_t numberOfCapabilities(const char* deviceUniqueIdUTF8);

        /**
         * @brief getDeviceName
         * @param deviceNumber      - Index of capture device.
         * @param deviceNameUTF8    - Friendly name of the capture device.
         * @param deviceNameLength
         * @param deviceUniqueIdUTF8        - Unique name of the capture device if it exist.
         *                                    Otherwise same as deviceNameUTF8.
         * @param deviceUniqueIdUTF8Length
         * @param productUniqueIdUTF8       - Unique product id if it exist.
         *                                    Null terminated otherwise.
         * @param productUniqueIdUTF8Length
         * @return
         */
        virtual int32_t getDeviceName(uint32_t deviceNumber,
                                      char* deviceNameUTF8,
                                      uint32_t deviceNameLength,
                                      char* deviceUniqueIdUTF8,
                                      uint32_t deviceUniqueIdUTF8Length,
                                      char* productUniqueIdUTF8 = 0,
                                      uint32_t productUniqueIdUTF8Length = 0) = 0;

        /**
         * @brief Gets the capabilities of the named device.
         * @param deviceUniqueIdUTF8
         * @param deviceCapabilityNumber
         * @param capability
         * @return
         */
        virtual int32_t getCapability(const char* deviceUniqueIdUTF8,
                                      uint32_t deviceCapabilityNumber,
                                      Capability& capability);

        /**
         * @brief Gets clockwise angle the captured frames should be rotated in order to be displayed correctly on
         * a normally rotated display.
         * @param deviceUniqueIdUTF8
         * @param orientation
         * @return
         */
        virtual int32_t getOrientation(const char* deviceUniqueIdUTF8,
                                       VideoRotation& orientation);

        /**
         * @brief Gets the capability that best matches the requested width, height and frame rate.
         * Returns the deviceCapabilityNumber on success.
         * @param deviceUniqueIdUTF8
         * @param requested
         * @param resulting
         * @return
         */
        virtual int32_t getBestMatchedCapability(const char* deviceUniqueIdUTF8,
                                                 const Capability& requested,
                                                 Capability& resulting);

    protected:
        virtual int32_t init() = 0;
        virtual int32_t createCapabilityMap(const char* deviceUniqueIdUTF8) = 0;

    protected:
        OCTK_DEFINE_DPTR(DeviceInfo)
        OCTK_DECLARE_PRIVATE(DeviceInfo)
        OCTK_DISABLE_COPY_MOVE(DeviceInfo)
    };

    static DeviceInfo::SharedPtr createDeviceInfo();
    static SharedPtr create(const char* deviceUniqueIdUTF8);

    explicit CameraCapture(CameraCapturePrivate *d);
    virtual ~CameraCapture();

    /**
     * @brief Register capture data callback
     * @param dataCallback
     */
    virtual void registerCaptureDataCallback(VideoSinkInterface<VideoFrame> *dataCallback);
    //    virtual void RegisterCaptureDataCallback(RawVideoSinkInterface* dataCallback);

    /**
     * @brief Remove capture data callback
     */
    virtual void deregisterCaptureDataCallback();

    /**
     * @brief Start capture device
     * @param capability
     * @return
     */
    virtual int32_t startCapture(const Capability& capability);

    virtual int32_t stopCapture();

    /**
     * @brief Returns true if the capture device is running
     * @return
     */
    virtual bool isCaptureStarted();

    /**
     * @brief Returns the name of the device used by this module.
     * @return
     */
    virtual const char* currentDeviceName() const;

    /**
     * @brief Gets the current configuration.
     * @param settings
     * @return
     */
    virtual int32_t captureSettings(Capability& settings);

    /**
     * @brief Set the rotation of the captured frames.
     *  If the rotation is set to the same as returned by DeviceInfo::getOrientation the captured frames are displayed
     *  correctly if rendered.
     * @param rotation
     * @return
     */
    virtual int32_t setCaptureRotation(VideoRotation rotation);

    /**
     * @brief Return whether the rotation is applied or left pending.
     * @return
     */
    virtual bool getApplyRotation();

    /**
     * @brief Tells the capture module whether to apply the pending rotation.
     * By default, the rotation is applied and the generated frame is up right.
     * When set to false, generated frames will carry the rotation information from SetCaptureRotation.
     * @param enable
     * @return Return value indicates whether this operation succeeds.
     */
    virtual bool setApplyRotation(bool enable);

protected:
    virtual bool init(const char* deviceUniqueIdUTF8) = 0;

protected:
    OCTK_DEFINE_DPTR(CameraCapture)
    OCTK_DECLARE_PRIVATE(CameraCapture)
    OCTK_DISABLE_COPY_MOVE(CameraCapture)
};

OCTK_END_NAMESPACE
