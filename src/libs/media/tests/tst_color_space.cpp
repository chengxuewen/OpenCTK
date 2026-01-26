/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_color_space.hpp>

#include <cstdint>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

OCTK_BEGIN_NAMESPACE

TEST(ColorSpace, TestSettingPrimariesFromUint8)
{
    ColorSpace colorSpace;
    EXPECT_TRUE(colorSpace.set_primaries_from_uint8(static_cast<uint8_t>(ColorSpace::PrimaryID::kBT470BG)));
    EXPECT_EQ(ColorSpace::PrimaryID::kBT470BG, colorSpace.primaries());
    EXPECT_FALSE(colorSpace.set_primaries_from_uint8(3));
    EXPECT_FALSE(colorSpace.set_primaries_from_uint8(23));
    EXPECT_FALSE(colorSpace.set_primaries_from_uint8(64));
}

TEST(ColorSpace, TestSettingTransferFromUint8)
{
    ColorSpace colorSpace;
    EXPECT_TRUE(colorSpace.set_transfer_from_uint8(static_cast<uint8_t>(ColorSpace::TransferID::kBT2020_10)));
    EXPECT_EQ(ColorSpace::TransferID::kBT2020_10, colorSpace.transfer());
    EXPECT_FALSE(colorSpace.set_transfer_from_uint8(3));
    EXPECT_FALSE(colorSpace.set_transfer_from_uint8(19));
    EXPECT_FALSE(colorSpace.set_transfer_from_uint8(128));
}

TEST(ColorSpace, TestSettingMatrixFromUint8)
{
    ColorSpace colorSpace;
    EXPECT_TRUE(colorSpace.set_matrix_from_uint8(static_cast<uint8_t>(ColorSpace::MatrixID::kCDNCLS)));
    EXPECT_EQ(ColorSpace::MatrixID::kCDNCLS, colorSpace.matrix());
    EXPECT_FALSE(colorSpace.set_matrix_from_uint8(3));
    EXPECT_FALSE(colorSpace.set_matrix_from_uint8(15));
    EXPECT_FALSE(colorSpace.set_matrix_from_uint8(255));
}

TEST(ColorSpace, TestSettingRangeFromUint8)
{
    ColorSpace colorSpace;
    EXPECT_TRUE(colorSpace.set_range_from_uint8(static_cast<uint8_t>(ColorSpace::RangeID::kFull)));
    EXPECT_EQ(ColorSpace::RangeID::kFull, colorSpace.range());
    EXPECT_FALSE(colorSpace.set_range_from_uint8(4));
}

TEST(ColorSpace, TestSettingChromaSitingHorizontalFromUint8)
{
    ColorSpace colorSpace;
    EXPECT_TRUE(colorSpace.set_chroma_siting_horizontal_from_uint8(
        static_cast<uint8_t>(ColorSpace::ChromaSiting::kCollocated)));
    EXPECT_EQ(ColorSpace::ChromaSiting::kCollocated, colorSpace.chroma_siting_horizontal());
    EXPECT_FALSE(colorSpace.set_chroma_siting_horizontal_from_uint8(3));
}

TEST(ColorSpace, TestSettingChromaSitingVerticalFromUint8)
{
    ColorSpace colorSpace;
    EXPECT_TRUE(
        colorSpace.set_chroma_siting_vertical_from_uint8(static_cast<uint8_t>(ColorSpace::ChromaSiting::kHalf)));
    EXPECT_EQ(ColorSpace::ChromaSiting::kHalf, colorSpace.chroma_siting_vertical());
    EXPECT_FALSE(colorSpace.set_chroma_siting_vertical_from_uint8(3));
}

TEST(ColorSpace, TestAsStringFunction)
{
    ColorSpace colorSpace(ColorSpace::PrimaryID::kBT709,
                          ColorSpace::TransferID::kBT709,
                          ColorSpace::MatrixID::kBT709,
                          ColorSpace::RangeID::kLimited);
    EXPECT_EQ(colorSpace.AsString(), "{primaries:kBT709, transfer:kBT709, matrix:kBT709, range:kLimited}");
}

OCTK_END_NAMESPACE