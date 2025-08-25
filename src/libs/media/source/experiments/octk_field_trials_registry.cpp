//
// Created by cxw on 25-8-15.
//

#include <octk_field_trials_registry.hpp>
#include <octk_field_trials.hpp>
#include <octk_logging.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

std::string FieldTrialsRegistry::Lookup(StringView key) const
{
#if WEBRTC_STRICT_FIELD_TRIALS == 1
    OCTK_DCHECK(absl::c_linear_search(kRegisteredFieldTrials, key) || test_keys_.contains(key))
        << key << " is not registered, see g3doc/field-trials.md.";
#elif WEBRTC_STRICT_FIELD_TRIALS == 2
    RTC_LOG_IF(LS_WARNING, !(absl::c_linear_search(kRegisteredFieldTrials, key) || test_keys_.contains(key)))
        << key << " is not registered, see g3doc/field-trials.md.";
#endif
    return GetValue(key);
}

OCTK_END_NAMESPACE
