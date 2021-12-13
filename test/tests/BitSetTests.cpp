/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
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
    BitSet<64u> bits({ 0u, 2u, 4u, 6u, 8u, 10u });
#ifdef _M_X64
    static_assert(std::is_same_v<decltype(bits)::BlockType, uint64_t>);
#else
    static_assert(std::is_same_v<decltype(bits)::BlockType, uint32_t>);
#endif
    constexpr auto size = sizeof(bits);
    static_assert(size == 8u);

    ASSERT_EQ(bits.data()[0], 0b10101010101U);
    ASSERT_EQ(bits.to_string(), std::string("0000000000000000000000000000000000000000000000000000010101010101"));
}

TEST(BitTest, test_basic)
{
    BitSet<8u> bits;
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
    ASSERT_EQ(bits.data()[0u], 0b01000001U);
    ASSERT_EQ(bits.to_string(), std::string("01000001"));
}

TEST(BitTest, test_flip)
{
    BitSet<8u> bits;
    static_assert(std::is_same_v<decltype(bits)::BlockType, uint8_t>);
    constexpr auto size = sizeof(bits);
    static_assert(size == sizeof(uint8_t));

    bits.flip();
    ASSERT_EQ(bits.data()[0u], 0xFFu);
    bits.flip();
    ASSERT_EQ(bits.data()[0u], 0x00u);
}

TEST(BitTest, test_trim8)
{
    BitSet<5u> bits;
    static_assert(std::is_same_v<decltype(bits)::BlockType, uint8_t>);
    constexpr auto size = sizeof(bits);
    static_assert(size == sizeof(uint8_t));

    bits.flip();
    ASSERT_EQ(bits[5], false);
    ASSERT_EQ(bits[6], false);
    ASSERT_EQ(bits[7], false);
    ASSERT_EQ(bits.data()[0u], 0b00011111u);
}

TEST(BitTest, test_trim16)
{
    BitSet<14u> bits;
    static_assert(std::is_same_v<decltype(bits)::BlockType, uint16_t>);
    constexpr auto size = sizeof(bits);
    static_assert(size == sizeof(uint16_t));

    bits.flip();
    ASSERT_EQ(bits[14], false);
    ASSERT_EQ(bits[15], false);
    ASSERT_EQ(bits.data()[0u], 0b0011111111111111u);
    ASSERT_EQ(bits.to_string(), std::string("11111111111111"));
}

TEST(BitTest, test_big)
{
    BitSet<256u> bits;
    constexpr auto size = sizeof(bits);
    static_assert(size == 32u);

    bits.flip();
#ifdef _M_X64
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
    BitSet<5u> bits1({ 0u, 2u, 4u });
    BitSet<5u> bits2({ 0u, 1u, 3u });

    auto res = bits1 ^ bits2;
    ASSERT_EQ(res.data()[0], 0b11110u);
    ASSERT_EQ(res.to_string(), "11110");
}

TEST(BitTest, test_xor15)
{
    BitSet<15u> bits1({ 0u, 2u, 4u });
    BitSet<15u> bits2({ 0u, 1u, 3u, 14u });

    auto res = bits1 ^ bits2;
    ASSERT_EQ(res.data()[0], 0b0100000000011110u);
    ASSERT_EQ(res.to_string(), "100000000011110");
}

TEST(BitTest, test_or5)
{
    BitSet<5u> bits1({ 0u, 2u, 4u });
    BitSet<5u> bits2({ 0u, 1u, 3u });

    auto res = bits1 | bits2;
    ASSERT_EQ(res.data()[0], 0b11111);
    ASSERT_EQ(res.to_string(), "11111");
}

TEST(BitTest, test_or15)
{
    BitSet<15u> bits1({ 0u, 2u, 4u });
    BitSet<15u> bits2({ 0u, 1u, 3u, 14u });

    auto res = bits1 | bits2;
    ASSERT_EQ(res.data()[0], 0b0100000000011111u);
    ASSERT_EQ(res.to_string(), "100000000011111");
}

TEST(BitTest, test_and5)
{
    BitSet<5u> bits1({ 0u, 2u, 4u });
    BitSet<5u> bits2({ 0u, 1u, 3u });

    auto res = bits1 & bits2;
    ASSERT_EQ(res.data()[0], 0b1);
    ASSERT_EQ(res.to_string(), "00001");
}

TEST(BitTest, test_and15)
{
    BitSet<15u> bits1({ 0u, 2u, 4u });
    BitSet<15u> bits2({ 0u, 1u, 3u, 14u });

    auto res = bits1 & bits2;
    ASSERT_EQ(res.data()[0], 0b1);
    ASSERT_EQ(res.to_string(), "000000000000001");
}

TEST(BitTest, test_neg5)
{
    BitSet<5u> bits1({ 0u, 2u, 4u });

    auto res = ~bits1;
    ASSERT_EQ(res.data()[0], 0b01010u);
    ASSERT_EQ(res.to_string(), "01010");
}

TEST(BitTest, test_neg15)
{
    BitSet<15u> bits1({ 0u, 2u, 4u });

    auto res = ~bits1;
    ASSERT_EQ(res.data()[0], 0b111111111101010u);
    ASSERT_EQ(res.to_string(), "111111111101010");
}

TEST(BitTest, test_count)
{
    BitSet<31u> bits1({ 0u, 2u, 4u, 7u, 9u, 12u, 16u, 19u, 22u, 29u });
    ASSERT_EQ(bits1.count(), 10u);
}

TEST(BitTest, test_iterator)
{
    BitSet<31u> bits1({ 0u, 2u, 4u, 7u, 9u, 12u, 16u, 19u, 22u, 29u });

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
    BitSet<31u> bits1({ 0u, 2u, 4u, 7u, 9u, 12u, 16u, 19u, 22u, 29u });

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
