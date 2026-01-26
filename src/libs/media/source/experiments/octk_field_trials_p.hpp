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

#pragma once

#include <private/octk_field_trials_registry_p.hpp>
#include <octk_checks.hpp>

#include <map>

OCTK_BEGIN_NAMESPACE

// The FieldTrials class is used to inject field trials into webrtc.
//
// Field trials allow webrtc clients (such as Chromium) to turn on feature code
// in binaries out in the field and gather information with that.
//
// They are designed to be easy to use with Chromium field trials and to speed
// up developers by reducing the need to wire up APIs to control whether a
// feature is on/off.
//
// The field trials are injected into objects that use them at creation time.
//
// NOTE: Creating multiple FieldTrials-object is currently prohibited
// until we remove the global string (TODO(bugs.webrtc.org/10335))
// (unless using CreateNoGlobal):
class FieldTrials : public FieldTrialsRegistry
{
public:
    template <typename K, typename U>
    using Map = std::map<K, U>;
    template <typename T>
    using Set = std::set<T>;

    explicit FieldTrials(const std::string &s);
    ~FieldTrials();

    // Create a FieldTrials object that is not reading/writing from
    // global variable (i.e can not be used for all parts of webrtc).
    static std::unique_ptr<FieldTrials> CreateNoGlobal(const std::string &s);

private:
    explicit FieldTrials(const std::string &s, bool);

    std::string GetValue(StringView key) const override;

    const std::string field_trial_string_;
    const Map<std::string, std::string> key_value_map_;
};

namespace test
{

class ScopedKeyValueConfig : public FieldTrialsRegistry
{
public:
    virtual ~ScopedKeyValueConfig();
    ScopedKeyValueConfig();
    explicit ScopedKeyValueConfig(StringView s);
    ScopedKeyValueConfig(ScopedKeyValueConfig &parent, StringView s);

private:
    ScopedKeyValueConfig(ScopedKeyValueConfig *parent, StringView s);
    ScopedKeyValueConfig *GetRoot(ScopedKeyValueConfig *n);
    std::string GetValue(StringView key) const override;
    std::string LookupRecurse(StringView key) const;

    ScopedKeyValueConfig *const parent_;

    // The leaf in a list of stacked ScopedKeyValueConfig.
    // Only set on root (e.g with parent_ == nullptr).
    const ScopedKeyValueConfig *leaf_;

    // Unlike std::less<std::string>, std::less<> is transparent and allows
    // heterogeneous lookup directly with StringView.
    std::map<std::string, std::string, std::less<>> key_value_map_;
};
} // namespace test

OCTK_END_NAMESPACE