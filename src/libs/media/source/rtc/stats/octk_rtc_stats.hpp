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

#ifndef _OCTK_RTC_STATS_HPP
#define _OCTK_RTC_STATS_HPP

#include <octk_media_global.hpp>
#include <octk_timestamp.hpp>
#include <octk_optional.hpp>
#include <octk_variant.hpp>
#include <octk_memory.hpp>
#include <octk_checks.hpp>

#include <stddef.h>
#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

OCTK_BEGIN_NAMESPACE

/**
 * @brief Abstract base class for RtcStats-derived dictionaries, see https://w3c.github.io/webrtc-stats/
 *
 * All derived classes must have the following static variable defined:
 *      static const char kType[];
 * It is used as a unique class identifier and a string representation of the class type,
 * see https://w3c.github.io/webrtc-stats/#rtcstatstype-str*.
 * Use the `OCTK_IMPLEMENT_RTCSTATS` macro when implementing subclasses, see macro for details.
 *
 * Derived classes list their dictionary attributes, Optional<T>, as public fields, allowing the following:
 *      RTCFooStats foo("fooId", Timestamp::Micros(GetCurrentTime()));
 *      foo.bar = 42;
 *      foo.baz = std::vector<std::string>();
 *      foo.baz->push_back("hello world");
 *      uint32_t x = *foo.bar;
 *
 * Pointers to all the attributes are available with `attributes()`, allowing iteration:
 *      for (const auto& attribute : foo.attributes()) {
 *          printf("%s = %s\n", attribute.name(), attribute.toString().c_str());
 *      }
 */
class OCTK_MEDIA_API RtcStats
{
public:
    /**
     * @brief A light-weight wrapper of an RtcStats attribute, i.e. an individual metric of type Optional<T>.
     */
    class OCTK_MEDIA_API Attribute
    {
    public:
        // All supported attribute types.
        using StatVariant = Variant<const Optional<bool> *,
                                    const Optional<int32_t> *,
                                    const Optional<uint32_t> *,
                                    const Optional<int64_t> *,
                                    const Optional<uint64_t> *,
                                    const Optional<double> *,
                                    const Optional<std::string> *,
                                    const Optional<std::vector<bool>> *,
                                    const Optional<std::vector<int32_t>> *,
                                    const Optional<std::vector<uint32_t>> *,
                                    const Optional<std::vector<int64_t>> *,
                                    const Optional<std::vector<uint64_t>> *,
                                    const Optional<std::vector<double>> *,
                                    const Optional<std::vector<std::string>> *,
                                    const Optional<std::map<std::string, uint64_t>> *,
                                    const Optional<std::map<std::string, double>> *>;
        struct VisitMakeAttribute
        {
            template <typename T> Attribute operator()(const Optional<T> *attribute)
            {
                return Attribute(name, attribute);
            }
            const char *name;
        };

        template <typename T>
        Attribute(const char *name, const Optional<T> *attribute)
            : mName(name)
            , mAttribute(attribute)
        {
        }

        const char *name() const { return mName; }
        const StatVariant &asVariant() const { return mAttribute; }

        bool hasValue() const;
        template <typename T> bool holdsAlternative() const
        {
            return utils::holds_alternative<const Optional<T> *>(mAttribute);
        }
        template <typename T> const Optional<T> &asOptional() const
        {
            OCTK_CHECK(holdsAlternative<T>());
            return *utils::get<const Optional<T> *>(mAttribute);
        }
        template <typename T> const T &get() const
        {
            OCTK_CHECK(this->holdsAlternative<T>());
            OCTK_CHECK(this->hasValue());
            return utils::get<const Optional<T> *>(mAttribute)->value();
        }

        bool isSequence() const;
        bool isString() const;
        std::string toString() const;

        bool operator==(const Attribute &other) const;
        bool operator!=(const Attribute &other) const;

    protected:
    private:
        const char *mName;
        StatVariant mAttribute;
    };
    using Attributes = std::vector<Attribute>;

    struct OCTK_MEDIA_API AttributeInit
    {
        AttributeInit(const char *name, const Attribute::StatVariant &variant)
            : name(name)
            , variant(variant)
        {
        }

        Attribute toAttribute() { return utils::visit(Attribute::VisitMakeAttribute{name}, variant); }

        const char *name;
        Attribute::StatVariant variant;
    };

    RtcStats(const std::string &id, Timestamp timestamp)
        : mId(id)
        , mTimestamp(timestamp)
    {
    }
    RtcStats(const RtcStats &other);
    virtual ~RtcStats();

    virtual std::unique_ptr<RtcStats> copy() const = 0;

    const std::string &id() const { return mId; }
    // Time relative to the UNIX epoch (Jan 1, 1970, UTC), in microseconds.
    Timestamp timestamp() const { return mTimestamp; }

    // Returns the static member variable `kType` of the implementing class.
    virtual const char *type() const = 0;
    // Returns all attributes of this stats object, i.e. a list of its individual
    // metrics as viewed via the Attribute wrapper.
    std::vector<Attribute> attributes() const;

