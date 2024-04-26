#include "openrct2/core/Endianness.h"

#include <gtest/gtest.h>

TEST(SwapBETest, ForUInt8_DoesNothing)
{
    uint8_t before = 0x12;
    uint8_t after = ByteSwapBE(before);
    ASSERT_EQ(before, after);
}

TEST(SwapBETest, ForUInt16_SwapsBytes)
{
    uint16_t before = 0x1234;
    uint16_t after = ByteSwapBE(before);
    ASSERT_EQ(0x3412u, after);
}

TEST(SwapBETest, ForUInt32_SwapsBytes)
{
    uint32_t before = 0x12345678;
    uint32_t after = ByteSwapBE(before);
    ASSERT_EQ(0x78563412u, after);
}

TEST(SwapBETest, ForUInt64_SwapsBytes)
{
    uint64_t before = 0x1234567887654321;
    uint64_t after = ByteSwapBE(before);
    ASSERT_EQ(0x2143658778563412u, after);
}

TEST(SwapBETest, ForCustomBlittableType_SwapsBytes)
{
    struct MyStruct
    {
        uint16_t value;
    };

    MyStruct before = { 0x1234 };
    MyStruct after = ByteSwapBE(before);
    ASSERT_EQ(0x3412, after.value);
}
