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

#include <octk_shared_pointer.hpp>
#include <octk_vector_map.hpp>
#include <octk_string.hpp>

OCTK_BEGIN_NAMESPACE

enum class RtcMediaSecurityType
{
    kSRTP_None = 0,
    kSDES_SRTP,
    kDTLS_SRTP
};

enum class RtcMediaType
{
    AUDIO,
    VIDEO,
    DATA,
    UNSUPPORTED
};

enum class RtcVideoFrameType
{
    kKey,
    kDelta,
    kEmpty,
};

enum class RtcH264PacketizationMode
{
    kNonInterleaved = 0, // Mode 1 - STAP-A, FU-A is allowed
    kSingleNalUnit       // Mode 0 - only single NALU allowed
};

enum class RtcH264Profile
{
    kProfileConstrainedBaseline,
    kProfileBaseline,
    kProfileMain,
    kProfileConstrainedHigh,
    kProfileHigh,
    kProfilePredictiveHigh444,
};

// All values are equal to ten times the level number, except level 1b which is special.
enum class RtcH264Level
{
    kLevel1_b = 0,
    kLevel1 = 10,
    kLevel1_1 = 11,
    kLevel1_2 = 12,
    kLevel1_3 = 13,
    kLevel2 = 20,
    kLevel2_1 = 21,
    kLevel2_2 = 22,
    kLevel3 = 30,
    kLevel3_1 = 31,
    kLevel3_2 = 32,
    kLevel4 = 40,
    kLevel4_1 = 41,
    kLevel4_2 = 42,
    kLevel5 = 50,
    kLevel5_1 = 51,
    kLevel5_2 = 52
};

using RtcCodecParameterMap = VectorMap<String, String>;

class RtcSdpVideoFormat
{
public:
    using SharedPtr = SharedPointer<RtcSdpVideoFormat>;

    OCTK_STATIC_CONSTANT_STRING(kVp8CodecName, "VP8")
    OCTK_STATIC_CONSTANT_STRING(kVp9CodecName, "VP9")
    OCTK_STATIC_CONSTANT_STRING(kAv1CodecName, "AV1")
    OCTK_STATIC_CONSTANT_STRING(kH264CodecName, "H264")
    OCTK_STATIC_CONSTANT_STRING(kH265CodecName, "H265")

    virtual StringView name() const = 0;
    virtual void setName(StringView name) = 0;

    virtual RtcCodecParameterMap parameters() const = 0;
    virtual void setParameters(const RtcCodecParameterMap &parameters) = 0;

    virtual Vector<uint8_t> scalabilityModes() const = 0;
    virtual void setScalabilityModes(const Vector<uint8_t> &scalabilityModes) = 0;

    virtual String toString() const = 0;
    virtual bool isSameCodec(const SharedPtr &other) const;

protected:
    virtual ~RtcSdpVideoFormat() = default;
};

struct RtcVideoCodec
{
    enum class Type
    {
        kGeneric = 0,
        kH264,
        kH265,
        kVP8,
        kVP9,
        kAV1,
    };
    enum class Mode
    {
        kRealtimeVideo,
        kScreenSharing
    };

    Type type{Type::kGeneric};
    Mode mode{Mode::kRealtimeVideo};

    uint16_t width{0};
    uint16_t height{0};

    uint32_t maxFramerate{0};

    unsigned int maxBitrate{0};   // kilobits/sec.
    unsigned int minBitrate{0};   // kilobits/sec.
    unsigned int startBitrate{0}; // kilobits/sec.

    bool frameDropEnabled{false};

    struct H264
    {
        int keyFrameInterval{0};
        uint8_t numberOfTemporalLayers{0};
    } h264;

    struct VP8
    {
        bool denoisingOn{false};
        bool automaticResizeOn{false};

        int keyFrameInterval{0};
        unsigned char numberOfTemporalLayers{0};
    } vp8;

    struct VP9
    {
        enum class InterLayerPredMode : int
        {
            kOn = 0,      // Inter-layer prediction is enabled.
            kOff = 1,     // Inter-layer prediction is disabled.
            kOnKeyPic = 2 // Inter-layer prediction is enabled but limited to key frames.
        };
        bool denoisingOn{false};
        bool flexibleMode{false};
        bool adaptiveQpMode{false};
        bool automaticResizeOn{false};

        int keyFrameInterval{0};
        unsigned char numberOfSpatialLayers{0};
        unsigned char numberOfTemporalLayers{0};

        InterLayerPredMode interLayerPred{InterLayerPredMode::kOff};
    } vp9;

    struct AV1
    {
        bool automaticResizeOn{true};
    } av1;
};

class RtcVideoBitrateAllocation
{
public:
    using SharedPtr = SharedPointer<RtcVideoBitrateAllocation>;

    virtual uint32_t getBitrate(size_t spatial_index, size_t temporal_index) const = 0;

    // Get the sum of all the temporal layer for a specific spatial layer.
    virtual uint32_t getSpatialLayerSum(size_t spatial_index) const = 0;

    // Whether the specific spatial layers has the bitrate set in any of its temporal layers.
    virtual bool isSpatialLayerUsed(size_t spatial_index) const = 0;

    // Sum of bitrates of temporal layers, from layer 0 to `temporal_index`
    // inclusive, of specified spatial layer `spatial_index`. Bitrates of lower
    // spatial layers are not included.
    virtual uint32_t getTemporalLayerSum(size_t spatial_index, size_t temporal_index) const = 0;

    // Returns a vector of the temporal layer bitrates for the specific spatial
    // layer. Length of the returned vector is cropped to the highest temporal
    // layer with a defined bitrate.
    virtual Vector<uint32_t> getTemporalLayerAllocation(size_t spatial_index) const = 0;

    // Sum of all bitrates.
    virtual uint32_t getSumBps() const = 0;

protected:
    virtual ~RtcVideoBitrateAllocation() = default;
};

class RtcEncodedImage
{
public:
    using SharedPtr = SharedPointer<RtcEncodedImage>;

    virtual size_t size() const = 0;
    virtual const uint8_t *data() const = 0;

    const uint8_t *begin() const { return this->data(); }
    const uint8_t *end() const { return this->data() + this->size(); }

protected:
    virtual ~RtcEncodedImage() = default;
};

struct RtcCodecSpecificInfo
{
    RtcVideoCodec::Type codecType{RtcVideoCodec::Type::kGeneric};
    bool endOfPicture{true};

    struct Union
    {
        struct VP8
        {
            bool nonReference;
            uint8_t temporalIdx;
            bool layerSync;
            int8_t keyIdx; // Negative value to skip keyIdx.

            bool useExplicitDependencies;
            static constexpr size_t kBuffersCount{3};
            size_t referencedBuffers[kBuffersCount];
            size_t referencedBuffersCount;
            size_t updatedBuffers[kBuffersCount];
            size_t updatedBuffersCount;
        } vp8;
        struct H264
        {
            RtcH264PacketizationMode packetizationMode{RtcH264PacketizationMode::kNonInterleaved};
            uint8_t temporalIndex{0};
            bool baseLayerSync{false};
            bool idrFrame{false};
        } h264;
    } codecSpecific;
};

OCTK_END_NAMESPACE