/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <objc/octk_objc_macros.hpp>

NS_ASSUME_NONNULL_BEGIN

@class OCTK_OBJC_TYPE(MacVideoCapturer);

OCTK_OBJC_EXPORT
@protocol OCTK_OBJC_TYPE
(MacVideoCapturerDelegate)<NSObject> -
    (void)capturer : (OCTK_OBJC_TYPE(MacVideoCapturer) *)capturer didCaptureVideoFrame
    : (OCTK_OBJC_TYPE(RTCVideoFrame) *)frame;
@end

OCTK_OBJC_EXPORT
@interface OCTK_OBJC_TYPE (MacVideoCapturer) : NSObject

@property(nonatomic, weak) id<OCTK_OBJC_TYPE(MacVideoCapturerDelegate)> delegate;

- (instancetype)initWithDelegate:(id<OCTK_OBJC_TYPE(MacVideoCapturerDelegate)>)delegate;

@end

NS_ASSUME_NONNULL_END
