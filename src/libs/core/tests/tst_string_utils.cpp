#include <octk_string_utils.hpp>

#include <string>

#include "gtest/gtest.h"

namespace
{

TEST(MatchTest, StartsWith)
{
    const std::string s1("123\0abc", 7);
    const octk::StringView a("foobar");
    const octk::StringView b(s1);
    const octk::StringView e;
    EXPECT_TRUE(octk::utils::stringStartsWith(a, a));
    EXPECT_TRUE(octk::utils::stringStartsWith(a, "foo"));
    EXPECT_TRUE(octk::utils::stringStartsWith(a, e));
    EXPECT_TRUE(octk::utils::stringStartsWith(b, s1));
    EXPECT_TRUE(octk::utils::stringStartsWith(b, b));
    EXPECT_TRUE(octk::utils::stringStartsWith(b, e));
    EXPECT_TRUE(octk::utils::stringStartsWith(e, ""));
    EXPECT_FALSE(octk::utils::stringStartsWith(a, b));
    EXPECT_FALSE(octk::utils::stringStartsWith(b, a));
    EXPECT_FALSE(octk::utils::stringStartsWith(e, a));
}

TEST(MatchTest, EndsWith)
{
    const std::string s1("123\0abc", 7);
    const octk::StringView a("foobar");
    const octk::StringView b(s1);
    const octk::StringView e;
    EXPECT_TRUE(octk::utils::stringEndsWith(a, a));
    EXPECT_TRUE(octk::utils::stringEndsWith(a, "bar"));
    EXPECT_TRUE(octk::utils::stringEndsWith(a, e));
    EXPECT_TRUE(octk::utils::stringEndsWith(b, s1));
    EXPECT_TRUE(octk::utils::stringEndsWith(b, b));
    EXPECT_TRUE(octk::utils::stringEndsWith(b, e));
    EXPECT_TRUE(octk::utils::stringEndsWith(e, ""));
    EXPECT_FALSE(octk::utils::stringEndsWith(a, b));
    EXPECT_FALSE(octk::utils::stringEndsWith(b, a));
    EXPECT_FALSE(octk::utils::stringEndsWith(e, a));
}

TEST(MatchTest, Contains)
{
    octk::StringView a("abcdefg");
    octk::StringView b("abcd");
    octk::StringView c("efg");
    octk::StringView d("gh");
    EXPECT_TRUE(octk::utils::stringContains(a, a));
    EXPECT_TRUE(octk::utils::stringContains(a, b));
    EXPECT_TRUE(octk::utils::stringContains(a, c));
    EXPECT_FALSE(octk::utils::stringContains(a, d));
    EXPECT_TRUE(octk::utils::stringContains("", ""));
    EXPECT_TRUE(octk::utils::stringContains("abc", ""));
    EXPECT_FALSE(octk::utils::stringContains("", "a"));
}

TEST(MatchTest, ContainsChar)
{
    octk::StringView a("abcdefg");
    octk::StringView b("abcd");
    EXPECT_TRUE(octk::utils::stringContains(a, 'a'));
    EXPECT_TRUE(octk::utils::stringContains(a, 'b'));
    EXPECT_TRUE(octk::utils::stringContains(a, 'e'));
    EXPECT_FALSE(octk::utils::stringContains(a, 'h'));

    EXPECT_TRUE(octk::utils::stringContains(b, 'a'));
    EXPECT_TRUE(octk::utils::stringContains(b, 'b'));
    EXPECT_FALSE(octk::utils::stringContains(b, 'e'));
    EXPECT_FALSE(octk::utils::stringContains(b, 'h'));

    EXPECT_FALSE(octk::utils::stringContains("", 'a'));
    EXPECT_FALSE(octk::utils::stringContains("", 'a'));
}

TEST(MatchTest, ContainsNull)
{
    const std::string s = "foo";
    const char *cs = "foo";
    const octk::StringView sv("foo");
    const octk::StringView sv2("foo\0bar", 4);
    EXPECT_EQ(s, "foo");
    EXPECT_EQ(sv, "foo");
    EXPECT_NE(sv2, "foo");
    EXPECT_TRUE(octk::utils::stringEndsWith(s, sv));
    EXPECT_TRUE(octk::utils::stringStartsWith(cs, sv));
    EXPECT_TRUE(octk::utils::stringContains(cs, sv));
    EXPECT_FALSE(octk::utils::stringContains(cs, sv2));
}

TEST(MatchTest, EqualsIgnoreCase)
{
    std::string text = "the";
    octk::StringView data(text);

    EXPECT_TRUE(octk::utils::stringEqualsIgnoreCase(data, "The"));
    EXPECT_TRUE(octk::utils::stringEqualsIgnoreCase(data, "THE"));
    EXPECT_TRUE(octk::utils::stringEqualsIgnoreCase(data, "the"));
    EXPECT_FALSE(octk::utils::stringEqualsIgnoreCase(data, "Quick"));
    EXPECT_FALSE(octk::utils::stringEqualsIgnoreCase(data, "then"));
}

TEST(MatchTest, StartsWithIgnoreCase)
{
    EXPECT_TRUE(octk::utils::stringStartsWithIgnoreCase("foo", "foo"));
    EXPECT_TRUE(octk::utils::stringStartsWithIgnoreCase("foo", "Fo"));
    EXPECT_TRUE(octk::utils::stringStartsWithIgnoreCase("foo", ""));
    EXPECT_FALSE(octk::utils::stringStartsWithIgnoreCase("foo", "fooo"));
    EXPECT_FALSE(octk::utils::stringStartsWithIgnoreCase("", "fo"));
}

TEST(MatchTest, EndsWithIgnoreCase)
{
    EXPECT_TRUE(octk::utils::stringEndsWithIgnoreCase("foo", "foo"));
    EXPECT_TRUE(octk::utils::stringEndsWithIgnoreCase("foo", "Oo"));
    EXPECT_TRUE(octk::utils::stringEndsWithIgnoreCase("foo", ""));
    EXPECT_FALSE(octk::utils::stringEndsWithIgnoreCase("foo", "fooo"));
    EXPECT_FALSE(octk::utils::stringEndsWithIgnoreCase("", "fo"));
}

TEST(MatchTest, ContainsIgnoreCase)
{
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase("foo", "foo"));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase("FOO", "Foo"));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase("--FOO", "Foo"));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase("FOO--", "Foo"));
    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase("BAR", "Foo"));
    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase("BAR", "Foo"));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase("123456", "123456"));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase("123456", "234"));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase("", ""));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase("abc", ""));
    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase("", "a"));
}

TEST(MatchTest, ContainsCharIgnoreCase)
{
    octk::StringView a("AaBCdefg!");
    octk::StringView b("AaBCd!");
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase(a, 'a'));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase(a, 'A'));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase(a, 'b'));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase(a, 'B'));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase(a, 'e'));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase(a, 'E'));
    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase(a, 'h'));
    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase(a, 'H'));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase(a, '!'));
    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase(a, '?'));

    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase(b, 'a'));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase(b, 'A'));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase(b, 'b'));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase(b, 'B'));
    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase(b, 'e'));
    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase(b, 'E'));
    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase(b, 'h'));
    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase(b, 'H'));
    EXPECT_TRUE(octk::utils::stringContainsIgnoreCase(b, '!'));
    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase(b, '?'));

    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase("", 'a'));
    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase("", 'A'));
    EXPECT_FALSE(octk::utils::stringContainsIgnoreCase("", '0'));
}

}  // namespace
