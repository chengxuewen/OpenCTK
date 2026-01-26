/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright 2018 The WebRTC project authors. All Rights Reserved.
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

#include <private/octk_field_trial_parser_p.hpp>
#include <octk_string_view.hpp>
#include <octk_time_delta.hpp>
#include <octk_data_rate.hpp>
#include <octk_data_size.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

template <>
Optional<DataRate> ParseTypedParameter<DataRate>(StringView str);
template <>
Optional<DataSize> ParseTypedParameter<DataSize>(StringView str);
template <>
Optional<TimeDelta> ParseTypedParameter<TimeDelta>(StringView str);

extern template class FieldTrialParameter<DataRate>;
extern template class FieldTrialParameter<DataSize>;
extern template class FieldTrialParameter<TimeDelta>;

extern template class FieldTrialConstrained<DataRate>;
extern template class FieldTrialConstrained<DataSize>;
extern template class FieldTrialConstrained<TimeDelta>;

extern template class FieldTrialOptional<DataRate>;
extern template class FieldTrialOptional<DataSize>;
extern template class FieldTrialOptional<TimeDelta>;

OCTK_END_NAMESPACE
