/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/core/BitSet.hpp>

using namespace OpenRCT2;

TEST(BitTest, test_index_construction)
{
    BitSet<64U> bits({ 0U, 2U, 4U, 6U, 8U, 10U });
#if defined(_M_X64) || defined(_M_ARM64)
    static_assert(std::is_same_v<decltype(bits)::BlockType, uint64_t>);
#else
    static_assert(std::is_same_v<decltype(bits)::BlockType, uint32_t>);
#endif
    constexpr auto size = sizeof(bits);
    static_assert(size == 8U);

    ASSERT_EQ(bits.data()[0], 0b10101010101U);
    ASSERT_EQ(bits.to_string(), std::string("0000000000000000000000000000000000000000000000000000010101010101"));
}

TEST(BitTest, test_basic)
{
    BitSet<8U> bits;
    static_assert(std::is_same_v<decltype(bits)::BlockType, uint8_t>);
    constexpr auto size = sizeof(bits);
    static_assert(size == sizeof(uint8_t));

    bits[0] = true;
    ASSERT_EQ(bits[0], true);
    bits[6] = true;
    ASSERT_EQ(bits[6], true);
    bits[0] = false;
    ASSERT_EQ(bits[0], false);
    bits[0] = bits[6];
    ASSERT_EQ(bits[0], true);
    ASSERT_EQ(bits.data()[0U], 0b01000001U);
    ASSERT_EQ(bits.to_string(), std::string("01000001"));
}

TEST(BitTest, test_flip)
{
    BitSet<8U> bits;
    static_assert(std::is_same_v<decltype(bits)::BlockType, uint8_t>);
    constexpr auto size = sizeof(bits);
    static_assert(size == sizeof(uint8_t));

    bits.flip();
    ASSERT_EQ(bits.data()[0U], 0xFFu);
    bits.flip();
    ASSERT_EQ(bits.data()[0U], 0x00u);
}

TEST(BitTest, test_trim8)
{
    BitSet<5U> bits;
    static_assert(std::is_same_v<decltype(bits)::BlockType, uint8_t>);
    constexpr auto size = sizeof(bits);
    static_assert(size == sizeof(uint8_t));

    bits.flip();
    ASSERT_EQ(bits[5], false);
    ASSERT_EQ(bits[6], false);
    ASSERT_EQ(bits[7], false);
    ASSERT_EQ(bits.data()[0U], 0b00011111u);
}

TEST(BitTest, test_trim16)
{
    BitSet<14U> bits;
    static_assert(std::is_same_v<decltype(bits)::BlockType, uint16_t>);
    constexpr auto size = sizeof(bits);
    static_assert(size == sizeof(uint16_t));

    bits.flip();
    ASSERT_EQ(bits[14], false);
    ASSERT_EQ(bits[15], false);
    ASSERT_EQ(bits.data()[0U], 0b0011111111111111u);
    ASSERT_EQ(bits.to_string(), std::string("11111111111111"));
}

TEST(BitTest, test_big)
{
    BitSet<256U> bits;
    constexpr auto size = sizeof(bits);
    static_assert(size == 32U);

    bits.flip();
#if defined(_M_X64) || defined(_M_ARM64)
    static_assert(std::is_same_v<decltype(bits)::BlockType, uint64_t>);
    static_assert(bits.data().size() == 4);
    ASSERT_EQ(bits.data()[0], ~0ULL);
    ASSERT_EQ(bits.data()[1], ~0ULL);
    ASSERT_EQ(bits.data()[2], ~0ULL);
    ASSERT_EQ(bits.data()[3], ~0ULL);
#else
    static_assert(std::is_same_v<decltype(bits)::BlockType, uint32_t>);
    static_assert(bits.data().size() == 8);
    ASSERT_EQ(bits.data()[0], ~0UL);
    ASSERT_EQ(bits.data()[1], ~0UL);
    ASSERT_EQ(bits.data()[2], ~0UL);
    ASSERT_EQ(bits.data()[3], ~0UL);
    ASSERT_EQ(bits.data()[4], ~0UL);
    ASSERT_EQ(bits.data()[5], ~0UL);
    ASSERT_EQ(bits.data()[6], ~0UL);
    ASSERT_EQ(bits.data()[7], ~0UL);
#endif
}

