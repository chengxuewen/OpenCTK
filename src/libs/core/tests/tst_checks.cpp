/*
 *  Copyright 2019 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_checks.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace
{

using ::testing::HasSubstr;
using ::testing::Not;

TEST(ChecksTest, ExpressionNotEvaluatedWhenCheckPassing)
{
    int i = 0;
    OCTK_CHECK(true) << "i=" << ++i;
    OCTK_CHECK_EQ(i, 0) << "Previous check passed, but i was incremented!";
}

#if GTEST_HAS_DEATH_TEST && !defined(OCTK_ANDROID)

TEST(ChecksDeathTest, Checks)
{
    return; //TODO
    EXPECT_DEATH(OCTK_FATAL() << "message",
                 "\n\n#\n"
                 "# Fatal error in: \\S+, line \\w+\n"
                 "# last system error: \\w+\n"
                 "# Check failed: FATAL\\(\\)\n"
                 "# message");

    int a = 1, b = 2;
    EXPECT_DEATH(OCTK_CHECK_EQ(a, b) << 1 << 2u,
                 "\n\n#\n"
                 "# Fatal error in: \\S+, line \\w+\n"
                 "# last system error: \\w+\n"
                 "# Check failed: a == b \\(1 vs. 2\\)\n"
                 "# 12");
    OCTK_CHECK_EQ(5, 5);

    OCTK_CHECK(true) << "Shouldn't crash" << 1;
    EXPECT_DEATH(OCTK_CHECK(false) << "Hi there!",
                 "\n\n#\n"
                 "# Fatal error in: \\S+, line \\w+\n"
                 "# last system error: \\w+\n"
                 "# Check failed: false\n"
                 "# Hi there!");

//    StructWithStringfy t;
//    EXPECT_DEATH(OCTK_CHECK(false) << t, HasSubstr("absl-stringify"));
}
#endif  // GTEST_HAS_DEATH_TEST && !defined(OCTK_ANDROID)

}  // namespace
