/*
*  Copyright (c) 2022 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef _OCTK_FIELD_TRIALS_HPP
#define _OCTK_FIELD_TRIALS_HPP

#include <octk_field_trials_registry.hpp>
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
    template <typename K, typename U> using Map = std::map<K, U>;
    template <typename T> using Set = std::set<T>;

    explicit FieldTrials(const std::string &s);
    ~FieldTrials();

    // Create a FieldTrials object that is not reading/writing from
    // global variable (i.e can not be used for all parts of webrtc).
    static std::unique_ptr<FieldTrials> CreateNoGlobal(const std::string &s);

private:
    explicit FieldTrials(const std::string &s, bool);

    std::string GetValue(StringView key) const override;

    const bool uses_global_;
    const std::string field_trial_string_;
    const char *const previous_field_trial_string_;
    const Map<std::string, std::string> key_value_map_;
};

namespace field_trial
{
// Returns the group name chosen for the named trial, or the empty string
// if the trial does not exists.
//
// Note: To keep things tidy append all the trial names with WebRTC.
std::string FindFullName(StringView name);

// Convenience method, returns true iff FindFullName(name) return a string that
// starts with "Enabled".
// TODO(tommi): Make sure all implementations support this.
inline bool IsEnabled(StringView name) { return FindFullName(name).find("Enabled") == 0; }

// Convenience method, returns true iff FindFullName(name) return a string that
// starts with "Disabled".
inline bool IsDisabled(StringView name) { return FindFullName(name).find("Disabled") == 0; }

// Optionally initialize field trial from a string.
// This method can be called at most once before any other call into webrtc.
// E.g. before the peer connection factory is constructed.
// Note: trials_string must never be destroyed.
void InitFieldTrialsFromString(const char *trials_string);

const char *GetFieldTrialString();

// Validates the given field trial string.
bool FieldTrialsStringIsValid(StringView trials_string);

// Merges two field trial strings.
//
// If a key (trial) exists twice with conflicting values (groups), the value
// in 'second' takes precedence.
// Shall only be called with valid FieldTrial strings.
std::string MergeFieldTrialsStrings(StringView first, StringView second);

// This helper allows to temporary "register" a field trial within the current
// scope. This is only useful for tests that use the global field trial string,
// otherwise you can use `webrtc::FieldTrialsRegistry`.
//
// If you want to isolate changes to the global field trial string itself within
// the current scope you should use `webrtc::test::ScopedFieldTrials`.
class FieldTrialsAllowedInScopeForTesting
{
public:
    explicit FieldTrialsAllowedInScopeForTesting(FieldTrials::Set<std::string> keys);
    ~FieldTrialsAllowedInScopeForTesting();
};

// This class is used to override field-trial configs within specific tests.
// After this class goes out of scope previous field trials will be restored.
class ScopedFieldTrials
{
public:
    explicit ScopedFieldTrials(StringView config);
    ScopedFieldTrials(const ScopedFieldTrials &) = delete;
    ScopedFieldTrials &operator=(const ScopedFieldTrials &) = delete;
    ~ScopedFieldTrials();

private:
    std::string current_field_trials_;
    const char *previous_field_trials_;
};
} // namespace field_trial

// Implementation using the field trial API fo the key value lookup.
class OCTK_MEDIA_API FieldTrialBasedConfig : public FieldTrialsRegistry
{
private:
    std::string GetValue(StringView key) const override { return field_trial::FindFullName(std::string(key)); }
};

OCTK_END_NAMESPACE

#endif // _OCTK_FIELD_TRIALS_HPP
