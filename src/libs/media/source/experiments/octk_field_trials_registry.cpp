/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright 2022 The WebRTC project authors. All Rights Reserved.
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

#include <private/octk_field_trials_registry_p.hpp>
#include <private/octk_field_trials_p.hpp>
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
