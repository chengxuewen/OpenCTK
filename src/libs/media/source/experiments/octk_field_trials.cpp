/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2022 The WebRTC project authors. All Rights Reserved.
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

#include <private/octk_field_trials_p.hpp>
#include <octk_string_encode.hpp>
#include <octk_checks.hpp>

#include <atomic>

OCTK_BEGIN_NAMESPACE

namespace
{
// This part is copied from system_wrappers/field_trial.cc.
FieldTrials::Map<std::string, std::string> InsertIntoMap(const std::string &s)
{
    std::string::size_type field_start = 0;
    FieldTrials::Map<std::string, std::string> key_value_map;
    while (field_start < s.size())
    {
        std::string::size_type separator_pos = s.find('/', field_start);
        OCTK_CHECK_NE(separator_pos, std::string::npos) << "Missing separator '/' after field trial key.";
        OCTK_CHECK_GT(separator_pos, field_start) << "Field trial key cannot be empty.";
        std::string key = s.substr(field_start, separator_pos - field_start);
        field_start = separator_pos + 1;

        OCTK_CHECK_LT(field_start, s.size()) << "Missing value after field trial key. String ended.";
        separator_pos = s.find('/', field_start);
        OCTK_CHECK_NE(separator_pos, std::string::npos) << "Missing terminating '/' in field trial string.";
        OCTK_CHECK_GT(separator_pos, field_start) << "Field trial value cannot be empty.";
        std::string value = s.substr(field_start, separator_pos - field_start);
        field_start = separator_pos + 1;

        // If a key is specified multiple times, only the value linked to the first
        // key is stored. note: This will crash in debug build when calling
        // InitFieldTrialsFromString().
        key_value_map.emplace(key, value);
    }
    // This check is technically redundant due to earlier checks.
    // We nevertheless keep the check to make it clear that the entire
    // string has been processed, and without indexing past the end.
    OCTK_CHECK_EQ(field_start, s.size());

    return key_value_map;
}
} // namespace

FieldTrials::FieldTrials(const std::string &s)
    : field_trial_string_(s)
    , key_value_map_(InsertIntoMap(s))
{
}

std::unique_ptr<FieldTrials> FieldTrials::CreateNoGlobal(const std::string &s)
{
    return std::unique_ptr<FieldTrials>(new FieldTrials(s, true));
}

FieldTrials::FieldTrials(const std::string &s, bool)
    : key_value_map_(InsertIntoMap(s))
{
}

FieldTrials::~FieldTrials()
{
}

std::string FieldTrials::GetValue(StringView key) const
{
    auto it = key_value_map_.find(std::string(key));
    if (it != key_value_map_.end())
    {
        return it->second;
    }

    return "";
}

namespace test
{
namespace
{
// This part is copied from system_wrappers/field_trial.cc.
void InsertIntoMap(std::map<std::string, std::string, std::less<>> &key_value_map, StringView s)
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

        key_value_map[key] = value;
    }
    // This check is technically redundant due to earlier checks.
    // We nevertheless keep the check to make it clear that the entire
    // string has been processed, and without indexing past the end.
    OCTK_CHECK_EQ(field_start, s.size());
}
} // namespace

ScopedKeyValueConfig::ScopedKeyValueConfig()
    : ScopedKeyValueConfig(nullptr, "")
{
}

ScopedKeyValueConfig::ScopedKeyValueConfig(StringView s)
    : ScopedKeyValueConfig(nullptr, s)
{
}

ScopedKeyValueConfig::ScopedKeyValueConfig(ScopedKeyValueConfig &parent, StringView s)
    : ScopedKeyValueConfig(&parent, s)
{
}

ScopedKeyValueConfig::ScopedKeyValueConfig(ScopedKeyValueConfig *parent, StringView s)
    : parent_(parent)
    , leaf_(nullptr)
{
    InsertIntoMap(key_value_map_, s);

    if (parent == nullptr)
    {
        // We are root, set leaf_.
        leaf_ = this;
    }
    else
    {
        // Link root to new leaf.
        GetRoot(parent)->leaf_ = this;
        OCTK_DCHECK(leaf_ == nullptr);
    }
}

ScopedKeyValueConfig::~ScopedKeyValueConfig()
{
    if (parent_)
    {
        GetRoot(parent_)->leaf_ = parent_;
    }
}

ScopedKeyValueConfig *ScopedKeyValueConfig::GetRoot(ScopedKeyValueConfig *n)
{
    while (n->parent_ != nullptr)
    {
        n = n->parent_;
    }
    return n;
}

std::string ScopedKeyValueConfig::GetValue(StringView key) const
{
    if (parent_ == nullptr)
    {
        return leaf_->LookupRecurse(key);
    }
    else
    {
        return LookupRecurse(key);
    }
}

std::string ScopedKeyValueConfig::LookupRecurse(StringView key) const
{
    auto it = key_value_map_.find(key);
    if (it != key_value_map_.end())
        return it->second;

    if (parent_)
    {
        return parent_->LookupRecurse(key);
    }

    return "";
}
} // namespace test

OCTK_END_NAMESPACE
