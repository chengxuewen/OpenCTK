/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef _OCTK_VIDEO_CODEC_INTERFACE_HPP
#define _OCTK_VIDEO_CODEC_INTERFACE_HPP

#include <octk_frame_instrumentation_data.hpp>
#include <octk_generic_frame_info.hpp>
#include <octk_codecs_h264_types.hpp>
#include <octk_video_codes_error.hpp>
#include <octk_video_codec_types.hpp>
#include <octk_scalability_mode.hpp>
#include <octk_codecs_vp8_types.hpp>
#include <octk_codecs_vp9_types.hpp>
#include <octk_video_frame.hpp>
#include <octk_optional.hpp>
#include <octk_variant.hpp>

#include <vector>

//#include "absl/types/variant.h"
//#include "api/video/video_frame.h"
//#include "api/video_codecs/scalability_mode.h"
//#include "api/video_codecs/video_decoder.h"
//#include "api/video_codecs/video_encoder.h"
//#include "common_video/frame_instrumentation_data.h"
//#include "common_video/generic_frame_descriptor/generic_frame_info.h"
//#include "modules/video_coding/codecs/h264/include/h264_globals.h"
//#include "modules/video_coding/codecs/vp9/include/vp9_globals.h"
//#include "modules/video_coding/include/video_error_codes.h"
//#include "rtc_base/system/rtc_export.h"

OCTK_BEGIN_NAMESPACE

// Note: If any pointers are added to this struct, it must be fitted
// with a copy-constructor. See below.
// Hack alert - the code assumes that thisstruct is memset when constructed.
struct CodecSpecificInfoVP8
{
    bool nonReference;
    uint8_t temporalIdx;
    bool layerSync;
    int8_t keyIdx;  // Negative value to skip keyIdx.

    // Used to generate the list of dependency frames.
    // `referencedBuffers` and `updatedBuffers` contain buffer IDs.
    // Note that the buffer IDs here have a one-to-one mapping with the actual
    // codec buffers, but the exact mapping (i.e. whether 0 refers to Last,
    // to Golden or to Arf) is not pre-determined.
    // More references may be specified than are strictly necessary, but not less.
    // TODO(bugs.webrtc.org/10242): Remove `useExplicitDependencies` once all
    // encoder-wrappers are updated.
    bool useExplicitDependencies;
    static constexpr size_t kBuffersCount = 3;
    size_t referencedBuffers[kBuffersCount];
    size_t referencedBuffersCount;
    size_t updatedBuffers[kBuffersCount];
    size_t updatedBuffersCount;
};
static_assert(std::is_trivial<CodecSpecificInfoVP8>::value &&
              std::is_standard_layout<CodecSpecificInfoVP8>::value,
              "");

// Hack alert - the code assumes that thisstruct is memset when constructed.
struct CodecSpecificInfoVP9
{
    bool first_frame_in_picture;  // First frame, increment picture_id.
    bool inter_pic_predicted;     // This layer frame is dependent on previously
    // coded frame(s).
    bool flexible_mode;
    bool ss_data_available;
    bool non_ref_for_inter_layer_pred;

    uint8_t temporal_idx;
    bool temporal_up_switch;
    bool inter_layer_predicted;  // Frame is dependent on directly lower spatial
    // layer frame.
    uint8_t gof_idx;

    // SS data.
    size_t num_spatial_layers;  // Always populated.
    size_t first_active_layer;
    bool spatial_layer_resolution_present;
    uint16_t width[kMaxVp9NumberOfSpatialLayers];
    uint16_t height[kMaxVp9NumberOfSpatialLayers];
    GofInfoVP9 gof;

    // Frame reference data.
    uint8_t num_ref_pics;
    uint8_t p_diff[kMaxVp9RefPics];
};
static_assert(std::is_trivial<CodecSpecificInfoVP9>::value &&
              std::is_standard_layout<CodecSpecificInfoVP9>::value,
              "");

// Hack alert - the code assumes that thisstruct is memset when constructed.
struct CodecSpecificInfoH264
{
    H264PacketizationMode packetization_mode;
    uint8_t temporal_idx;
    bool base_layer_sync;
    bool idr_frame;
};
static_assert(std::is_trivial<CodecSpecificInfoH264>::value &&
              std::is_standard_layout<CodecSpecificInfoH264>::value,
              "");

union CodecSpecificInfoUnion
{
    CodecSpecificInfoVP8 VP8;
    CodecSpecificInfoVP9 VP9;
    CodecSpecificInfoH264 H264;
};
static_assert(std::is_trivial<CodecSpecificInfoUnion>::value &&
              std::is_standard_layout<CodecSpecificInfoUnion>::value,
              "");

// Note: if any pointers are added to this struct or its sub-structs, it
// must be fitted with a copy-constructor. This is because it is copied
// in the copy-constructor of VCMEncodedFrame.
struct OCTK_MEDIA_API CodecSpecificInfo
{
    CodecSpecificInfo();
    CodecSpecificInfo(const CodecSpecificInfo &);
    ~CodecSpecificInfo();

    VideoCodecType codecType;
    CodecSpecificInfoUnion codecSpecific;
    bool end_of_picture = true;
    Optional<GenericFrameInfo> generic_frame_info;
    Optional<FrameDependencyStructure> template_structure;
    Optional<ScalabilityMode> scalability_mode;

    // Required for automatic corruption detection.
    Optional<Variant<FrameInstrumentationSyncData, FrameInstrumentationData>> frame_instrumentation_data;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_VIDEO_CODEC_INTERFACE_HPP
