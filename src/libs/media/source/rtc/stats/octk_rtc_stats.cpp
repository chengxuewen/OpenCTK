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

#include <octk_rtc_stats.hpp>
#include <octk_string_encode.hpp>

OCTK_BEGIN_NAMESPACE

namespace
{
struct VisitHasValue
{
    template <typename T> bool operator()(const Optional<T> *attribute) { return attribute->has_value(); }
};

struct VisitIsSequence
{
    // Any type of vector is a sequence.
    template <typename T> bool operator()(const Optional<std::vector<T>> *attribute) { return true; }
    // Any other type is not.
    template <typename T> bool operator()(const Optional<T> *attribute) { return false; }
};

// Converts the attribute to string in a JSON-compatible way.
struct VisitToString
{
    template <typename T,
              typename std::enable_if<std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value ||
                                          std::is_same<T, bool>::value || std::is_same<T, std::string>::value,
                                      bool>::type = true>
    std::string ValueToString(const T &value)
    {
        return utils::toString(value);
    }
    // Convert 64-bit integers to doubles before converting to string because JSON
    // represents all numbers as floating points with ~15 digits of precision.
    template <typename T,
              typename std::enable_if<std::is_same<T, int64_t>::value || std::is_same<T, uint64_t>::value ||
                                          std::is_same<T, double>::value,
                                      bool>::type = true>
    std::string ValueToString(const T &value)
    {
        char buf[32];
        const int len = std::snprintf(&buf[0], OCTK_ARRAY_SIZE(buf), "%.16g", static_cast<double>(value));
        OCTK_DCHECK_LE(len, OCTK_ARRAY_SIZE(buf));
        return std::string(&buf[0], len);
    }

    // Vector attributes.
    template <typename T> std::string operator()(const Optional<std::vector<T>> *attribute)
    {
        std::stringstream ss;
        ss << "[";
        const char *separator = "";
        constexpr bool element_is_string = std::is_same<T, std::string>::value;
        for (const T &element : attribute->value())
        {
            ss << separator;
            if (element_is_string)
            {
                ss << "\"";
            }
            ss << ValueToString(element);
            if (element_is_string)
            {
                ss << "\"";
            }
            separator = ",";
        }
        ss << "]";
        return ss.str();
    }
    // Map attributes.
    template <typename T> std::string operator()(const Optional<std::map<std::string, T>> *attribute)
    {
        std::stringstream ss;
        ss << "{";
        const char *separator = "";
        constexpr bool element_is_string = std::is_same<T, std::string>::value;
        for (const auto &pair : attribute->value())
        {
            ss << separator;
            ss << "\"" << pair.first << "\":";
            if (element_is_string)
            {
                ss << "\"";
            }
            ss << ValueToString(pair.second);
            if (element_is_string)
            {
                ss << "\"";
            }
            separator = ",";
        }
        ss << "}";
        return ss.str();
    }
    // Simple attributes.
    template <typename T> std::string operator()(const Optional<T> *attribute)
    {
        return ValueToString(attribute->value());
    }
};

struct VisitIsEqual
{
    template <typename T> bool operator()(const Optional<T> *attribute)
    {
        if (!other.holdsAlternative<T>())
        {
            return false;
        }
        return *attribute == other.asOptional<T>();
    }

    const RtcStats::Attribute &other;
};

} // namespace

bool RtcStats::Attribute::hasValue() const { return utils::visit(VisitHasValue{}, mAttribute); }

bool RtcStats::Attribute::isSequence() const { return utils::visit(VisitIsSequence{}, mAttribute); }

bool RtcStats::Attribute::isString() const
{
    return utils::holds_alternative<const Optional<std::string> *>(mAttribute);
}

std::string RtcStats::Attribute::toString() const
{
    if (!this->hasValue())
    {
        return "null";
    }
    return utils::visit(VisitToString{}, mAttribute);
}

bool RtcStats::Attribute::operator==(const RtcStats::Attribute &other) const
{
    return utils::visit(VisitIsEqual{other}, mAttribute);
}

bool RtcStats::Attribute::operator!=(const RtcStats::Attribute &other) const { return !(*this == other); }

RtcStats::RtcStats(const RtcStats &other)
    : RtcStats(other.mId, other.mTimestamp)
{
}

RtcStats::~RtcStats() { }

bool RtcStats::operator==(const RtcStats &other) const
{
    if (type() != other.type() || id() != other.id())
    {
        return false;
    }
    std::vector<Attribute> attributes = this->attributes();
    std::vector<Attribute> other_attributes = other.attributes();
    OCTK_DCHECK_EQ(attributes.size(), other_attributes.size());
    for (size_t i = 0; i < attributes.size(); ++i)
    {
        if (attributes[i] != other_attributes[i])
        {
            return false;
        }
    }
    return true;
}

bool RtcStats::operator!=(const RtcStats &other) const { return !(*this == other); }

std::string RtcStats::toJson() const
{
    std::stringstream ss;
    ss << "{\"type\":\"" << type()
       << "\","
          "\"id\":\""
       << mId
       << "\","
          "\"timestamp\":"
       << mTimestamp.us();
    for (const Attribute &attribute : attributes())
    {
        if (attribute.hasValue())
        {
            ss << ",\"" << attribute.name() << "\":";
            if (attribute.holdsAlternative<std::string>())
            {
                ss << "\"";
            }
            ss << attribute.toString();
            if (attribute.holdsAlternative<std::string>())
            {
                ss << "\"";
            }
        }
    }
    ss << "}";
    return ss.str();
}

std::vector<RtcStats::Attribute> RtcStats::attributes() const { return attributesImpl(0); }

std::vector<RtcStats::Attribute> RtcStats::attributesImpl(size_t additional_capacity) const
{
    std::vector<Attribute> attributes;
    attributes.reserve(additional_capacity);
    return attributes;
}

OCTK_END_NAMESPACE
