/*
*  Copyright 2016 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#include <octk_rtc_stats.hpp>
#include <octk_json.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>

using namespace octk;

namespace
{
// JSON stores numbers as floating point numbers with 53 significant bits, which
// amounts to about 15.95 decimal digits. Thus, when comparing large numbers
// processed by JSON, that's all the precision we should expect.
const double JSON_EPSILON = 1e-15;

// We do this since Google Test doesn't support relative error.
// This is computed as follows:
// If |a - b| / |a| < EPS, then |a - b| < |a| * EPS, so |a| * EPS is the
// maximum expected error.
double GetExpectedError(const double expected_value) { return JSON_EPSILON * fabs(expected_value); }

} // namespace

class RtcTestStats : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcTestStats(const std::string &id, Timestamp timestamp)
        : RtcStats(id, timestamp)
    {
    }
    ~RtcTestStats() override { }

    Optional<bool> m_bool;
    Optional<int32_t> m_int32;
    Optional<uint32_t> m_uint32;
    Optional<int64_t> m_int64;
    Optional<uint64_t> m_uint64;
    Optional<double> m_double;
    Optional<std::string> m_string;
    Optional<std::vector<bool>> m_sequence_bool;
    Optional<std::vector<int32_t>> m_sequence_int32;
    Optional<std::vector<uint32_t>> m_sequence_uint32;
    Optional<std::vector<int64_t>> m_sequence_int64;
    Optional<std::vector<uint64_t>> m_sequence_uint64;
    Optional<std::vector<double>> m_sequence_double;
    Optional<std::vector<std::string>> m_sequence_string;
    Optional<std::map<std::string, uint64_t>> m_map_string_uint64;
    Optional<std::map<std::string, double>> m_map_string_double;
};

OCTK_IMPLEMENT_RTCSTATS(RtcTestStats,
                        RtcStats,
                        "test-stats",
                        AttributeInit("mBool", &m_bool),
                        AttributeInit("mInt32", &m_int32),
                        AttributeInit("mUint32", &m_uint32),
                        AttributeInit("mInt64", &m_int64),
                        AttributeInit("mUint64", &m_uint64),
                        AttributeInit("mDouble", &m_double),
                        AttributeInit("mString", &m_string),
                        AttributeInit("mSequenceBool", &m_sequence_bool),
                        AttributeInit("mSequenceInt32", &m_sequence_int32),
                        AttributeInit("mSequenceUint32", &m_sequence_uint32),
                        AttributeInit("mSequenceInt64", &m_sequence_int64),
                        AttributeInit("mSequenceUint64", &m_sequence_uint64),
                        AttributeInit("mSequenceDouble", &m_sequence_double),
                        AttributeInit("mSequenceString", &m_sequence_string),
                        AttributeInit("mMapStringUint64", &m_map_string_uint64),
                        AttributeInit("mMapStringDouble", &m_map_string_double))

class RtcChildStats : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();

    RtcChildStats(const std::string &id, Timestamp timestamp)
        : RtcStats(id, timestamp)
    {
    }

    Optional<int32_t> child_int;
};

OCTK_IMPLEMENT_RTCSTATS(RtcChildStats, RtcStats, "child-stats", AttributeInit("childInt", &child_int))

class RtcGrandChildStats : public RtcChildStats
{
public:
    OCTK_DECLARE_RTCSTATS();

    RtcGrandChildStats(const std::string &id, Timestamp timestamp)
        : RtcChildStats(id, timestamp)
    {
    }

    Optional<int32_t> grandchild_int;
};

OCTK_IMPLEMENT_RTCSTATS(RtcGrandChildStats,
                        RtcChildStats,
                        "grandchild-stats",
                        AttributeInit("grandchildInt", &grandchild_int))

TEST(RtcStatsTest, RtcStatsAndAttributes)
{
    RtcTestStats stats("testId", Timestamp::Micros(42));
    EXPECT_EQ(stats.id(), "testId");
    EXPECT_EQ(stats.timestamp().us(), static_cast<int64_t>(42));
    std::vector<RtcStats::Attribute> attributes = stats.attributes();
    EXPECT_EQ(attributes.size(), static_cast<size_t>(16));
    for (const auto &attribute : attributes)
    {
        EXPECT_FALSE(attribute.hasValue());
    }
    stats.m_bool = true;
    stats.m_int32 = 123;
    stats.m_uint32 = 123;
    stats.m_int64 = 123;
    stats.m_uint64 = 123;
    stats.m_double = 123.0;
    stats.m_string = std::string("123");

    std::vector<bool> sequence_bool;
    sequence_bool.push_back(true);
    std::vector<int32_t> sequence_int32;
    sequence_int32.push_back(static_cast<int32_t>(1));
    std::vector<uint32_t> sequence_uint32;
    sequence_uint32.push_back(static_cast<uint32_t>(2));
    std::vector<int64_t> sequence_int64;
    sequence_int64.push_back(static_cast<int64_t>(3));
    std::vector<uint64_t> sequence_uint64;
    sequence_uint64.push_back(static_cast<uint64_t>(4));
    std::vector<double> sequence_double;
    sequence_double.push_back(5.0);
    std::vector<std::string> sequence_string;
    sequence_string.push_back(std::string("six"));

    std::map<std::string, uint64_t> map_string_uint64{{"seven", 8}};
    std::map<std::string, double> map_string_double{{"nine", 10.0}};

    stats.m_sequence_bool = sequence_bool;
    stats.m_sequence_int32 = sequence_int32;
    stats.m_sequence_uint32 = sequence_uint32;
    EXPECT_FALSE(stats.m_sequence_int64.has_value());
    stats.m_sequence_int64 = sequence_int64;
    stats.m_sequence_uint64 = sequence_uint64;
    stats.m_sequence_double = sequence_double;
    stats.m_sequence_string = sequence_string;
    stats.m_map_string_uint64 = map_string_uint64;
    stats.m_map_string_double = map_string_double;
    for (const auto &attribute : attributes)
    {
        EXPECT_TRUE(attribute.hasValue());
    }
    EXPECT_EQ(*stats.m_bool, true);
    EXPECT_EQ(*stats.m_int32, static_cast<int32_t>(123));
    EXPECT_EQ(*stats.m_uint32, static_cast<uint32_t>(123));
    EXPECT_EQ(*stats.m_int64, static_cast<int64_t>(123));
    EXPECT_EQ(*stats.m_uint64, static_cast<uint64_t>(123));
    EXPECT_EQ(*stats.m_double, 123.0);
    EXPECT_EQ(*stats.m_string, std::string("123"));
    EXPECT_EQ(*stats.m_sequence_bool, sequence_bool);
    EXPECT_EQ(*stats.m_sequence_int32, sequence_int32);
    EXPECT_EQ(*stats.m_sequence_uint32, sequence_uint32);
    EXPECT_EQ(*stats.m_sequence_int64, sequence_int64);
    EXPECT_EQ(*stats.m_sequence_uint64, sequence_uint64);
    EXPECT_EQ(*stats.m_sequence_double, sequence_double);
    EXPECT_EQ(*stats.m_sequence_string, sequence_string);
    EXPECT_EQ(*stats.m_map_string_uint64, map_string_uint64);
    EXPECT_EQ(*stats.m_map_string_double, map_string_double);

    int32_t numbers[] = {4, 8, 15, 16, 23, 42};
    std::vector<int32_t> numbers_sequence(&numbers[0], &numbers[6]);
    stats.m_sequence_int32->clear();
    stats.m_sequence_int32->insert(stats.m_sequence_int32->end(), numbers_sequence.begin(), numbers_sequence.end());
    EXPECT_EQ(*stats.m_sequence_int32, numbers_sequence);
}

TEST(RtcStatsTest, EqualityOperator)
{
    RtcTestStats empty_stats("testId", Timestamp::Micros(123));
    EXPECT_EQ(empty_stats, empty_stats);

    RtcTestStats stats_with_all_values = empty_stats;
    stats_with_all_values.m_bool = true;
    stats_with_all_values.m_int32 = 123;
    stats_with_all_values.m_uint32 = 123;
    stats_with_all_values.m_int64 = 123;
    stats_with_all_values.m_uint64 = 123;
    stats_with_all_values.m_double = 123.0;
    stats_with_all_values.m_string = "123";
    stats_with_all_values.m_sequence_bool = std::vector<bool>();
    stats_with_all_values.m_sequence_int32 = std::vector<int32_t>();
    stats_with_all_values.m_sequence_uint32 = std::vector<uint32_t>();
    stats_with_all_values.m_sequence_int64 = std::vector<int64_t>();
    stats_with_all_values.m_sequence_uint64 = std::vector<uint64_t>();
    stats_with_all_values.m_sequence_double = std::vector<double>();
    stats_with_all_values.m_sequence_string = std::vector<std::string>();
    stats_with_all_values.m_map_string_uint64 = std::map<std::string, uint64_t>();
    stats_with_all_values.m_map_string_double = std::map<std::string, double>();
    EXPECT_NE(stats_with_all_values, empty_stats);
    EXPECT_EQ(stats_with_all_values, stats_with_all_values);
    EXPECT_NE(stats_with_all_values.attribute(stats_with_all_values.m_int32),
              stats_with_all_values.attribute(stats_with_all_values.m_uint32));

    RtcTestStats one_member_different[] = {
        stats_with_all_values,
        stats_with_all_values,
        stats_with_all_values,
        stats_with_all_values,
        stats_with_all_values,
        stats_with_all_values,
        stats_with_all_values,
        stats_with_all_values,
        stats_with_all_values,
        stats_with_all_values,
        stats_with_all_values,
        stats_with_all_values,
        stats_with_all_values,
        stats_with_all_values,
    };
    for (size_t i = 0; i < 14; ++i)
    {
        EXPECT_EQ(stats_with_all_values, one_member_different[i]);
    }
    one_member_different[0].m_bool = false;
    one_member_different[1].m_int32 = 321;
    one_member_different[2].m_uint32 = 321;
    one_member_different[3].m_int64 = 321;
    one_member_different[4].m_uint64 = 321;
    one_member_different[5].m_double = 321.0;
    one_member_different[6].m_string = "321";
    one_member_different[7].m_sequence_bool->push_back(false);
    one_member_different[8].m_sequence_int32->push_back(321);
    one_member_different[9].m_sequence_uint32->push_back(321);
    one_member_different[10].m_sequence_int64->push_back(321);
    one_member_different[11].m_sequence_uint64->push_back(321);
    one_member_different[12].m_sequence_double->push_back(321.0);
    one_member_different[13].m_sequence_string->push_back("321");
    (*one_member_different[13].m_map_string_uint64)["321"] = 321;
    (*one_member_different[13].m_map_string_double)["321"] = 321.0;
    for (size_t i = 0; i < 14; ++i)
    {
        EXPECT_NE(stats_with_all_values, one_member_different[i]);
    }

    RtcTestStats empty_stats_different_id("testId2", Timestamp::Micros(123));
    EXPECT_NE(empty_stats, empty_stats_different_id);
    RtcTestStats empty_stats_different_timestamp("testId", Timestamp::Micros(321));
    EXPECT_EQ(empty_stats, empty_stats_different_timestamp);

    RtcChildStats child("childId", Timestamp::Micros(42));
    RtcGrandChildStats grandchild("grandchildId", Timestamp::Micros(42));
    EXPECT_NE(child, grandchild);

    RtcChildStats stats_with_defined_member("leId", Timestamp::Micros(0));
    stats_with_defined_member.child_int = 0;
    RtcChildStats stats_with_undefined_member("leId", Timestamp::Micros(0));
    EXPECT_NE(stats_with_defined_member, stats_with_undefined_member);
    EXPECT_NE(stats_with_undefined_member, stats_with_defined_member);
}

TEST(RtcStatsTest, RtcStatsGrandChild)
{
    RtcGrandChildStats stats("grandchild", Timestamp::Micros(0.0));
    stats.child_int = 1;
    stats.grandchild_int = 2;
    int32_t sum = 0;
    for (const auto &attribute : stats.attributes())
    {
        sum += attribute.get<int32_t>();
    }
    EXPECT_EQ(sum, static_cast<int32_t>(3));

    std::unique_ptr<RtcStats> copy_ptr = stats.copy();
    const RtcGrandChildStats &copy = copy_ptr->cast_to<RtcGrandChildStats>();
    EXPECT_EQ(*copy.child_int, *stats.child_int);
    EXPECT_EQ(*copy.grandchild_int, *stats.grandchild_int);
}

TEST(RtcStatsTest, RtcStatsPrintsValidJson)
{
    std::string id = "statsId";
    int timestamp = 42;
    bool m_bool = true;
    int m_int32 = 123;
    int64_t m_int64 = 1234567890123456499L;
    double m_double = 123.4567890123456499;
    std::string m_string = "123";

    std::vector<bool> sequence_bool;
    std::vector<int32_t> sequence_int32;
    sequence_int32.push_back(static_cast<int32_t>(1));
    std::vector<int64_t> sequence_int64;
    sequence_int64.push_back(static_cast<int64_t>(-1234567890123456499L));
    sequence_int64.push_back(static_cast<int64_t>(1));
    sequence_int64.push_back(static_cast<int64_t>(1234567890123456499L));
    std::vector<double> sequence_double;
    sequence_double.push_back(123.4567890123456499);
    sequence_double.push_back(1234567890123.456499);
    std::vector<std::string> sequence_string;
    sequence_string.push_back(std::string("four"));

    std::map<std::string, uint64_t> map_string_uint64{{"long", static_cast<uint64_t>(1234567890123456499L)}};
    std::map<std::string, double> map_string_double{{"three", 123.4567890123456499},
                                                    {"thirteen", 123.4567890123456499}};

    RtcTestStats stats(id, Timestamp::Micros(timestamp));
    stats.m_bool = m_bool;
    stats.m_int32 = m_int32;
    stats.m_int64 = m_int64;
    stats.m_double = m_double;
    stats.m_string = m_string;
    stats.m_sequence_bool = sequence_bool;
    stats.m_sequence_int32 = sequence_int32;
    stats.m_sequence_int64 = sequence_int64;
    stats.m_sequence_double = sequence_double;
    stats.m_sequence_string = sequence_string;
    stats.m_map_string_uint64 = map_string_uint64;
    stats.m_map_string_double = map_string_double;
    std::string json_stats = stats.toJson();

    auto expected = utils::parseJson(json_stats);
    EXPECT_TRUE(expected.has_value());
    Json json = expected.value();
    EXPECT_TRUE(utils::readJsonValue(json, "id", &id));
    EXPECT_TRUE(utils::readJsonValue(json, "timestamp", &timestamp));
    EXPECT_TRUE(utils::readJsonValue(json, "mBool", &m_bool));
    EXPECT_TRUE(utils::readJsonValue(json, "mInt32", &m_int32));
    EXPECT_TRUE(utils::readJsonValue(json, "mDouble", &m_double));
    EXPECT_TRUE(utils::readJsonValue(json, "mString", &m_string));

    Json json_array;
    EXPECT_TRUE(utils::readJsonValue(json, "mSequenceBool", &json_array));
    EXPECT_TRUE(utils::parseJsonToVector(json_array, &sequence_bool));

    EXPECT_TRUE(utils::readJsonValue(json, "mSequenceInt32", &json_array));
    EXPECT_TRUE(utils::parseJsonToVector(json_array, &sequence_int32));

    EXPECT_TRUE(utils::readJsonValue(json, "mSequenceDouble", &json_array));
    EXPECT_TRUE(utils::parseJsonToVector(json_array, &sequence_double));

    EXPECT_TRUE(utils::readJsonValue(json, "mSequenceString", &json_array));
    EXPECT_TRUE(utils::parseJsonToVector(json_array, &sequence_string));

    Json json_map;
    EXPECT_TRUE(utils::readJsonValue(json, "mMapStringDouble", &json_map));
    for (const auto &entry : map_string_double)
    {
        double double_output = 0.0;
        EXPECT_TRUE(utils::readJsonValue(json_map, entry.first, &double_output));
        EXPECT_NEAR(double_output, entry.second, GetExpectedError(entry.second));
    }

    EXPECT_EQ(id, stats.id());
    EXPECT_EQ(timestamp, stats.timestamp().us());
    EXPECT_EQ(m_bool, *stats.m_bool);
    EXPECT_EQ(m_int32, *stats.m_int32);
    EXPECT_EQ(m_string, *stats.m_string);
    EXPECT_EQ(sequence_bool, *stats.m_sequence_bool);
    EXPECT_EQ(sequence_int32, *stats.m_sequence_int32);
    EXPECT_EQ(sequence_string, *stats.m_sequence_string);
    EXPECT_EQ(map_string_double, *stats.m_map_string_double);

    EXPECT_NEAR(m_double, *stats.m_double, GetExpectedError(*stats.m_double));

    EXPECT_EQ(sequence_double.size(), stats.m_sequence_double->size());
    for (size_t i = 0; i < stats.m_sequence_double->size(); ++i)
    {
        EXPECT_NEAR(sequence_double[i],
                    stats.m_sequence_double->at(i),
                    GetExpectedError(stats.m_sequence_double->at(i)));
    }

    EXPECT_EQ(map_string_double.size(), stats.m_map_string_double->size());
    for (const auto &entry : map_string_double)
    {
        auto it = stats.m_map_string_double->find(entry.first);
        EXPECT_NE(it, stats.m_map_string_double->end());
        EXPECT_NEAR(entry.second, it->second, GetExpectedError(it->second));
    }

    // We read mInt64 as double since JSON stores all numbers as doubles, so there
    // is not enough precision to represent large numbers.
    double m_int64_as_double;
    std::vector<double> sequence_int64_as_double;

    EXPECT_TRUE(utils::readJsonValue(json, "mInt64", &m_int64_as_double));

    EXPECT_TRUE(utils::readJsonValue(json, "mSequenceInt64", &json_array));
    EXPECT_TRUE(utils::parseJsonToVector(json_array, &sequence_int64_as_double));

    double stats_m_int64_as_double = static_cast<double>(*stats.m_int64);
    EXPECT_NEAR(m_int64_as_double, stats_m_int64_as_double, GetExpectedError(stats_m_int64_as_double));

    EXPECT_EQ(sequence_int64_as_double.size(), stats.m_sequence_int64->size());
    for (size_t i = 0; i < stats.m_sequence_int64->size(); ++i)
    {
        const double stats_value_as_double = static_cast<double>((*stats.m_sequence_int64)[i]);
        EXPECT_NEAR(sequence_int64_as_double[i], stats_value_as_double, GetExpectedError(stats_value_as_double));
    }

    // Similarly, read Uint64 as double
    EXPECT_TRUE(utils::readJsonValue(json, "mMapStringUint64", &json_map));
    for (const auto &entry : map_string_uint64)
    {
        const double stats_value_as_double = static_cast<double>((*stats.m_map_string_uint64)[entry.first]);
        double double_output = 0.0;
        EXPECT_TRUE(utils::readJsonValue(json_map, entry.first, &double_output));
        EXPECT_NEAR(double_output, stats_value_as_double, GetExpectedError(stats_value_as_double));
    }

    // Neither stats.m_uint32 nor stats.m_uint64 are defined, so "mUint64" and
    // "mUint32" should not be part of the generated JSON object.
    int m_uint32;
    int m_uint64;
    EXPECT_FALSE(stats.m_uint32.has_value());
    EXPECT_FALSE(stats.m_uint64.has_value());
    EXPECT_FALSE(utils::readJsonValue(json, "mUint32", &m_uint32));
    EXPECT_FALSE(utils::readJsonValue(json, "mUint64", &m_uint64));

    std::cout << stats.toJson() << std::endl;
}

TEST(RtcStatsTest, IsSequence)
{
    RtcTestStats stats("statsId", Timestamp::Micros(42));
    EXPECT_FALSE(stats.attribute(stats.m_bool).isSequence());
    EXPECT_FALSE(stats.attribute(stats.m_int32).isSequence());
    EXPECT_FALSE(stats.attribute(stats.m_uint32).isSequence());
    EXPECT_FALSE(stats.attribute(stats.m_int64).isSequence());
    EXPECT_FALSE(stats.attribute(stats.m_uint64).isSequence());
    EXPECT_FALSE(stats.attribute(stats.m_double).isSequence());
    EXPECT_FALSE(stats.attribute(stats.m_string).isSequence());
    EXPECT_TRUE(stats.attribute(stats.m_sequence_bool).isSequence());
    EXPECT_TRUE(stats.attribute(stats.m_sequence_int32).isSequence());
    EXPECT_TRUE(stats.attribute(stats.m_sequence_uint32).isSequence());
    EXPECT_TRUE(stats.attribute(stats.m_sequence_int64).isSequence());
    EXPECT_TRUE(stats.attribute(stats.m_sequence_uint64).isSequence());
    EXPECT_TRUE(stats.attribute(stats.m_sequence_double).isSequence());
    EXPECT_TRUE(stats.attribute(stats.m_sequence_string).isSequence());
    EXPECT_FALSE(stats.attribute(stats.m_map_string_uint64).isSequence());
    EXPECT_FALSE(stats.attribute(stats.m_map_string_double).isSequence());
}

TEST(RtcStatsTest, IsString)
{
    RtcTestStats stats("statsId", Timestamp::Micros(42));
    EXPECT_TRUE(stats.attribute(stats.m_string).isString());
    EXPECT_FALSE(stats.attribute(stats.m_bool).isString());
    EXPECT_FALSE(stats.attribute(stats.m_int32).isString());
    EXPECT_FALSE(stats.attribute(stats.m_uint32).isString());
    EXPECT_FALSE(stats.attribute(stats.m_int64).isString());
    EXPECT_FALSE(stats.attribute(stats.m_uint64).isString());
    EXPECT_FALSE(stats.attribute(stats.m_double).isString());
    EXPECT_FALSE(stats.attribute(stats.m_sequence_bool).isString());
    EXPECT_FALSE(stats.attribute(stats.m_sequence_int32).isString());
    EXPECT_FALSE(stats.attribute(stats.m_sequence_uint32).isString());
    EXPECT_FALSE(stats.attribute(stats.m_sequence_int64).isString());
    EXPECT_FALSE(stats.attribute(stats.m_sequence_uint64).isString());
    EXPECT_FALSE(stats.attribute(stats.m_sequence_double).isString());
    EXPECT_FALSE(stats.attribute(stats.m_sequence_string).isString());
    EXPECT_FALSE(stats.attribute(stats.m_map_string_uint64).isString());
    EXPECT_FALSE(stats.attribute(stats.m_map_string_double).isString());
}

TEST(RtcStatsTest, AttributeToString)
{
    RtcTestStats stats("statsId", Timestamp::Micros(42));
    stats.m_bool = true;
    EXPECT_EQ("true", stats.attribute(stats.m_bool).toString());

    stats.m_string = "foo";
    EXPECT_EQ("foo", stats.attribute(stats.m_string).toString());
    stats.m_int32 = -32;
    EXPECT_EQ("-32", stats.attribute(stats.m_int32).toString());
    stats.m_uint32 = 32;
    EXPECT_EQ("32", stats.attribute(stats.m_uint32).toString());
    stats.m_int64 = -64;
    EXPECT_EQ("-64", stats.attribute(stats.m_int64).toString());
    stats.m_uint64 = 64;
    EXPECT_EQ("64", stats.attribute(stats.m_uint64).toString());
    stats.m_double = 0.5;
    EXPECT_EQ("0.5", stats.attribute(stats.m_double).toString());
    stats.m_sequence_bool = {true, false};
    EXPECT_EQ("[true,false]", stats.attribute(stats.m_sequence_bool).toString());
    stats.m_sequence_int32 = {-32, 32};
    EXPECT_EQ("[-32,32]", stats.attribute(stats.m_sequence_int32).toString());
    stats.m_sequence_uint32 = {64, 32};
    EXPECT_EQ("[64,32]", stats.attribute(stats.m_sequence_uint32).toString());
    stats.m_sequence_int64 = {-64, 32};
    EXPECT_EQ("[-64,32]", stats.attribute(stats.m_sequence_int64).toString());
    stats.m_sequence_uint64 = {16, 32};
    EXPECT_EQ("[16,32]", stats.attribute(stats.m_sequence_uint64).toString());
    stats.m_sequence_double = {0.5, 0.25};
    EXPECT_EQ("[0.5,0.25]", stats.attribute(stats.m_sequence_double).toString());
    stats.m_sequence_string = {"foo", "bar"};
    EXPECT_EQ("[\"foo\",\"bar\"]", stats.attribute(stats.m_sequence_string).toString());
    stats.m_map_string_uint64 = std::map<std::string, uint64_t>();
    stats.m_map_string_uint64->emplace("foo", 32);
    stats.m_map_string_uint64->emplace("bar", 64);
    EXPECT_EQ("{\"bar\":64,\"foo\":32}", stats.attribute(stats.m_map_string_uint64).toString());
    stats.m_map_string_double = std::map<std::string, double>();
    stats.m_map_string_double->emplace("foo", 0.5);
    stats.m_map_string_double->emplace("bar", 0.25);
    EXPECT_EQ("{\"bar\":0.25,\"foo\":0.5}", stats.attribute(stats.m_map_string_double).toString());
}

// Death tests.
// Disabled on Android because death tests misbehave on Android, see base/test/gtest_util.h.
#if OCTK_DCHECK_IS_ON && GTEST_HAS_DEATH_TEST && !defined(OCTK_OS_ANDROID)

TEST(RtcStatsDeathTest, ValueOfUndefinedMember)
{
    RtcTestStats stats("testId", Timestamp::Micros(0));
    EXPECT_FALSE(stats.m_int32.has_value());
    EXPECT_DEATH(*stats.m_int32, "");
}

TEST(RtcStatsDeathTest, InvalidCasting)
{
    RtcGrandChildStats stats("grandchild", Timestamp::Micros(0.0));
    EXPECT_DEATH(stats.cast_to<RtcChildStats>(), "");
}

#endif // OCTK_DCHECK_IS_ON && GTEST_HAS_DEATH_TEST && !defined(OCTK_OS_ANDROID)
