/*
*  Copyright 2019 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef _OCTK_FIELD_TRIALS_VIEW_HPP
#define _OCTK_FIELD_TRIALS_VIEW_HPP

#include <octk_string_view.hpp>
#include <octk_string_utils.hpp>

#include <set>

OCTK_BEGIN_NAMESPACE

// An interface that provides the means to access field trials.
//
// Note that there are no guarantess that the meaning of a particular key-value
// mapping will be preserved over time and no announcements will be made if they
// are changed. It's up to the library user to ensure that the behavior does not
// break.
class OCTK_CORE_API FieldTrialsView
{
public:
    template <typename T> using Set = std::set<T>;

    virtual ~FieldTrialsView() = default;

    // Returns the configured value for `key` or an empty string if the field
    // trial isn't configured.
    virtual std::string Lookup(StringView key) const = 0;

    bool IsEnabled(StringView key) const { return utils::stringStartsWith(Lookup(key), "Enabled"); }

    bool IsDisabled(StringView key) const { return utils::stringStartsWith(Lookup(key), "Disabled"); }
};

OCTK_END_NAMESPACE

#endif // _OCTK_FIELD_TRIALS_VIEW_HPP
