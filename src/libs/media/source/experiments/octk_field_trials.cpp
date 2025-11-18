//
// Created by cxw on 25-8-15.
//

#include <octk_field_trials.hpp>
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

// Makes sure that only one instance is created, since the usage
// of global string makes behaviour unpredicatable otherwise.
// TODO(bugs.webrtc.org/10335): Remove once global string is gone.
std::atomic<bool> instance_created_{false};

} // namespace

FieldTrials::FieldTrials(const std::string &s)
    : uses_global_(true)
    , field_trial_string_(s)
    , previous_field_trial_string_(field_trial::GetFieldTrialString())
    , key_value_map_(InsertIntoMap(s))
{
    // TODO(bugs.webrtc.org/10335): Remove the global string!
    field_trial::InitFieldTrialsFromString(field_trial_string_.c_str());
    OCTK_CHECK(!instance_created_.exchange(true)) << "Only one instance may be instanciated at any given time!";
}

std::unique_ptr<FieldTrials> FieldTrials::CreateNoGlobal(const std::string &s)
{
    return std::unique_ptr<FieldTrials>(new FieldTrials(s, true));
}

FieldTrials::FieldTrials(const std::string &s, bool)
    : uses_global_(false)
    , previous_field_trial_string_(nullptr)
    , key_value_map_(InsertIntoMap(s))
{
}

FieldTrials::~FieldTrials()
{
    // TODO(bugs.webrtc.org/10335): Remove the global string!
    if (uses_global_)
    {
        field_trial::InitFieldTrialsFromString(previous_field_trial_string_);
        OCTK_CHECK(instance_created_.exchange(false));
    }
}

std::string FieldTrials::GetValue(StringView key) const
{
    auto it = key_value_map_.find(std::string(key));
    if (it != key_value_map_.end())
        return it->second;

    // Check the global string so that programs using
    // a mix between FieldTrials and the global string continue to work
    // TODO(bugs.webrtc.org/10335): Remove the global string!
    if (uses_global_)
    {
        return field_trial::FindFullName(std::string(key));
    }
    return "";
}


