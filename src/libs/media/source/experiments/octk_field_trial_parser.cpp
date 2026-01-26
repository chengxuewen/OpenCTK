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

#include <private/octk_field_trial_parser_p.hpp>
#include <octk_safe_conversions.hpp>
#include <octk_checks.hpp>

#include <type_traits>
#include <cinttypes>
#include <algorithm>
#include <utility>
#include <map>

OCTK_BEGIN_NAMESPACE

FieldTrialParameterInterface::FieldTrialParameterInterface(StringView key)
    : key_(key)
{
}
FieldTrialParameterInterface::~FieldTrialParameterInterface()
{
    OCTK_DCHECK(used_) << "Field trial parameter with key: '" << key_ << "' never used.";
}

void ParseFieldTrial(std::initializer_list<FieldTrialParameterInterface *> fields, StringView trial_string)
{
    std::map<StringView, FieldTrialParameterInterface *> field_map;
    FieldTrialParameterInterface *keyless_field = nullptr;
    for (FieldTrialParameterInterface *field : fields)
    {
        field->MarkAsUsed();
        if (!field->sub_parameters_.empty())
        {
            for (FieldTrialParameterInterface *sub_field : field->sub_parameters_)
            {
                OCTK_DCHECK(!sub_field->key_.empty());
                sub_field->MarkAsUsed();
                field_map[sub_field->key_] = sub_field;
            }
            continue;
        }

        if (field->key_.empty())
        {
            OCTK_DCHECK(!keyless_field);
            keyless_field = field;
        }
        else
        {
            field_map[field->key_] = field;
        }
    }
    bool logged_unknown_key = false;

    StringView tail = trial_string;
    while (!tail.empty())
    {
        size_t key_end = tail.find_first_of(",:");
        StringView key = tail.substr(0, key_end);
        Optional<std::string> opt_value;
        if (key_end == StringView::npos)
        {
            tail = "";
        }
        else if (tail[key_end] == ':')
        {
            tail = tail.substr(key_end + 1);
            size_t value_end = tail.find(',');
            opt_value.emplace(tail.substr(0, value_end));
            if (value_end == StringView::npos)
            {
                tail = "";
            }
            else
            {
                tail = tail.substr(value_end + 1);
            }
        }
        else
        {
            OCTK_DCHECK_EQ(tail[key_end], ',');
            tail = tail.substr(key_end + 1);
        }

        auto field = field_map.find(key);
        if (field != field_map.end())
        {
            if (!field->second->Parse(std::move(opt_value)))
            {
                OCTK_WARNING() << "Failed to read field with key: '" << key << "' in trial: \"" << trial_string << "\"";
            }
        }
        else if (!opt_value && keyless_field && !key.empty())
        {
            if (!keyless_field->Parse(std::string(key)))
            {
                OCTK_WARNING() << "Failed to read empty key field with value '" << key << "' in trial: \""
                               << trial_string << "\"";
            }
        }
        else if (key.empty() || key[0] != '_')
        {
            // "_" is be used to prefix keys that are part of the string for
            // debugging purposes but not neccessarily used.
            // e.g. WebRTC-Experiment/param: value, _DebuggingString
            if (!logged_unknown_key)
            {
                OCTK_INFO() << "No field with key: '" << key << "' (found in trial: \"" << trial_string << "\")";
                std::string valid_keys;
                for (const auto &f : field_map)
                {
                    valid_keys.append(f.first.data(), f.first.size());
                    valid_keys += ", ";
                }
                OCTK_INFO() << "Valid keys are: " << valid_keys;
                logged_unknown_key = true;
            }
        }
    }

    for (FieldTrialParameterInterface *field : fields)
    {
        field->ParseDone();
    }
}

template <>
Optional<bool> ParseTypedParameter<bool>(StringView str)
{
    if (str == "true" || str == "1")
    {
        return true;
    }
    else if (str == "false" || str == "0")
    {
        return false;
    }
    return utils::nullopt;
}

template <>
Optional<double> ParseTypedParameter<double>(StringView str)
{
    double value;
    char unit[2]{0, 0};
    if (sscanf(std::string(str).c_str(), "%lf%1s", &value, unit) >= 1)
    {
        if (unit[0] == '%')
            return value / 100;
        return value;
    }
    else
    {
        return utils::nullopt;
    }
}

