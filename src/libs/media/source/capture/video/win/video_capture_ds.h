/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef _OCTK_VIDEO_CAPTURE_MAIN_SOURCE_WINDOWS_VIDEO_CAPTURE_DS_H_
#define _OCTK_VIDEO_CAPTURE_MAIN_SOURCE_WINDOWS_VIDEO_CAPTURE_DS_H_

#include "api/scoped_refptr.h"
#include "modules/video_capture/video_capture_impl.h"
#include "modules/video_capture/windows/device_info_ds.h"

#define CAPTURE_FILTER_NAME L"VideoCaptureFilter"
#define SINK_FILTER_NAME L"SinkFilter"

namespace webrtc {
namespace videocapturemodule {
// Forward declaraion
class CaptureSinkFilter;

class VideoCaptureDS : public VideoCaptureImpl {
 public:
  VideoCaptureDS();

  virtual int32_t init(const char* deviceUniqueIdUTF8);

  /*************************************************************************
   *
   *   start/stop
   *
   *************************************************************************/
  int32_t StartCapture(const VideoCaptureCapability& capability) override;
  int32_t StopCapture() override;

  /**************************************************************************
   *
   *   Properties of the set device
   *
   **************************************************************************/

  bool CaptureStarted() override;
  int32_t CaptureSettings(VideoCaptureCapability& settings) override;

 protected:
  ~VideoCaptureDS() override;

  // Help functions

  int32_t SetCameraOutput(const VideoCaptureCapability& requestedCapability);
  int32_t DisconnectGraph();
  HRESULT ConnectDVCamera();

  DeviceInfoDS _dsInfo OCTK_ATTRIBUTE_GUARDED_BY(api_checker_);

  IBaseFilter* _captureFilter OCTK_ATTRIBUTE_GUARDED_BY(api_checker_);
  IGraphBuilder* _graphBuilder OCTK_ATTRIBUTE_GUARDED_BY(api_checker_);
  IMediaControl* _mediaControl OCTK_ATTRIBUTE_GUARDED_BY(api_checker_);
  std::shared_ptr<CaptureSinkFilter> sink_filter_
      OCTK_ATTRIBUTE_GUARDED_BY(api_checker_);
  IPin* _inputSendPin OCTK_ATTRIBUTE_GUARDED_BY(api_checker_);
  IPin* _outputCapturePin OCTK_ATTRIBUTE_GUARDED_BY(api_checker_);

  // Microsoft DV interface (external DV cameras)
  IBaseFilter* _dvFilter OCTK_ATTRIBUTE_GUARDED_BY(api_checker_);
  IPin* _inputDvPin OCTK_ATTRIBUTE_GUARDED_BY(api_checker_);
  IPin* _outputDvPin OCTK_ATTRIBUTE_GUARDED_BY(api_checker_);
};
}  // namespace videocapturemodule
}  // namespace webrtc
#endif  // _OCTK_VIDEO_CAPTURE_MAIN_SOURCE_WINDOWS_VIDEO_CAPTURE_DS_H_
