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

#include <octk_error.hpp>

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

TEST(ErrorTest, CreateSuccess)
{
    auto error = Error::create(testDomain(), TestDomain::kTestError1, "Test message");

    ASSERT_NE(error, nullptr);
    EXPECT_EQ(&error->domain(), &testDomain());
    EXPECT_EQ(error->code(), TestDomain::kTestError1);
    EXPECT_EQ(error->message(), "Test message");
    EXPECT_EQ(error->cause(), nullptr);
}

TEST(ErrorTest, CreateWithDefaultDomain)
{
    const auto error = Error::create("Default domain Test");
    auto domain = error->domain();
    const auto errorShare = error;
    EXPECT_EQ(error->refCount(), 2);
    EXPECT_EQ(errorShare->refCount(), 2);
    auto errorCopy = error;
    EXPECT_EQ(errorCopy->refCount(), 1);
    EXPECT_TRUE(error->domain().id() == Error::kInvalidId);
}

TEST(ErrorTest, CreateWithEmptyMessage)
{
    auto error = Error::create(testDomain(), TestDomain::kTestError2, "");

    ASSERT_NE(error, nullptr);
    EXPECT_TRUE(error->message().empty());
}

TEST(ErrorTest, CreateWithCause)
{
    auto cause = Error::create(testDomain(), TestDomain::kTestError1, "Root cause");
    auto error = Error::create(testDomain(), TestDomain::kTestError2, "Wrapper error", cause);

    EXPECT_NE(error->cause(), nullptr);
    EXPECT_EQ(error->cause()->code(), TestDomain::kTestError1);
    EXPECT_EQ(error->cause()->message(), "Root cause");
}

TEST(ErrorTest, ToString)
{
    auto domain = testDomain();
    auto error = Error::create(domain, TestDomain::kTestError1, "Something went wrong");

    std::string string = error->toString();
    EXPECT_FALSE(string.empty());
    EXPECT_NE(string.find(domain.type().data()), std::string::npos) << string << ", " << domain.type().data();
    EXPECT_NE(string.find("100"), std::string::npos);
    EXPECT_NE(string.find("Something went wrong"), std::string::npos);
}

TEST(ErrorTest, ToStringWithCause)
{
    auto domain = testDomain();
    auto cause = Error::create(domain, TestDomain::kTestError1, "Root cause");
    auto error = Error::create(domain, TestDomain::kTestError2, "Wrapper", cause);

    std::string string = error->toString();
    EXPECT_NE(string.find("Wrapper"), std::string::npos);
    EXPECT_NE(string.find("Root cause"), std::string::npos);
    EXPECT_NE(string.find("Caused by"), std::string::npos);
}

TEST(ErrorTest, Depth)
{
    auto domain = testDomain();

    auto error0 = Error::create(domain, TestDomain::kTestError1, "Level 0");
    EXPECT_EQ(error0->depth(), 0u);

    auto error1 = Error::create(domain, TestDomain::kTestError2, "Level 1", error0);
    EXPECT_EQ(error1->depth(), 1u);

    auto error2 = Error::create(domain, TestDomain::kTestError3, "Level 2", error1);
    EXPECT_EQ(error2->depth(), 2u);
}

OCTK_END_NAMESPACE
