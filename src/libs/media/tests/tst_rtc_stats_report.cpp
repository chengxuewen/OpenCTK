/*
*  Copyright 2016 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#include <octk_rtc_stats_report.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>

using namespace octk;

class RtcTestStats1 : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();

    RtcTestStats1(const std::string &id, Timestamp timestamp)
        : RtcStats(id, timestamp)
    {
    }

    Optional<int32_t> integer;
};

OCTK_IMPLEMENT_RTCSTATS(RtcTestStats1, RtcStats, "test-stats-1", AttributeInit("integer", &integer))

class RtcTestStats2 : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();

    RtcTestStats2(const std::string &id, Timestamp timestamp)
        : RtcStats(id, timestamp)
    {
    }

    Optional<double> number;
};

OCTK_IMPLEMENT_RTCSTATS(RtcTestStats2, RtcStats, "test-stats-2", AttributeInit("number", &number))

class RtcTestStats3 : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();

    RtcTestStats3(const std::string &id, Timestamp timestamp)
        : RtcStats(id, timestamp)
    {
    }

    Optional<std::string> string;
};

OCTK_IMPLEMENT_RTCSTATS(RtcTestStats3, RtcStats, "test-stats-3", AttributeInit("string", &string))

TEST(RtcStatsReport, AddAndgetStats)
{
    SharedRefPtr<RtcStatsReport> report = RtcStatsReport::create(Timestamp::Micros(1337));
    EXPECT_EQ(report->timestamp().us_or(-1), 1337u);
    EXPECT_EQ(report->size(), static_cast<size_t>(0));
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats1("a0", Timestamp::Micros(1))));
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats1("a1", Timestamp::Micros(2))));
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats2("b0", Timestamp::Micros(4))));
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats2("b1", Timestamp::Micros(8))));
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats1("a2", Timestamp::Micros(16))));
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats2("b2", Timestamp::Micros(32))));
    EXPECT_EQ(report->size(), static_cast<size_t>(6));

    EXPECT_EQ(report->get("missing"), nullptr);
    EXPECT_EQ(report->get("a0")->id(), "a0");
    EXPECT_EQ(report->get("b2")->id(), "b2");

    std::vector<const RtcTestStats1 *> a = report->getStatsOfType<RtcTestStats1>();
    EXPECT_EQ(a.size(), static_cast<size_t>(3));
    int64_t mask = 0;
    for (const RtcTestStats1 *stats : a)
    {
        mask |= stats->timestamp().us();
    }
    EXPECT_EQ(mask, static_cast<int64_t>(1 | 2 | 16));

    std::vector<const RtcTestStats2 *> b = report->getStatsOfType<RtcTestStats2>();
    EXPECT_EQ(b.size(), static_cast<size_t>(3));
    mask = 0;
    for (const RtcTestStats2 *stats : b)
    {
        mask |= stats->timestamp().us();
    }
    EXPECT_EQ(mask, static_cast<int64_t>(4 | 8 | 32));

    EXPECT_EQ(report->getStatsOfType<RtcTestStats3>().size(), static_cast<size_t>(0));
}

TEST(RtcStatsReport, StatsOrder)
{
    SharedRefPtr<RtcStatsReport> report = RtcStatsReport::create(Timestamp::Micros(1337));
    EXPECT_EQ(report->timestamp().us(), 1337u);
    EXPECT_EQ(report->timestamp().us_or(-1), 1337u);
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats1("C", Timestamp::Micros(2))));
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats1("D", Timestamp::Micros(3))));
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats2("B", Timestamp::Micros(1))));
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats2("A", Timestamp::Micros(0))));
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats2("E", Timestamp::Micros(4))));
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats2("F", Timestamp::Micros(5))));
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats2("G", Timestamp::Micros(6))));
    int64_t i = 0;
    for (const RtcStats &stats : *report)
    {
        EXPECT_EQ(stats.timestamp().us(), i);
        ++i;
    }
    EXPECT_EQ(i, static_cast<int64_t>(7));
}

TEST(RtcStatsReport, take)
{
    SharedRefPtr<RtcStatsReport> report = RtcStatsReport::create(Timestamp::Zero());
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats1("A", Timestamp::Micros(1))));
    report->addStats(std::unique_ptr<RtcStats>(new RtcTestStats1("B", Timestamp::Micros(2))));
    EXPECT_TRUE(report->get("A"));
    EXPECT_EQ(report->size(), 2u);
    auto a = report->take("A");
    EXPECT_TRUE(a);
    EXPECT_EQ(report->size(), 1u);
    EXPECT_FALSE(report->get("A"));
    EXPECT_FALSE(report->take("A"));
}

TEST(RtcStatsReport, takeMembersFrom)
{
    SharedRefPtr<RtcStatsReport> a = RtcStatsReport::create(Timestamp::Micros(1337));
    EXPECT_EQ(a->timestamp().us_or(-1), 1337u);
    a->addStats(std::unique_ptr<RtcStats>(new RtcTestStats1("B", Timestamp::Micros(1))));
    a->addStats(std::unique_ptr<RtcStats>(new RtcTestStats1("C", Timestamp::Micros(2))));
    a->addStats(std::unique_ptr<RtcStats>(new RtcTestStats1("E", Timestamp::Micros(4))));
    SharedRefPtr<RtcStatsReport> b = RtcStatsReport::create(Timestamp::Micros(1338));
    EXPECT_EQ(b->timestamp().us_or(-1), 1338u);
    b->addStats(std::unique_ptr<RtcStats>(new RtcTestStats1("A", Timestamp::Micros(0))));
    b->addStats(std::unique_ptr<RtcStats>(new RtcTestStats1("D", Timestamp::Micros(3))));
    b->addStats(std::unique_ptr<RtcStats>(new RtcTestStats1("F", Timestamp::Micros(5))));

    a->takeMembersFrom(b);
    EXPECT_EQ(b->size(), static_cast<size_t>(0));
    int64_t i = 0;
    for (const RtcStats &stats : *a)
    {
        EXPECT_EQ(stats.timestamp().us(), i);
        ++i;
    }
    EXPECT_EQ(i, static_cast<int64_t>(6));
}