    template <typename T> Attribute attribute(const Optional<T> &stat) const
    {
        for (const auto &attribute : attributes())
        {
            if (!attribute.holdsAlternative<T>())
            {
                continue;
            }
            if (utils::get<const Optional<T> *>(attribute.asVariant()) == &stat)
            {
                return attribute;
            }
        }
        OCTK_CHECK_NOTREACHED();
        static Optional<bool> boolOpt;
        return Attribute("", &boolOpt);
    }

    /**
     * @brief Checks if the two stats objects are of the same type and have the same attribute values.
     * Timestamps are not compared. These operators are exposed for testing.
     * @param other
     * @return
     */
    bool operator==(const RtcStats &other) const;

    bool operator!=(const RtcStats &other) const;

    /**
     * @brief Creates a JSON readable string representation of the stats object,
     * listing all of its attributes (names and values).
     * @return
     */
    std::string toJson() const;

    /**
     * @brief Downcasts the stats object to an `RtcStats` subclass `T`. DCHECKs that the object is of type `T`.
     * @tparam T
     * @return
     */
    template <typename T> const T &cast_to() const
    {
        OCTK_DCHECK_EQ(type(), T::kType);
        return static_cast<const T &>(*this);
    }

protected:
    virtual std::vector<Attribute> attributesImpl(size_t additionalCapacity) const;

    std::string const mId;
    Timestamp mTimestamp;
};

OCTK_END_NAMESPACE

/**
 * @brief All `RtcStats` classes should use these macros.
 *  `OCTK_DECLARE_RTCSTATS` is placed in a public section of the class definition.
 *  `OCTK_IMPLEMENT_RTCSTATS` is placed outside the class definition (in a .cc).
 *
 *  These macros declare (in _DECL) and define (in _IMPL) the static `kType` and overrides methods as  required by
 *  subclasses of `RtcStats`: `copy`, `type` and `attributesImpl`. The |...| argument is a list of addresses to each
 *  attribute defined in the implementing class. The list must have at least one attribute.
 *
 *  (Since class names need to be known to implement these methods this cannot be part of the base `RtcStats`.
 *  While these methods could be implemented using templates, that would only work for immediate subclasses.
 *  Subclasses of subclasses also have to override these methods, resulting in boilerplate code.
 *  Using a macro avoids this and works for any `RtcStats` class, including grandchildren.)
 *
 *  Sample usage:
 *  @code
 *  rtcfoostats.h:
 *  class RTCFooStats : public RtcStats {
 *  public:
 *      OCTK_DECLARE_RTCSTATS();
 *
 *      RTCFooStats(const std::string& id, Timestamp timestamp);
 *
 *      std::optional<int32_t> foo;
 *      std::optional<int32_t> bar;
 *  };
 *
 *  rtcfoostats.cc:
 *  OCTK_IMPLEMENT_RTCSTATS(RTCFooStats, RtcStats, "foo-stats" &foo, &bar);
 *
 *  RTCFooStats::RTCFooStats(const std::string& id, Timestamp timestamp)
 *  : RtcStats(id, timestamp), foo("foo"), bar("bar") {   }
 */
#define OCTK_DECLARE_RTCSTATS()                                                                                        \
protected:                                                                                                             \
    std::vector<octk::RtcStats::Attribute> attributesImpl(size_t additionalCapacity) const override;                   \
                                                                                                                       \
public:                                                                                                                \
    static const char kType[];                                                                                         \
                                                                                                                       \
    std::unique_ptr<octk::RtcStats> copy() const override;                                                             \
    const char *type() const override

#define OCTK_IMPLEMENT_RTCSTATS(ThisClass, ParentClass, TypeString, ...)                                               \
    const char ThisClass::kType[] = TypeString;                                                                        \
                                                                                                                       \
    std::unique_ptr<octk::RtcStats> ThisClass::copy() const { return octk::utils::make_unique<ThisClass>(*this); }     \
                                                                                                                       \
    const char *ThisClass::type() const { return ThisClass::kType; }                                                   \
                                                                                                                       \
    std::vector<octk::RtcStats::Attribute> ThisClass::attributesImpl(size_t additionalCapacity) const                  \
    {                                                                                                                  \
        octk::RtcStats::AttributeInit attributeInits[] = {__VA_ARGS__};                                                \
        size_t attributeInitsSize = sizeof(attributeInits) / sizeof(attributeInits[0]);                                \
        std::vector<octk::RtcStats::Attribute> attributes =                                                            \
            ParentClass::attributesImpl(attributeInitsSize + additionalCapacity);                                      \
        for (size_t i = 0; i < attributeInitsSize; ++i)                                                                \
        {                                                                                                              \
            attributes.push_back(attributeInits[i].toAttribute());                                                     \
        }                                                                                                              \
        return attributes;                                                                                             \
    }

#endif // _OCTK_RTC_STATS_HPP
