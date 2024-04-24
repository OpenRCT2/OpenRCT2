/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/GameTicks.h>

struct GameTickTag {};

using TestGameTick = GameTicks<uint32_t, GameTickTag>;

///////////////////////////////////////////////////////////////////////////////
// Tests for GameTicks<T, Tag>
///////////////////////////////////////////////////////////////////////////////

TEST(GameTicksTest, Construction)
{
    TestGameTick tick(10);
    EXPECT_EQ(10, tick.Value);
}

TEST(GameTicksTest, FromSeconds)
{
    TestGameTick actual = TestGameTick::FromMilliseconds(0);
    EXPECT_EQ(0, actual.Value);

    TestGameTick actual = TestGameTick::FromSeconds(1);
    EXPECT_EQ(40, actual.Value);
}

TEST(GameTicksTest, FromMilliseconds)
{
    TestGameTick actual = TestGameTick::FromMilliseconds(0);
    EXPECT_EQ(0, actual.Value);

    TestGameTick actual = TestGameTick::FromMilliseconds(1);
    EXPECT_EQ(1, actual.Value);

    TestGameTick actual = TestGameTick::FromMilliseconds(24);
    EXPECT_EQ(1, actual.Value);

    TestGameTick actual = TestGameTick::FromMilliseconds(1000);
    EXPECT_EQ(40, actual.Value);
}

TEST(GameTicksTest, Addition)
{
    TestGameTick tick1(10);
    TestGameTick tick2(20);

    TestGameTick actual = tick1 + tick2;
    EXPECT_EQ(30, actual.Value);
}

TEST(GameTicksTest, Subtraction)
{
    TestGameTick tick1(10);
    TestGameTick tick2(20);

    TestGameTick actual = tick2 - tick1;
    EXPECT_EQ(10, actual.Value);
}

TEST(GameTicksTest, Multiplication)
{
    TestGameTick tick1(10);
    TestGameTick tick2(20);

    TestGameTick actual = tick1 * tick2;
    EXPECT_EQ(200, actual.Value);
}

TEST(GameTicksTest, Division)
{
    TestGameTick tick1(10);
    TestGameTick tick2(20);

    TestGameTick actual = tick2 / tick1;
    EXPECT_EQ(2, actual.Value);
}

TEST(GameTicksTest, Modulus)
{
    TestGameTick tick1(10);
    TestGameTick tick2(20);

    TestGameTick actual = tick2 % tick1;
    EXPECT_EQ(0, actual.Value);

    TestGameTick tick3(3);
    TestGameTick tick4(2);

    TestGameTick actual = tick3 % tick4;
    EXPECT_EQ(1, actual.Value);
}

TEST(GameTicksTest, IncrementDecrement)
{
    TestGameTick tick(10);

    ++tick;
    EXPECT_EQ(11, tick.Value);

    tick++;
    EXPECT_EQ(12, tick.Value);

    --tick;
    EXPECT_EQ(11, tick.Value);

    tick--;
    EXPECT_EQ(10, tick.Value);
}

TEST(GameTicksTest, Comparisons)
{
    TestGameTick tick1(10);
    TestGameTick tick2(20);
    TestGameTick tick3(10);

    EXPECT_TRUE(tick1 == tick3);
    EXPECT_TRUE(tick1 != tick2);
    EXPECT_TRUE(tick2 > tick1);
    EXPECT_TRUE(tick1 < tick2);
    EXPECT_TRUE(tick1 <= tick3);
    EXPECT_TRUE(tick2 >= tick1);
}
