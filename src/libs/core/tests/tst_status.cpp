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

#include <octk_status.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

OCTK_BEGIN_NAMESPACE

namespace
{
class TestDomain : public Error::Domain
{
public:
    enum Code
    {
        kTestError1 = 100,
        kTestError2 = 200,
        kTestError3 = 300
    };

    using Error::Domain::Domain;
    TestDomain() = default;

    StringView codeString(Id code) const override
    {
        switch (code)
        {
            case kTestError1: return "Test error 1";
            case kTestError2: return "Test error 2";
            case kTestError3: return "Test error 3";
            default: return "";
        }
    }
};
OCTK_DEFINE_ERROR_DOMAIN(TestDomain, testDomain, "Test domain")

class AnotherDomain : public Error::Domain
{
public:
    enum Code
    {
        kAnotherError = 400
    };

    using Error::Domain::Domain;
    AnotherDomain() = default;

    StringView codeString(Id code) const override
    {
        switch (code)
        {
            case kAnotherError: return "Another error";
            default: return "";
        }
    }
};
OCTK_DEFINE_ERROR_DOMAIN(AnotherDomain, anotherDomain, "Another domain")
} // namespace

TEST(StatusTest, DefaultConstructor)
{
    Status status;

    EXPECT_TRUE(status.ok());
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_EQ(status.error(), nullptr);
    EXPECT_TRUE(status.errorString().empty());
}

TEST(StatusTest, ConstructFromError)
{
    auto domain = testDomain();

    // no cause
    const auto error = Error::create(domain, TestDomain::kTestError1, "Test error");
    Status status(error);

    EXPECT_FALSE(status.ok());
    EXPECT_FALSE(static_cast<bool>(status));
    EXPECT_EQ(status.error(), error.data());
    EXPECT_EQ(&status.error()->domain(), &domain);
    EXPECT_EQ(status.error()->code(), TestDomain::kTestError1);
    EXPECT_EQ(status.error()->message(), "Test error");

    // with cause
    auto cause = Error::create(domain, TestDomain::kTestError2, "Cause");
    const auto error2 = Error::create(domain, TestDomain::kTestError1, "Test error", cause);
    Status status2(error2);
    EXPECT_FALSE(status2.ok());
    EXPECT_FALSE(static_cast<bool>(status2));
    EXPECT_EQ(status2.error(), error2.data());
    EXPECT_EQ(&status2.error()->domain(), &domain);
    EXPECT_EQ(status2.error()->code(), TestDomain::kTestError1);
    EXPECT_EQ(status2.error()->message(), "Test error");
    EXPECT_EQ(status2.error()->depth(), 1);
}

TEST(StatusTest, ConstructWithParameters)
{
    auto domain = testDomain();

    // message
    Status status("Direct construction");
    EXPECT_FALSE(status.ok());
    EXPECT_STREQ(status.errorString().c_str(), "Direct construction");

    // no cause
    Status status1(domain, TestDomain::kTestError1, "Direct construction");
    EXPECT_FALSE(status1.ok());
    EXPECT_EQ(status1.error()->code(), TestDomain::kTestError1);

    // with cause
    auto cause = Error::create(domain, TestDomain::kTestError2, "Cause");
    Status status2(domain, TestDomain::kTestError3, "With cause", cause);
    EXPECT_FALSE(status2.ok());
    EXPECT_EQ(status2.error()->code(), TestDomain::kTestError3);
    EXPECT_NE(status2.error()->cause(), nullptr);
}

TEST(StatusTest, CopyConstructor)
{
    auto domain = testDomain();
    Status status1(domain, TestDomain::kTestError1, "Original");
    Status status2 = status1;

    EXPECT_EQ(status1.ok(), status2.ok());
    EXPECT_EQ(status1.error()->code(), status2.error()->code());
    EXPECT_EQ(status1.error()->message(), status2.error()->message());
    EXPECT_EQ(status1.error(), status2.error());
}

TEST(StatusTest, MoveConstructor)
{
    auto domain = testDomain();
    Status status1(domain, TestDomain::kTestError1, "To be moved");

    const Error *original_error = status1.error();

    // move constructor
    Status status2 = std::move(status1);

    EXPECT_FALSE(status2.ok());
    EXPECT_EQ(status2.error(), original_error);
    EXPECT_EQ(status2.error()->message(), "To be moved");

    EXPECT_TRUE(status1.ok() || status1.error() == nullptr);
}

TEST(StatusTest, CopyAssignment)
{
    auto domain = testDomain();
    Status status1(domain, TestDomain::kTestError1, "Source");
    Status status2;

    EXPECT_TRUE(status2.ok());

    status2 = status1;
    EXPECT_FALSE(status2.ok());
    EXPECT_EQ(status1.error()->code(), status2.error()->code());
    EXPECT_EQ(status1.error()->message(), status2.error()->message());
}

