/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#pragma once

#include <octk_string_view.hpp>
#include <private/octk_field_trials_registry_p.hpp>

#include <functional>
#include <map>
#include <string>

OCTK_BEGIN_NAMESPACE

class OCTK_MEDIA_API ExplicitKeyValueConfig : public FieldTrialsRegistry
{
public:
    explicit ExplicitKeyValueConfig(StringView s);

private:
    std::string GetValue(StringView key) const override;

    // Unlike std::less<std::string>, std::less<> is transparent and allows
    // heterogeneous lookup directly with StringView.
    std::map<std::string, std::string, std::less<>> key_value_map_;
};

OCTK_END_NAMESPACE