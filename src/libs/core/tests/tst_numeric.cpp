//
// Created by cxw on 25-8-11.
//

#include <octk_numeric.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace octk;

TEST(GCDTest, PositiveNumbers)
{
    EXPECT_EQ(3, utils::gcd(9, 6));
    EXPECT_EQ(1, utils::gcd(17, 13));
    EXPECT_EQ(14, utils::gcd(42, 56));
}

TEST(GCDTest, NegativeNumbers)
{
    EXPECT_EQ(4, utils::gcd(-12, 8));
    EXPECT_EQ(4, utils::gcd(12, -8));
    EXPECT_EQ(4, utils::gcd(-12, -8));
}

TEST(GCDTest, LargeNumbers)
{
    EXPECT_EQ(0, (INT_MAX - 1) % 6);
    EXPECT_EQ(6, utils::gcd(INT_MAX - 1, 300));
    EXPECT_EQ(1, utils::gcd(INT_MAX, INT_MAX - 2));
    EXPECT_EQ(300, utils::gcd(90000, 300));
    EXPECT_EQ(1, utils::gcd(1234567, 1234568));
}

TEST(GCDTest, EdgeCases)
{
    EXPECT_EQ(0, utils::gcd(0, 0));
    EXPECT_EQ(5, utils::gcd(5, 0));
    EXPECT_EQ(5, utils::gcd(0, 5));
    EXPECT_EQ(1, utils::gcd(1, 1));
}

TEST(GCDTest, CompareImplementations)
{
    EXPECT_EQ(utils::gcd(9, 6), utils::gcd_iterative(9, 6));
    EXPECT_EQ(utils::gcd(0, 5), utils::gcd_iterative(0, 5));
    EXPECT_EQ(utils::gcd(-12, 8), utils::gcd_iterative(-12, 8));
}

TEST(GCDTest, Performance)
{
    const int a = 1234567890;
    const int b = 987654321;
    for (int i = 0; i < 10000; ++i)
    {
        EXPECT_EQ(9, utils::gcd(a, b));
    }
}

TEST(LCMTest, BasicCalculation)
{
    EXPECT_EQ(12, utils::lcm(3, 4));
    EXPECT_EQ(12, utils::lcm(4, 6));
    EXPECT_EQ(35, utils::lcm(5, 7));
    EXPECT_EQ(60, utils::lcm(12, 15));
}

TEST(LCMTest, ZeroInput)
{
    EXPECT_EQ(0, utils::lcm(0, 5));
    EXPECT_EQ(0, utils::lcm(0, 0));
    EXPECT_EQ(0, utils::lcm(8, 0));
}

TEST(LCMTest, NegativeNumbers)
{
    EXPECT_EQ(12, utils::lcm(-4, 6));
    EXPECT_EQ(35, utils::lcm(5, -7));
    EXPECT_EQ(60, utils::lcm(-12, -15));
}

TEST(LCMTest, BoundaryConditions)
{
    EXPECT_EQ(0, utils::lcm(0, 0));
    EXPECT_EQ(0, utils::lcm(5, 0));
    EXPECT_EQ(0, utils::lcm(0, 5));
    EXPECT_EQ(5, utils::lcm(5, 5));
}

TEST(LCMTest, MixedTypes)
{
    EXPECT_EQ(24L, utils::lcm(8, 12L));  // int + long
    EXPECT_EQ(100, utils::lcm(25, 100u)); // signed + unsigned
}

TEST(LCMTest, LargeNumbers)
{
    EXPECT_EQ(INT_MAX, utils::lcm(INT_MAX, 1));
    EXPECT_EQ(INT_MAX - 1, utils::lcm(INT_MAX - 1, 1));
}

TEST(LCMTest, GCDRelation)
{
    const int a = 56;
    const int b = 98;
    EXPECT_EQ((a * b) / utils::gcd(a, b), utils::lcm(a, b));
}