template <>
Optional<int> ParseTypedParameter<int>(StringView str)
{
    int64_t value;
    if (sscanf(std::string(str).c_str(), "%" SCNd64, &value) == 1)
    {
        if (utils::IsValueInRangeForNumericType<int, int64_t>(value))
        {
            return static_cast<int>(value);
        }
    }
    return utils::nullopt;
}

template <>
Optional<unsigned> ParseTypedParameter<unsigned>(StringView str)
{
    int64_t value;
    if (sscanf(std::string(str).c_str(), "%" SCNd64, &value) == 1)
    {
        if (utils::IsValueInRangeForNumericType<unsigned, int64_t>(value))
        {
            return static_cast<unsigned>(value);
        }
    }
    return utils::nullopt;
}

template <>
Optional<std::string> ParseTypedParameter<std::string>(StringView str)
{
    return std::string(str);
}

template <>
Optional<Optional<bool>> ParseTypedParameter<Optional<bool>>(StringView str)
{
    return ParseOptionalParameter<bool>(str);
}
template <>
Optional<Optional<int>> ParseTypedParameter<Optional<int>>(StringView str)
{
    return ParseOptionalParameter<int>(str);
}
template <>
Optional<Optional<unsigned>> ParseTypedParameter<Optional<unsigned>>(StringView str)
{
    return ParseOptionalParameter<unsigned>(str);
}
template <>
Optional<Optional<double>> ParseTypedParameter<Optional<double>>(StringView str)
{
    return ParseOptionalParameter<double>(str);
}

FieldTrialFlag::FieldTrialFlag(StringView key)
    : FieldTrialFlag(key, false)
{
}

FieldTrialFlag::FieldTrialFlag(StringView key, bool default_value)
    : FieldTrialParameterInterface(key)
    , value_(default_value)
{
}

bool FieldTrialFlag::Get() const
{
    return value_;
}

FieldTrialFlag::operator bool() const
{
    return value_;
}

bool FieldTrialFlag::Parse(Optional<std::string> str_value)
{
    // Only set the flag if there is no argument provided.
    if (str_value)
    {
        Optional<bool> opt_value = ParseTypedParameter<bool>(*str_value);
        if (!opt_value)
            return false;
        value_ = *opt_value;
    }
    else
    {
        value_ = true;
    }
    return true;
}

AbstractFieldTrialEnum::AbstractFieldTrialEnum(StringView key, int default_value, std::map<std::string, int> mapping)
    : FieldTrialParameterInterface(key)
    , value_(default_value)
    , enum_mapping_(mapping)
{
    for (auto &key_val : enum_mapping_)
        valid_values_.insert(key_val.second);
}
AbstractFieldTrialEnum::AbstractFieldTrialEnum(const AbstractFieldTrialEnum &) = default;
AbstractFieldTrialEnum::~AbstractFieldTrialEnum() = default;

bool AbstractFieldTrialEnum::Parse(Optional<std::string> str_value)
{
    if (str_value)
    {
        auto it = enum_mapping_.find(*str_value);
        if (it != enum_mapping_.end())
        {
            value_ = it->second;
            return true;
        }
        Optional<int> value = ParseTypedParameter<int>(*str_value);
        if (value.has_value() && (valid_values_.find(*value) != valid_values_.end()))
        {
            value_ = *value;
            return true;
        }
    }
    return false;
}

template class FieldTrialParameter<bool>;
template class FieldTrialParameter<double>;
template class FieldTrialParameter<int>;
template class FieldTrialParameter<unsigned>;
template class FieldTrialParameter<std::string>;

template class FieldTrialConstrained<double>;
template class FieldTrialConstrained<int>;
template class FieldTrialConstrained<unsigned>;

template class FieldTrialOptional<double>;
template class FieldTrialOptional<int>;
template class FieldTrialOptional<unsigned>;
template class FieldTrialOptional<bool>;
template class FieldTrialOptional<std::string>;

OCTK_END_NAMESPACE