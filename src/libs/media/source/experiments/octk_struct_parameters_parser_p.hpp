/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#pragma once

#include <private/octk_field_trial_parser_p.hpp>
#include <private/octk_field_trial_units_p.hpp>
#include <octk_string_encode.hpp>
#include <octk_string_view.hpp>
#include <octk_optional.hpp>
#include <octk_memory.hpp>

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <map>

OCTK_BEGIN_NAMESPACE

namespace detail
{
struct TypedMemberParser
{
public:
    bool (*parse)(StringView src, void *target);
    void (*encode)(const void *src, std::string *target);
};

struct MemberParameter
{
    const char *key;
    void *member_ptr;
    TypedMemberParser parser;
};

template <typename T>
class TypedParser
{
public:
    static bool Parse(StringView src, void *target);
    static void Encode(const void *src, std::string *target);
};

// Instantiated in cc file to avoid duplication during compile. Add additional
// parsers as needed. Generally, try to use these suggested types even if the
// context where the value is used might require a different type. For instance,
// a size_t representing a packet size should use an int parameter as there's no
// need to support packet sizes larger than INT32_MAX.
extern template class TypedParser<bool>;
extern template class TypedParser<double>;
extern template class TypedParser<int>;
extern template class TypedParser<unsigned>;
extern template class TypedParser<Optional<double>>;
extern template class TypedParser<Optional<int>>;
extern template class TypedParser<Optional<unsigned>>;

extern template class TypedParser<DataRate>;
extern template class TypedParser<DataSize>;
extern template class TypedParser<TimeDelta>;
extern template class TypedParser<Optional<DataRate>>;
extern template class TypedParser<Optional<DataSize>>;
extern template class TypedParser<Optional<TimeDelta>>;

template <typename T>
void AddMembers(MemberParameter *out, const char *key, T *member)
{
    *out = MemberParameter{key, member, TypedMemberParser{&TypedParser<T>::Parse, &TypedParser<T>::Encode}};
}

template <typename T, typename... Args>
void AddMembers(MemberParameter *out, const char *key, T *member, Args... args)
{
    AddMembers(out, key, member);
    AddMembers(++out, args...);
}
} // namespace detail

class StructParametersParser
{
public:
    template <typename T, typename... Args>
    static std::unique_ptr<StructParametersParser> Create(const char *first_key, T *first_member, Args... args)
    {
        std::vector<detail::MemberParameter> members(sizeof...(args) / 2 + 1);
        detail::AddMembers(&members.front(), std::move(first_key), first_member, args...);
        return std::unique_ptr<StructParametersParser>(new StructParametersParser(std::move(members)));
    }

    void Parse(StringView src);
    std::string Encode() const;

private:
    explicit StructParametersParser(std::vector<detail::MemberParameter> members);

    std::vector<detail::MemberParameter> members_;
};

OCTK_END_NAMESPACE
