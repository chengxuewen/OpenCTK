/* Copyright 2019 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef _OCTK_FEC_CONTROLLER_OVERRIDE_HPP
#define _OCTK_FEC_CONTROLLER_OVERRIDE_HPP

#include <octk_media_global.hpp>

OCTK_BEGIN_NAMESPACE

// Interface for temporarily overriding FecController's bitrate allocation.
class FecControllerOverride
{
public:
    virtual void SetFecAllowed(bool fec_allowed) = 0;

protected:
    virtual ~FecControllerOverride() = default;
};

OCTK_END_NAMESPACE

#endif // _OCTK_FEC_CONTROLLER_OVERRIDE_HPP
