/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <test/octk_explicit_key_value_config_p.hpp>
#include <octk_checks.hpp>

#include <string>

OCTK_BEGIN_NAMESPACE

ExplicitKeyValueConfig::ExplicitKeyValueConfig(StringView s)
{
    std::string::size_type field_start = 0;
    while (field_start < s.size())
    {
        std::string::size_type separator_pos = s.find('/', field_start);
        OCTK_CHECK_NE(separator_pos, std::string::npos) << "Missing separator '/' after field trial key.";
        OCTK_CHECK_GT(separator_pos, field_start) << "Field trial key cannot be empty.";
        std::string key(s.substr(field_start, separator_pos - field_start));
        field_start = separator_pos + 1;

        OCTK_CHECK_LT(field_start, s.size()) << "Missing value after field trial key. String ended.";
        separator_pos = s.find('/', field_start);
        OCTK_CHECK_NE(separator_pos, std::string::npos) << "Missing terminating '/' in field trial string.";
        OCTK_CHECK_GT(separator_pos, field_start) << "Field trial value cannot be empty.";
        std::string value(s.substr(field_start, separator_pos - field_start));
        field_start = separator_pos + 1;

        key_value_map_[key] = value;
    }
    // This check is technically redundant due to earlier checks.
    // We nevertheless keep the check to make it clear that the entire
    // string has been processed, and without indexing past the end.
    OCTK_CHECK_EQ(field_start, s.size());
}

std::string ExplicitKeyValueConfig::GetValue(StringView key) const
{
    auto it = key_value_map_.find(key);
    if (it != key_value_map_.end())
        return it->second;
    return "";
}

OCTK_END_NAMESPACE
