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

#include <private/octk_field_trial_list_p.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

FieldTrialListBase::FieldTrialListBase(StringView key)
    : FieldTrialParameterInterface(key)
    , failed_(false)
    , parse_got_called_(false)
{
}

bool FieldTrialListBase::Failed() const
{
    return failed_;
}
bool FieldTrialListBase::Used() const
{
    return parse_got_called_;
}

int FieldTrialListWrapper::Length()
{
    return GetList()->Size();
}
bool FieldTrialListWrapper::Failed()
{
    return GetList()->Failed();
}
bool FieldTrialListWrapper::Used()
{
    return GetList()->Used();
}

bool FieldTrialStructListBase::Parse(Optional<std::string> str_value)
{
    OCTK_DCHECK_NOTREACHED();
    return true;
}

int FieldTrialStructListBase::ValidateAndGetLength()
{
    int length = -1;
    for (std::unique_ptr<FieldTrialListWrapper> &list : sub_lists_)
    {
        if (list->Failed())
            return -1;
        else if (!list->Used())
            continue;
        else if (length == -1)
            length = list->Length();
        else if (length != list->Length())
            return -1;
    }

    return length;
}

OCTK_END_NAMESPACE