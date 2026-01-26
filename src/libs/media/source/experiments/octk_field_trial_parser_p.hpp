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

#include <octk_string_view.hpp>
#include <octk_optional.hpp>

#include <initializer_list>
#include <string>
#include <vector>
#include <map>
#include <set>

OCTK_BEGIN_NAMESPACE

class FieldTrialParameterInterface
{
public:
    virtual ~FieldTrialParameterInterface();
    std::string key() const { return key_; }

protected:
    // Protected to allow implementations to provide assignment and copy.
    FieldTrialParameterInterface(const FieldTrialParameterInterface &) = default;
    FieldTrialParameterInterface &operator=(const FieldTrialParameterInterface &) = default;
    explicit FieldTrialParameterInterface(StringView key);
    friend void ParseFieldTrial(std::initializer_list<FieldTrialParameterInterface *> fields, StringView trial_string);
    void MarkAsUsed() { used_ = true; }
    virtual bool Parse(Optional<std::string> str_value) = 0;

    virtual void ParseDone() { }

    std::vector<FieldTrialParameterInterface *> sub_parameters_;

private:
    std::string key_;
    bool used_ = false;
};

// ParseFieldTrial function parses the given string and fills the given fields
// with extracted values if available.
void ParseFieldTrial(std::initializer_list<FieldTrialParameterInterface *> fields, StringView trial_string);

// Specialize this in code file for custom types. Should return utils::nullopt if
// the given string cannot be properly parsed.
template <typename T>
Optional<T> ParseTypedParameter(StringView);

// This class uses the ParseTypedParameter function to implement a parameter
// implementation with an enforced default value.
template <typename T>
class FieldTrialParameter : public FieldTrialParameterInterface
{
public:
    FieldTrialParameter(StringView key, T default_value)
        : FieldTrialParameterInterface(key)
        , value_(default_value)
    {
    }
    T Get() const { return value_; }
    operator T() const { return Get(); }
    const T *operator->() const { return &value_; }

    void SetForTest(T value) { value_ = value; }

protected:
    bool Parse(Optional<std::string> str_value) override
    {
        if (str_value)
        {
            Optional<T> value = ParseTypedParameter<T>(*str_value);
            if (value.has_value())
            {
                value_ = value.value();
                return true;
            }
        }
        return false;
    }

private:
    T value_;
};

// This class uses the ParseTypedParameter function to implement a parameter
// implementation with an enforced default value and a range constraint. Values
// outside the configured range will be ignored.
template <typename T>
class FieldTrialConstrained : public FieldTrialParameterInterface
{
public:
    FieldTrialConstrained(StringView key, T default_value, Optional<T> lower_limit, Optional<T> upper_limit)
        : FieldTrialParameterInterface(key)
        , value_(default_value)
        , lower_limit_(lower_limit)
        , upper_limit_(upper_limit)
    {
    }
    T Get() const { return value_; }
    operator T() const { return Get(); }
    const T *operator->() const { return &value_; }

protected:
    bool Parse(Optional<std::string> str_value) override
    {
        if (str_value)
        {
            Optional<T> value = ParseTypedParameter<T>(*str_value);
            if (value && (!lower_limit_ || *value >= *lower_limit_) && (!upper_limit_ || *value <= *upper_limit_))
            {
                value_ = *value;
                return true;
            }
        }
        return false;
    }

private:
    T value_;
    Optional<T> lower_limit_;
    Optional<T> upper_limit_;
};

class AbstractFieldTrialEnum : public FieldTrialParameterInterface
{
public:
    AbstractFieldTrialEnum(StringView key, int default_value, std::map<std::string, int> mapping);
    ~AbstractFieldTrialEnum() override;
    AbstractFieldTrialEnum(const AbstractFieldTrialEnum &);

protected:
    bool Parse(Optional<std::string> str_value) override;

protected:
    int value_;
    std::map<std::string, int> enum_mapping_;
    std::set<int> valid_values_;
};

