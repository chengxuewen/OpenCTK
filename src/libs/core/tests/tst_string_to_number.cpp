/*
 *  Copyright 2017 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_string_to_number.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <cstdint>
#include <limits>
#include <string>

using namespace octk;

namespace
{
// clang-format off
using IntegerTypes = ::testing::Types<char,
                                      signed char, unsigned char,       // NOLINT(runtime/int)
                                      short, unsigned short,      // NOLINT(runtime/int)
                                      int, unsigned int,        // NOLINT(runtime/int)
                                      long, unsigned long,       // NOLINT(runtime/int)
                                      long long, unsigned long long,  // NOLINT(runtime/int)
                                      int8_t, uint8_t,
                                      int16_t, uint16_t,
                                      int32_t, uint32_t,
                                      int64_t, uint64_t>;
// clang-format on

template <typename T> class BasicNumberTest : public ::testing::Test
{
};

TYPED_TEST_SUITE_P(BasicNumberTest);

TYPED_TEST_P(BasicNumberTest, TestValidNumbers)
{
    using T = TypeParam;
    constexpr T min_value = std::numeric_limits<T>::lowest();
    constexpr T max_value = std::numeric_limits<T>::max();
    constexpr T zero_value = 0;
    const std::string min_string = std::to_string(min_value);
    const std::string max_string = std::to_string(max_value);
    EXPECT_EQ(min_value, utils::stringToNumber<T>(min_string));
    EXPECT_EQ(min_value, utils::stringToNumber<T>(min_string.c_str()));
    EXPECT_EQ(max_value, utils::stringToNumber<T>(max_string));
    EXPECT_EQ(max_value, utils::stringToNumber<T>(max_string.c_str()));
    EXPECT_EQ(zero_value, utils::stringToNumber<T>("0"));
    EXPECT_EQ(zero_value, utils::stringToNumber<T>("-0"));
    EXPECT_EQ(zero_value, utils::stringToNumber<T>(std::string("-0000000000000")));
}

TYPED_TEST_P(BasicNumberTest, TestInvalidNumbers)
{
    using T = TypeParam;
    // Value ranges aren't strictly enforced in this test, since that would either
    // require doctoring specific strings for each data type, which is a hassle
    // across platforms, or to be able to do addition of values larger than the
    // largest type, which is another hassle.
    constexpr T min_value = std::numeric_limits<T>::lowest();
    constexpr T max_value = std::numeric_limits<T>::max();
    // If the type supports negative values, make the large negative value
    // approximately ten times larger. If the type is unsigned, just use -2.
    const std::string too_low_string = (min_value == 0) ? "-2" : (std::to_string(min_value) + "1");
    // Make the large value approximately ten times larger than the maximum.
    const std::string too_large_string = std::to_string(max_value) + "1";
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(too_low_string));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(too_low_string.c_str()));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(too_large_string));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(too_large_string.c_str()));
}

TYPED_TEST_P(BasicNumberTest, TestInvalidInputs)
{
    using T = TypeParam;
    const char kInvalidCharArray[] = "Invalid string containing 47";
    const char kPlusMinusCharArray[] = "+-100";
    const char kNumberFollowedByCruft[] = "640x480";
    const char kEmbeddedNul[] = {'1', '2', '\0', '3', '4'};
    const char kBeginningEmbeddedNul[] = {'\0', '1', '2', '3', '4'};
    const char kTrailingEmbeddedNul[] = {'1', '2', '3', '4', '\0'};

    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(kInvalidCharArray));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(std::string(kInvalidCharArray)));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(kPlusMinusCharArray));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(std::string(kPlusMinusCharArray)));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(kNumberFollowedByCruft));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(std::string(kNumberFollowedByCruft)));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(" 5"));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(" - 5"));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>("- 5"));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(" -5"));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>("5 "));
    // Test various types of empty inputs
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>({nullptr, 0}));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(""));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(std::string()));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(std::string("")));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(StringView()));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(StringView(nullptr, 0)));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(StringView("")));
    // Test strings with embedded nuls.
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(StringView(kEmbeddedNul, sizeof(kEmbeddedNul))));
    EXPECT_EQ(utils::nullopt,
              utils::stringToNumber<T>(StringView(kBeginningEmbeddedNul, sizeof(kBeginningEmbeddedNul))));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<T>(StringView(kTrailingEmbeddedNul, sizeof(kTrailingEmbeddedNul))));
}

REGISTER_TYPED_TEST_SUITE_P(BasicNumberTest, TestValidNumbers, TestInvalidNumbers, TestInvalidInputs);
} // namespace

INSTANTIATE_TYPED_TEST_SUITE_P(StringToNumberTest_Integers, BasicNumberTest, IntegerTypes);

TEST(StringToNumberTest, TestSpecificValues)
{
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<uint8_t>("256"));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<uint8_t>("-256"));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<int8_t>("256"));
    EXPECT_EQ(utils::nullopt, utils::stringToNumber<int8_t>("-256"));
}
