/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright 2016 The WebRTC Project Authors.
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

#include <octk_type_traits.hpp>
#include <octk_vector_map.hpp>
#include <octk_timestamp.hpp>
#include <octk_optional.hpp>
#include <octk_checks.hpp>
#include <octk_vector.hpp>
#include <octk_string.hpp>
#include <octk_memory.hpp>

#include <utility>

OCTK_BEGIN_NAMESPACE
//typename X, typename = traits::enable_if_t<traits::is_same_v<X, T>>
#define OCTK__RTC_STATS_ATTRIBUTE_GETTER(T, F, E)                                                                      \
    template <>                                                                                                        \
    T get<T>() const                                                                                                   \
    {                                                                                                                  \
        OCTK_CHECK(E == this->type());                                                                                 \
        return this->F();                                                                                              \
    }                                                                                                                  \
    template <>                                                                                                        \
    Optional<T> getOptional<T>() const                                                                                 \
    {                                                                                                                  \
        return E == this->type() && this->hasValue() ? utils::make_optional(this->get<T>()) : utils::nullopt;          \
    }

/**
 * @brief Abstract base class for RtcStats-derived dictionaries, see https://w3c.github.io/webrtc-stats/
 */
class RtcStats
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcStats);

    /**
     * @brief A light-weight wrapper of an RtcStats attribute, i.e. an individual metric of type Optional<T>.
     */
    class Attribute
    {
    public:
        OCTK_DEFINE_SHARED_PTR(Attribute);

        using Bool = bool;
        using Int32 = int32_t;
        using Uint32 = uint32_t;
        using Int64 = int64_t;
        using Uint64 = uint64_t;
        using Double = double;
        using String = String;

        using BoolVector = Vector<bool>;
        using Int32Vector = Vector<int32_t>;
        using Int64Vector = Vector<int64_t>;
        using Uint32Vector = Vector<uint32_t>;
        using Uint64Vector = Vector<uint64_t>;
        using DoubleVector = Vector<double>;
        using StringVector = Vector<String>;

        using StringDoubleMap = VectorMap<String, double>;
        using StringUint64Map = VectorMap<String, uint64_t>;

        enum class Type
        {
            kBool,   // bool
            kInt32,  // int32_t
            kUint32, // uint32_t
            kInt64,  // int64_t
            kUint64, // uint64_t
            kDouble, // double
            kString, // std::string

            kBoolVector,   // Vector<bool>
            kInt32Vector,  // Vector<int32_t>
            kUint32Vector, // Vector<uint32_t>
            kInt64Vector,  // Vector<int64_t>
            kUint64Vector, // Vector<uint64_t>
            kDoubleVector, // Vector<double>
            kStringVector, // Vector<std::string>

            kStringDoubleMap, // Map<std::string, double>
            kStringUint64Map, // Map<std::string, uint64_t>
        };

        virtual ~Attribute() = default;

        virtual Type type() const = 0;
        virtual bool hasValue() const = 0;
        virtual StringView name() const = 0;

        virtual Bool toBool() const = 0;
        virtual Int32 toInt32() const = 0;
        virtual Int64 toInt64() const = 0;
        virtual Uint32 toUint32() const = 0;
        virtual Uint64 toUint64() const = 0;
        virtual Double toDouble() const = 0;
        virtual String toString() const = 0;
        virtual BoolVector toBoolVector() const = 0;
        virtual Int32Vector toInt32Vector() const = 0;
        virtual Int64Vector toInt64Vector() const = 0;
        virtual Uint32Vector toUint32Vector() const = 0;
        virtual Uint64Vector toUint64Vector() const = 0;
        virtual DoubleVector toDoubleVector() const = 0;
        virtual StringVector toStringVector() const = 0;
        virtual StringUint64Map toStringUint64Map() const = 0;
        virtual StringDoubleMap toStringDoubleMap() const = 0;

        template <typename T>
        T get() const
        {
            OCTK_STATIC_ASSERT("unsupported type");
            return T();
        }
        template <typename T>
        Optional<T> getOptional() const
        {
            return utils::nullopt;
        }
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(Bool, toBool, Type::kBool)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(Int32, toInt32, Type::kInt32)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(Int64, toInt64, Type::kInt64)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(Uint32, toUint32, Type::kUint32)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(Uint64, toUint64, Type::kUint64)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(Double, toDouble, Type::kDouble)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(String, toString, Type::kString)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(BoolVector, toBoolVector, Type::kBoolVector)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(Int32Vector, toInt32Vector, Type::kInt32Vector)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(Int64Vector, toInt64Vector, Type::kInt64Vector)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(Uint32Vector, toUint32Vector, Type::kUint32Vector)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(Uint64Vector, toUint64Vector, Type::kUint64Vector)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(DoubleVector, toDoubleVector, Type::kDoubleVector)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(StringVector, toStringVector, Type::kStringVector)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(StringUint64Map, toStringUint64Map, Type::kStringUint64Map)
        OCTK__RTC_STATS_ATTRIBUTE_GETTER(StringDoubleMap, toStringDoubleMap, Type::kStringDoubleMap)
    };
    using Attributes = Vector<Attribute::SharedPtr>;

    virtual String toJson() const = 0;

    virtual StringView id() const = 0;

    virtual StringView type() const = 0;

    virtual int64_t timestamp() const = 0;

    virtual Attributes attributes() = 0;

protected:
    virtual ~RtcStats() = default;
};

OCTK_END_NAMESPACE