namespace field_trial
{

static const char *trials_init_string = NULL;

namespace
{

constexpr char kPersistentStringSeparator = '/';

FieldTrials::Set<std::string> &TestKeys()
{
    static auto *test_keys = new FieldTrials::Set<std::string>();
    return *test_keys;
}

// Validates the given field trial string.
//  E.g.:
//    "WebRTC-experimentFoo/Enabled/WebRTC-experimentBar/Enabled100kbps/"
//    Assigns the process to group "Enabled" on WebRTCExperimentFoo trial
//    and to group "Enabled100kbps" on WebRTCExperimentBar.
//
//  E.g. invalid config:
//    "WebRTC-experiment1/Enabled"  (note missing / separator at the end).
bool FieldTrialsStringIsValidInternal(const StringView trials)
{
    if (trials.empty())
        return true;

    size_t next_item = 0;
    std::map<StringView, StringView> field_trials;
    while (next_item < trials.length())
    {
        size_t name_end = trials.find(kPersistentStringSeparator, next_item);
        if (name_end == trials.npos || next_item == name_end)
            return false;
        size_t group_name_end = trials.find(kPersistentStringSeparator, name_end + 1);
        if (group_name_end == trials.npos || name_end + 1 == group_name_end)
            return false;
        StringView name = trials.substr(next_item, name_end - next_item);
        StringView group_name = trials.substr(name_end + 1, group_name_end - name_end - 1);

        next_item = group_name_end + 1;

        // Fail if duplicate with different group name.
        if (field_trials.find(name) != field_trials.end() && field_trials.find(name)->second != group_name)
        {
            return false;
        }

        field_trials[name] = group_name;
    }

    return true;
}

} // namespace

bool FieldTrialsStringIsValid(StringView trials_string) { return FieldTrialsStringIsValidInternal(trials_string); }

void InsertOrReplaceFieldTrialStringsInMap(std::map<std::string, std::string> *fieldtrial_map,
                                           const StringView trials_string)
{
    if (FieldTrialsStringIsValidInternal(trials_string))
    {
        std::vector<StringView> tokens = utils::split(trials_string, '/');
        // Skip last token which is empty due to trailing '/'.
        for (size_t idx = 0; idx < tokens.size() - 1; idx += 2)
        {
            (*fieldtrial_map)[std::string(tokens[idx])] = std::string(tokens[idx + 1]);
        }
    }
    else
    {
        OCTK_DCHECK_NOTREACHED() << "Invalid field trials string:" << trials_string;
    }
}

std::string MergeFieldTrialsStrings(StringView first, StringView second)
{
    std::map<std::string, std::string> fieldtrial_map;
    InsertOrReplaceFieldTrialStringsInMap(&fieldtrial_map, first);
    InsertOrReplaceFieldTrialStringsInMap(&fieldtrial_map, second);

    // Merge into fieldtrial string.
    std::string merged = "";
    for (auto const &fieldtrial : fieldtrial_map)
    {
        merged += fieldtrial.first + '/' + fieldtrial.second + '/';
    }
    return merged;
}

#ifndef WEBOCTK_EXCLUDE_FIELD_TRIAL_DEFAULT
std::string FindFullName(StringView name)
{
#    if WEBOCTK_STRICT_FIELD_TRIALS == 1
    OCTK_DCHECK(absl::c_linear_search(kRegisteredFieldTrials, name) || TestKeys().contains(name))
        << name << " is not registered, see g3doc/field-trials.md.";
#    elif WEBOCTK_STRICT_FIELD_TRIALS == 2
    OCTK_LOG_IF(LS_WARNING, !(absl::c_linear_search(kRegisteredFieldTrials, name) || TestKeys().contains(name)))
        << name << " is not registered, see g3doc/field-trials.md.";
#    endif

    if (trials_init_string == NULL)
        return std::string();

    StringView trials_string(trials_init_string);
    if (trials_string.empty())
        return std::string();

    size_t next_item = 0;
    while (next_item < trials_string.length())
    {
        // Find next name/value pair in field trial configuration string.
        size_t field_name_end = trials_string.find(kPersistentStringSeparator, next_item);
        if (field_name_end == trials_string.npos || field_name_end == next_item)
            break;
        size_t field_value_end = trials_string.find(kPersistentStringSeparator, field_name_end + 1);
        if (field_value_end == trials_string.npos || field_value_end == field_name_end + 1)
            break;
        StringView field_name = trials_string.substr(next_item, field_name_end - next_item);
        StringView field_value = trials_string.substr(field_name_end + 1, field_value_end - field_name_end - 1);
        next_item = field_value_end + 1;

        if (name == field_name)
            return std::string(field_value);
    }
    return std::string();
}
#endif // WEBOCTK_EXCLUDE_FIELD_TRIAL_DEFAULT

// Optionally initialize field trial from a string.
void InitFieldTrialsFromString(const char *trials_string)
{
    OCTK_INFO() << "Setting field trial string:" << trials_string;
    if (trials_string)
    {
        OCTK_DCHECK(FieldTrialsStringIsValidInternal(trials_string)) << "Invalid field trials string:" << trials_string;
    };
    trials_init_string = trials_string;
}

const char *GetFieldTrialString() { return trials_init_string; }

FieldTrialsAllowedInScopeForTesting::FieldTrialsAllowedInScopeForTesting(FieldTrials::Set<std::string> keys)
{
    TestKeys() = std::move(keys);
}

FieldTrialsAllowedInScopeForTesting::~FieldTrialsAllowedInScopeForTesting() { TestKeys().clear(); }


ScopedFieldTrials::ScopedFieldTrials(StringView config)
    : current_field_trials_(config)
    , previous_field_trials_(field_trial::GetFieldTrialString())
{
    OCTK_CHECK(field_trial::FieldTrialsStringIsValid(current_field_trials_.c_str()))
        << "Invalid field trials string: " << current_field_trials_;
    field_trial::InitFieldTrialsFromString(current_field_trials_.c_str());
}

ScopedFieldTrials::~ScopedFieldTrials()
{
    OCTK_CHECK(field_trial::FieldTrialsStringIsValid(previous_field_trials_))
        << "Invalid field trials string: " << previous_field_trials_;
    field_trial::InitFieldTrialsFromString(previous_field_trials_);
}

} // namespace field_trial

OCTK_END_NAMESPACE
