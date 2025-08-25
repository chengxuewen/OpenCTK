/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_exception.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <utility>

using namespace octk;

namespace
{
}  // namespace

#if OCTK_HAS_EXCEPTIONS
#   define EXPECT_THROW_OR_DEATH(statement, expected_exception) EXPECT_THROW(statement, expected_exception)
#else
#   define EXPECT_THROW_OR_DEATH(statement, expected_exception) EXPECT_DEATH(statement, #expected_exception)
#endif

TEST(BufferTest, Test_std_logic_error)
{
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::logic_error, ""), std::logic_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::logic_error, "std::logic_error:d", 1), std::logic_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::logic_error, StringView("std::logic_error")), std::logic_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::logic_error, std::string("std::logic_error")), std::logic_error);

    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_LOGIC_ERROR(""), std::logic_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_LOGIC_ERROR("std::logic_error:d", 1), std::logic_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_LOGIC_ERROR(StringView("std::logic_error")), std::logic_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_LOGIC_ERROR(std::string("std::logic_error")), std::logic_error);
}

TEST(BufferTest, Test_std_invalid_argument)
{
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::invalid_argument, ""), std::invalid_argument);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::invalid_argument, "std::invalid_argument:d", 1), std::invalid_argument);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::invalid_argument, StringView("std::invalid_argument")),
                          std::invalid_argument);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::invalid_argument, std::string("std::invalid_argument")),
                          std::invalid_argument);

    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_INVALID_ARGUMENT(""), std::invalid_argument);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_INVALID_ARGUMENT("std::invalid_argument:d", 1), std::invalid_argument);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_INVALID_ARGUMENT(StringView("std::invalid_argument")), std::invalid_argument);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_INVALID_ARGUMENT(std::string("std::invalid_argument")), std::invalid_argument);
}

TEST(BufferTest, Test_std_domain_error)
{
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::domain_error, ""), std::domain_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::domain_error, "std::domain_error:d", 1), std::domain_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::domain_error, StringView("std::domain_error")), std::domain_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::domain_error, std::string("std::domain_error")), std::domain_error);

    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_DOMAIN_ERROR(""), std::domain_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_DOMAIN_ERROR("std::domain_error:d", 1), std::domain_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_DOMAIN_ERROR(StringView("std::domain_error")), std::domain_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_DOMAIN_ERROR(std::string("std::domain_error")), std::domain_error);
}

TEST(BufferTest, Test_std_length_error)
{
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::length_error, ""), std::length_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::length_error, "std::length_error:d", 1), std::length_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::length_error, StringView("std::length_error")), std::length_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::length_error, std::string("std::length_error")), std::length_error);

    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_LENGTH_ERROR(""), std::length_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_LENGTH_ERROR("std::length_error:d", 1), std::length_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_LENGTH_ERROR(StringView("std::length_error")), std::length_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_LENGTH_ERROR(std::string("std::length_error")), std::length_error);
}

TEST(BufferTest, Test_std_out_of_range)
{
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::out_of_range, ""), std::out_of_range);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::out_of_range, "std::out_of_range:d", 1), std::out_of_range);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::out_of_range, StringView("std::out_of_range")), std::out_of_range);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::out_of_range, std::string("std::out_of_range")), std::out_of_range);

    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_OUT_OF_RANGE(""), std::out_of_range);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_OUT_OF_RANGE("std::out_of_range:d", 1), std::out_of_range);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_OUT_OF_RANGE(StringView("std::out_of_range")), std::out_of_range);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_OUT_OF_RANGE(std::string("std::out_of_range")), std::out_of_range);
}

TEST(BufferTest, Test_std_runtime_error)
{
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::runtime_error, ""), std::runtime_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::runtime_error, "std::runtime_error:d", 1), std::runtime_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::runtime_error, StringView("std::runtime_error")), std::runtime_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::runtime_error, std::string("std::runtime_error")), std::runtime_error);

    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_RUNTIME_ERROR(""), std::runtime_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_RUNTIME_ERROR("std::runtime_error:d", 1), std::runtime_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_RUNTIME_ERROR(StringView("std::runtime_error")), std::runtime_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_RUNTIME_ERROR(std::string("std::runtime_error")), std::runtime_error);
}

TEST(BufferTest, Test_std_range_error)
{
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::range_error, ""), std::range_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::range_error, "std::range_error:d", 1), std::range_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::range_error, StringView("std::range_error")), std::range_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::range_error, std::string("std::range_error")), std::range_error);

    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_RANGE_ERROR(""), std::range_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_RANGE_ERROR("std::range_error:d", 1), std::range_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_RANGE_ERROR(StringView("std::range_error")), std::range_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_RANGE_ERROR(std::string("std::range_error")), std::range_error);
}

TEST(BufferTest, Test_std_overflow_error)
{
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::overflow_error, ""), std::overflow_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::overflow_error, "std::overflow_error:d", 1), std::overflow_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::overflow_error, StringView("std::overflow_error")), std::overflow_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::overflow_error, std::string("std::overflow_error")), std::overflow_error);

    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_OVERFLOW_ERROR(""), std::overflow_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_OVERFLOW_ERROR("std::overflow_error:d", 1), std::overflow_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_OVERFLOW_ERROR(StringView("std::overflow_error")), std::overflow_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_OVERFLOW_ERROR(std::string("std::overflow_error")), std::overflow_error);
}

TEST(BufferTest, Test_std_underflow_error)
{
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::underflow_error, ""), std::underflow_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::underflow_error, "std::underflow_error:d", 1), std::underflow_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::underflow_error, StringView("std::underflow_error")), std::underflow_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW(std::underflow_error, std::string("std::underflow_error")), std::underflow_error);

    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_UNDERFLOW_ERROR(""), std::underflow_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_UNDERFLOW_ERROR("std::underflow_error:d", 1), std::underflow_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_UNDERFLOW_ERROR(StringView("std::underflow_error")), std::underflow_error);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_UNDERFLOW_ERROR(std::string("std::underflow_error")), std::underflow_error);
}

TEST(BufferTest, Test_std_bad_function_call)
{
    EXPECT_THROW_OR_DEATH(OCTK_THROW_NO_MSG(std::bad_function_call), std::bad_function_call);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_BAD_FUNCTION_CALL(), std::bad_function_call);
}

TEST(BufferTest, Test_std_bad_alloc)
{
    EXPECT_THROW_OR_DEATH(OCTK_THROW_NO_MSG(std::bad_alloc), std::bad_alloc);
    EXPECT_THROW_OR_DEATH(OCTK_THROW_STD_BAD_ALLOC(), std::bad_alloc);
}