TEST(BitTest, test_xor5)
{
    BitSet<5U> bits1({ 0U, 2U, 4U });
    BitSet<5U> bits2({ 0U, 1U, 3U });

    auto res = bits1 ^ bits2;
    ASSERT_EQ(res.data()[0], 0b11110u);
    ASSERT_EQ(res.to_string(), "11110");
}

TEST(BitTest, test_xor15)
{
    BitSet<15U> bits1({ 0U, 2U, 4U });
    BitSet<15U> bits2({ 0U, 1U, 3U, 14U });

    auto res = bits1 ^ bits2;
    ASSERT_EQ(res.data()[0], 0b0100000000011110u);
    ASSERT_EQ(res.to_string(), "100000000011110");
}

TEST(BitTest, test_or5)
{
    BitSet<5U> bits1({ 0U, 2U, 4U });
    BitSet<5U> bits2({ 0U, 1U, 3U });

    auto res = bits1 | bits2;
    ASSERT_EQ(res.data()[0], 0b11111);
    ASSERT_EQ(res.to_string(), "11111");
}

TEST(BitTest, test_or15)
{
    BitSet<15U> bits1({ 0U, 2U, 4U });
    BitSet<15U> bits2({ 0U, 1U, 3U, 14U });

    auto res = bits1 | bits2;
    ASSERT_EQ(res.data()[0], 0b0100000000011111u);
    ASSERT_EQ(res.to_string(), "100000000011111");
}

TEST(BitTest, test_and5)
{
    BitSet<5U> bits1({ 0U, 2U, 4U });
    BitSet<5U> bits2({ 0U, 1U, 3U });

    auto res = bits1 & bits2;
    ASSERT_EQ(res.data()[0], 0b1);
    ASSERT_EQ(res.to_string(), "00001");
}

TEST(BitTest, test_and15)
{
    BitSet<15U> bits1({ 0U, 2U, 4U });
    BitSet<15U> bits2({ 0U, 1U, 3U, 14U });

    auto res = bits1 & bits2;
    ASSERT_EQ(res.data()[0], 0b1);
    ASSERT_EQ(res.to_string(), "000000000000001");
}

TEST(BitTest, test_neg5)
{
    BitSet<5U> bits1({ 0U, 2U, 4U });

    auto res = ~bits1;
    ASSERT_EQ(res.data()[0], 0b01010u);
    ASSERT_EQ(res.to_string(), "01010");
}

TEST(BitTest, test_neg15)
{
    BitSet<15U> bits1({ 0U, 2U, 4U });

    auto res = ~bits1;
    ASSERT_EQ(res.data()[0], 0b111111111101010u);
    ASSERT_EQ(res.to_string(), "111111111101010");
}

TEST(BitTest, test_count)
{
    BitSet<31U> bits1({ 0U, 2U, 4U, 7U, 9U, 12U, 16U, 19U, 22U, 29U });
    ASSERT_EQ(bits1.count(), 10U);
}

TEST(BitTest, test_iterator)
{
    BitSet<31U> bits1({ 0U, 2U, 4U, 7U, 9U, 12U, 16U, 19U, 22U, 29U });

    int totalBits = 0;
    for (auto v : bits1)
    {
        if (v)
            totalBits++;
    }
    ASSERT_EQ(totalBits, 10);
}

TEST(BitTest, test_iterator_const)
{
    BitSet<31U> bits1({ 0U, 2U, 4U, 7U, 9U, 12U, 16U, 19U, 22U, 29U });

    int totalBits = 0;
    auto test = [&](const auto& b) {
        for (auto v : b)
        {
            if (v)
                totalBits++;
        }
    };

    test(bits1);
    ASSERT_EQ(totalBits, 10);
}