// The FieldTrialEnum class can be used to quickly define a parser for a
// specific enum. It handles values provided as integers and as strings if a
// mapping is provided.
template <typename T>
class FieldTrialEnum : public AbstractFieldTrialEnum
{
public:
    FieldTrialEnum(StringView key, T default_value, std::map<std::string, T> mapping)
        : AbstractFieldTrialEnum(key, static_cast<int>(default_value), ToIntMap(mapping))
    {
    }
    T Get() const { return static_cast<T>(value_); }
    operator T() const { return Get(); }

private:
    static std::map<std::string, int> ToIntMap(std::map<std::string, T> mapping)
    {
        std::map<std::string, int> res;
        for (const auto &it : mapping)
            res[it.first] = static_cast<int>(it.second);
        return res;
    }
};

// This class uses the ParseTypedParameter function to implement an optional
// parameter implementation that can default to utils::nullopt.
template <typename T>
class FieldTrialOptional : public FieldTrialParameterInterface
{
public:
    explicit FieldTrialOptional(StringView key)
        : FieldTrialParameterInterface(key)
    {
    }
    FieldTrialOptional(StringView key, Optional<T> default_value)
        : FieldTrialParameterInterface(key)
        , value_(default_value)
    {
    }
    Optional<T> GetOptional() const { return value_; }
    const T &Value() const { return value_.value(); }
    const T &operator*() const { return value_.value(); }
    const T *operator->() const { return &value_.value(); }
    explicit operator bool() const { return value_.has_value(); }

protected:
    bool Parse(Optional<std::string> str_value) override
    {
        if (str_value)
        {
            Optional<T> value = ParseTypedParameter<T>(*str_value);
            if (!value.has_value())
                return false;
            value_ = value.value();
        }
        else
        {
            value_ = utils::nullopt;
        }
        return true;
    }

private:
    Optional<T> value_;
};

// Equivalent to a FieldTrialParameter<bool> in the case that both key and value
// are present. If key is missing, evaluates to false. If key is present, but no
// explicit value is provided, the flag evaluates to true.
class FieldTrialFlag : public FieldTrialParameterInterface
{
public:
    explicit FieldTrialFlag(StringView key);
    FieldTrialFlag(StringView key, bool default_value);
    bool Get() const;
    explicit operator bool() const;

protected:
    bool Parse(Optional<std::string> str_value) override;

private:
    bool value_;
};

template <typename T>
Optional<Optional<T>> ParseOptionalParameter(StringView str)
{
    if (str.empty())
        return Optional<T>();
    auto parsed = ParseTypedParameter<T>(str);
    if (parsed.has_value())
        return parsed;
    return utils::nullopt;
}

template <>
Optional<bool> ParseTypedParameter<bool>(StringView str);
template <>
Optional<double> ParseTypedParameter<double>(StringView str);
template <>
Optional<int> ParseTypedParameter<int>(StringView str);
template <>
Optional<unsigned> ParseTypedParameter<unsigned>(StringView str);
template <>
Optional<std::string> ParseTypedParameter<std::string>(StringView str);

template <>
Optional<Optional<bool>> ParseTypedParameter<Optional<bool>>(StringView str);
template <>
Optional<Optional<int>> ParseTypedParameter<Optional<int>>(StringView str);
template <>
Optional<Optional<unsigned>> ParseTypedParameter<Optional<unsigned>>(StringView str);
template <>
Optional<Optional<double>> ParseTypedParameter<Optional<double>>(StringView str);

// Accepts true, false, else parsed with sscanf %i, true if != 0.
extern template class FieldTrialParameter<bool>;
// Interpreted using sscanf %lf.
extern template class FieldTrialParameter<double>;
// Interpreted using sscanf %i.
extern template class FieldTrialParameter<int>;
// Interpreted using sscanf %u.
extern template class FieldTrialParameter<unsigned>;
// Using the given value as is.
extern template class FieldTrialParameter<std::string>;

extern template class FieldTrialConstrained<double>;
extern template class FieldTrialConstrained<int>;
extern template class FieldTrialConstrained<unsigned>;

extern template class FieldTrialOptional<double>;
extern template class FieldTrialOptional<int>;
extern template class FieldTrialOptional<unsigned>;
extern template class FieldTrialOptional<bool>;
extern template class FieldTrialOptional<std::string>;

OCTK_END_NAMESPACE