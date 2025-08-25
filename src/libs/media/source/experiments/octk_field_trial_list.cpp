//
// Created by cxw on 25-8-15.
//

#include <octk_field_trial_list.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

FieldTrialListBase::FieldTrialListBase(StringView key)
    : FieldTrialParameterInterface(key)
    , failed_(false)
    , parse_got_called_(false)
{
}

bool FieldTrialListBase::Failed() const { return failed_; }
bool FieldTrialListBase::Used() const { return parse_got_called_; }

int FieldTrialListWrapper::Length() { return GetList()->Size(); }
bool FieldTrialListWrapper::Failed() { return GetList()->Failed(); }
bool FieldTrialListWrapper::Used() { return GetList()->Used(); }

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