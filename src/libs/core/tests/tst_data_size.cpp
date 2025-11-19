/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_data_size.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <limits>

using namespace octk;

namespace test
{

TEST(DataSizeTest, ConstExpr)
{
    constexpr int64_t kValue = 12345;
    constexpr DataSize kDataSizeZero = DataSize::Zero();
    constexpr DataSize kDataSizeInf = DataSize::Infinity();
    OCTK_STATIC_ASSERT(DataSize() == kDataSizeZero);
    OCTK_STATIC_ASSERT(kDataSizeZero.IsZero());
    OCTK_STATIC_ASSERT(kDataSizeInf.IsInfinite());
    OCTK_STATIC_ASSERT(kDataSizeInf.bytes_or(-1) == -1);
    OCTK_STATIC_ASSERT(kDataSizeInf > kDataSizeZero);

    OCTK_CXX14_CONSTEXPR DataSize kDataSize = DataSize::Bytes(kValue);
    OCTK_CXX14_CONSTEXPR_ASSERT(kDataSize.bytes_or(-1) == kValue);

    EXPECT_EQ(kDataSize.bytes(), kValue);
}

TEST(DataSizeTest, GetBackSameValues)
{
    const int64_t kValue = 123 * 8;
    EXPECT_EQ(DataSize::Bytes(kValue).bytes(), kValue);
}

TEST(DataSizeTest, IdentityChecks)
{
    const int64_t kValue = 3000;
    EXPECT_TRUE(DataSize::Zero().IsZero());
    EXPECT_FALSE(DataSize::Bytes(kValue).IsZero());

    EXPECT_TRUE(DataSize::Infinity().IsInfinite());
    EXPECT_FALSE(DataSize::Zero().IsInfinite());
    EXPECT_FALSE(DataSize::Bytes(kValue).IsInfinite());

    EXPECT_FALSE(DataSize::Infinity().IsFinite());
    EXPECT_TRUE(DataSize::Bytes(kValue).IsFinite());
    EXPECT_TRUE(DataSize::Zero().IsFinite());
}

TEST(DataSizeTest, ComparisonOperators)
{
    const int64_t kSmall = 450;
    const int64_t kLarge = 451;
    const DataSize smallDataSize = DataSize::Bytes(kSmall);
    const DataSize largeDataSize = DataSize::Bytes(kLarge);

    EXPECT_EQ(DataSize::Zero(), DataSize::Bytes(0));
    EXPECT_EQ(DataSize::Infinity(), DataSize::Infinity());
    EXPECT_EQ(smallDataSize, smallDataSize);
    EXPECT_LE(smallDataSize, smallDataSize);
    EXPECT_GE(smallDataSize, smallDataSize);
    EXPECT_NE(smallDataSize, largeDataSize);
    EXPECT_LE(smallDataSize, largeDataSize);
    EXPECT_LT(smallDataSize, largeDataSize);
    EXPECT_GE(largeDataSize, smallDataSize);
    EXPECT_GT(largeDataSize, smallDataSize);
    EXPECT_LT(DataSize::Zero(), smallDataSize);
    EXPECT_GT(DataSize::Infinity(), largeDataSize);
}

TEST(DataSizeTest, ConvertsToAndFromDouble)
{
    const int64_t kValue = 128;
    const double kDoubleValue = static_cast<double>(kValue);

    EXPECT_EQ(DataSize::Bytes(kValue).bytes<double>(), kDoubleValue);
    EXPECT_EQ(DataSize::Bytes(kDoubleValue).bytes(), kValue);

    const double kInfinity = std::numeric_limits<double>::infinity();
    EXPECT_EQ(DataSize::Infinity().bytes<double>(), kInfinity);
    EXPECT_TRUE(DataSize::Bytes(kInfinity).IsInfinite());
}

TEST(DataSizeTest, MathOperations)
{
    const int64_t kValueA = 450;
    const int64_t kValueB = 267;
    const DataSize size_a = DataSize::Bytes(kValueA);
    const DataSize size_b = DataSize::Bytes(kValueB);
    EXPECT_EQ((size_a + size_b).bytes(), kValueA + kValueB);
    EXPECT_EQ((size_a - size_b).bytes(), kValueA - kValueB);

    const int32_t kInt32Value = 123;
    const double kFloatValue = 123.0;
    EXPECT_EQ((size_a * kValueB).bytes(), kValueA * kValueB);
    EXPECT_EQ((size_a * kInt32Value).bytes(), kValueA * kInt32Value);
    EXPECT_EQ((size_a * kFloatValue).bytes(), kValueA * kFloatValue);

    EXPECT_EQ((size_a / 10).bytes(), kValueA / 10);
    EXPECT_EQ(size_a / size_b, static_cast<double>(kValueA) / kValueB);

    DataSize mutable_size = DataSize::Bytes(kValueA);
    mutable_size += size_b;
    EXPECT_EQ(mutable_size.bytes(), kValueA + kValueB);
    mutable_size -= size_a;
    EXPECT_EQ(mutable_size.bytes(), kValueB);
}
}  // namespace test
