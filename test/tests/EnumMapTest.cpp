/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include <gtest/gtest.h>
#include <openrct2/core/EnumMap.hpp>

enum class TestEnumClassContinuous
{
    A,
    B,
    C,
    D,
    E,
    F,
    G
};

enum class TestEnumClassNonContinuous
{
    A = 1,
    B,
    C = 7,
    D,
    E,
    F,
    G
};

template<typename TEnum> void TestEnumKeyLookup()
{
    // clang-format off
    EnumMap<TEnum> enumMap = {
        { "A", TEnum::A },
        { "B", TEnum::B },
        { "C", TEnum::C },
        { "D", TEnum::D },
        { "E", TEnum::E },
        { "F", TEnum::F },
        { "G", TEnum::G },
    };
    // clang-format on

    ASSERT_EQ(enumMap.find("Z"), enumMap.end());

    auto itA = enumMap.find("A");
    ASSERT_NE(itA, enumMap.end());
    ASSERT_EQ(itA->second, TEnum::A);

    auto itB = enumMap.find("B");
    ASSERT_NE(itB, enumMap.end());
    ASSERT_EQ(itB->second, TEnum::B);

    auto itC = enumMap.find("C");
    ASSERT_NE(itC, enumMap.end());
    ASSERT_EQ(itC->second, TEnum::C);

    auto itD = enumMap.find("D");
    ASSERT_NE(itD, enumMap.end());
    ASSERT_EQ(itD->second, TEnum::D);

    auto itE = enumMap.find("E");
    ASSERT_NE(itE, enumMap.end());
    ASSERT_EQ(itE->second, TEnum::E);

    auto itF = enumMap.find("F");
    ASSERT_NE(itF, enumMap.end());
    ASSERT_EQ(itF->second, TEnum::F);

    auto itG = enumMap.find("G");
    ASSERT_NE(itG, enumMap.end());
    ASSERT_EQ(itG->second, TEnum::G);

    SUCCEED();
}

template<typename TEnum> void TestEnumValueLookup()
{
    // clang-format off
    EnumMap<TEnum> enumMap = {
        { "A", TEnum::A },
        { "B", TEnum::B },
        { "C", TEnum::C },
        { "D", TEnum::D },
        { "E", TEnum::E },
        { "F", TEnum::F },
        { "G", TEnum::G },
    };
    // clang-format on

    ASSERT_EQ(enumMap.find("Z"), enumMap.end());

    auto itA = enumMap.find(TEnum::A);
    ASSERT_NE(itA, enumMap.end());
    ASSERT_EQ(itA->second, TEnum::A);

    auto itB = enumMap.find(TEnum::B);
    ASSERT_NE(itB, enumMap.end());
    ASSERT_EQ(itB->second, TEnum::B);

    auto itC = enumMap.find(TEnum::C);
    ASSERT_NE(itC, enumMap.end());
    ASSERT_EQ(itC->second, TEnum::C);

    auto itD = enumMap.find(TEnum::D);
    ASSERT_NE(itD, enumMap.end());
    ASSERT_EQ(itD->second, TEnum::D);

    auto itE = enumMap.find(TEnum::E);
    ASSERT_NE(itE, enumMap.end());
    ASSERT_EQ(itE->second, TEnum::E);

    auto itF = enumMap.find(TEnum::F);
    ASSERT_NE(itF, enumMap.end());
    ASSERT_EQ(itF->second, TEnum::F);

    auto itG = enumMap.find(TEnum::G);
    ASSERT_NE(itG, enumMap.end());
    ASSERT_EQ(itG->second, TEnum::G);

    SUCCEED();
}

TEST(EnumMapTest, LookupContinuousByKey)
{
    TestEnumKeyLookup<TestEnumClassContinuous>();
}

TEST(EnumMapTest, LookupContinuousByValue)
{
    TestEnumValueLookup<TestEnumClassContinuous>();
}

TEST(EnumMapTest, LookupNonContinuousByKey)
{
    TestEnumKeyLookup<TestEnumClassNonContinuous>();
}

TEST(EnumMapTest, LookupNonContinuousByValue)
{
    TestEnumValueLookup<TestEnumClassNonContinuous>();
}