TEST(StatusTest, MoveAssignment)
{
    auto domain = testDomain();
    Status status1(domain, TestDomain::kTestError1, "Source");
    Status status2;

    const Error *original_error = status1.error();

    status2 = std::move(status1);

    EXPECT_FALSE(status2.ok());
    EXPECT_EQ(status2.error(), original_error);
    EXPECT_TRUE(status1.ok());
}

TEST(StatusTest, ArrowOperator)
{
    auto domain = testDomain();
    Status status(domain, TestDomain::kTestError1, "Test message");

    EXPECT_EQ(status.error()->code(), TestDomain::kTestError1);
    EXPECT_EQ(status.error()->message(), "Test message");

    Status ok_status;
    EXPECT_EQ(ok_status.error(), nullptr);
}

TEST(StatusTest, ToString)
{
    // ok status
    Status ok_status;
    EXPECT_EQ(ok_status.errorString(), "");

    // error status
    auto domain = testDomain();
    Status error_status(domain, TestDomain::kTestError1, "Failed operation");

    std::string result = error_status.errorString();
    EXPECT_NE(result.find("TestDomain"), std::string::npos);
    EXPECT_NE(result.find("Failed operation"), std::string::npos);
}

TEST(StatusTest, StreamOutput)
{
    auto domain = testDomain();
    Status status(domain, TestDomain::kTestError1, "Stream test");

    std::ostringstream oss;
    oss << status;

    std::string result = oss.str();
    EXPECT_NE(result.find("Stream test"), std::string::npos);
}

TEST(StatusTest, Comparison)
{
    auto domain = testDomain();

    Status ok1;
    Status ok2;
    Status error1(domain, TestDomain::kTestError1, "Error 1");
    Status error2(domain, TestDomain::kTestError1, "Error 1");
    Status error3(domain, TestDomain::kTestError2, "Error 2");

    // ok status comparison
    EXPECT_EQ(ok1, ok2);

    // error status comparison
    if (error1.error() && error2.error())
    {
        EXPECT_EQ(error1.error()->code(), error2.error()->code());
    }

    EXPECT_NE(error1, error3);
    EXPECT_NE(ok1, error1);
}

TEST(IntegrationTest, ErrorChainPropagation)
{
    auto domain = testDomain();

    auto root = Error::create(domain, TestDomain::kTestError1, "Root error");
    auto middle = Error::create(domain, TestDomain::kTestError2, "Middle error", root);
    auto top = Error::create(domain, TestDomain::kTestError3, "Top error", middle);

    Status status(top);

    ASSERT_FALSE(status.ok());
    EXPECT_EQ(status.error()->code(), TestDomain::kTestError3);

    const Error *cause = status.error()->cause();
    ASSERT_NE(cause, nullptr);
    EXPECT_EQ(cause->code(), TestDomain::kTestError2);

    cause = cause->cause();
    ASSERT_NE(cause, nullptr);
    EXPECT_EQ(cause->code(), TestDomain::kTestError1);
}

TEST(IntegrationTest, DomainSpecificFormatting)
{
    auto domain = testDomain();

    Status status(domain, TestDomain::kTestError1, "Custom message");

    std::string formatted = status.errorString();
    EXPECT_NE(formatted.find("TestDomain"), std::string::npos);
    EXPECT_NE(formatted.find("Custom message"), std::string::npos);
}

TEST(BoundaryTest, ZeroCode)
{
    auto domain = testDomain();
    Status status(domain, 0, "Code zero error");

    EXPECT_FALSE(status.ok());
    EXPECT_EQ(status.error()->code(), 0);
}

TEST(BoundaryTest, NegativeCode)
{
    auto domain = testDomain();
    Status status(domain, -1, "Negative code");

    EXPECT_FALSE(status.ok());
    EXPECT_EQ(status.error()->code(), -1);
}

TEST(PerformanceTest, CreateManyStatus)
{
    auto domain = testDomain();

    const int kIterations = 1000;
    for (int i = 0; i < kIterations; ++i)
    {
        Status status(domain, i % 100, "Message " + std::to_string(i));
        EXPECT_FALSE(status.ok());
    }
}

TEST(PerformanceTest, CopyPerformance)
{
    auto domain = testDomain();
    Status original(domain, TestDomain::kTestError1, "Original");

    const int kCopies = 10000;
    std::vector<Status> statuses;
    statuses.reserve(kCopies);

    for (int i = 0; i < kCopies; ++i)
    {
        statuses.push_back(original);
    }

    for (const auto &status : statuses)
    {
        EXPECT_EQ(status.error(), original.error());
    }
}

OCTK_END_NAMESPACE
