/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "openrct2/config/IniWriter.hpp"

#include "openrct2/config/ConfigEnum.hpp"
#include "openrct2/core/MemoryStream.h"
#include "openrct2/platform/Platform.h"

#include <gtest/gtest.h>
#include <limits>

class IniWriterTest : public testing::Test
{
};

static auto Enum_Currency = ConfigEnum<int32_t>({
    ConfigEnumEntry<int32_t>("GBP", 1),
});

TEST_F(IniWriterTest, create_empty)
{
    OpenRCT2::MemoryStream ms(0);
    ASSERT_EQ(ms.CanRead(), true);
    ASSERT_EQ(ms.CanWrite(), true);
    auto iw = CreateIniWriter(&ms);
    ASSERT_NE(iw, nullptr);
}

TEST_F(IniWriterTest, create_one_section)
{
    OpenRCT2::MemoryStream ms(1000);
    auto iw = CreateIniWriter(&ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteSection("OpenRCT2");
    uint8_t null_terminator = 0;
    ms.Write(&null_terminator, 1);
    ASSERT_GE(ms.GetPosition(), 12);
    ASSERT_LE(ms.GetPosition(), 13); // Accommodate for varying-sized newline (Windows)
    ASSERT_EQ(ms.GetLength(), ms.GetPosition());
    ms.SetPosition(0);
    const char* ini = reinterpret_cast<const char*>(ms.ReadString());
    ASSERT_STREQ(ini, "[OpenRCT2]" PLATFORM_NEWLINE);
    Memory::Free(ini);
}

TEST_F(IniWriterTest, create_multiple_sections)
{
    OpenRCT2::MemoryStream ms(1000);
    auto iw = CreateIniWriter(&ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteSection("OpenRCT1");
    iw->WriteSection("OpenRCT2");
    iw->WriteSection("OpenRCT3");
    iw->WriteSection("OpenRCT4");
    uint8_t null_terminator = 0;
    ms.Write(&null_terminator, 1);
    ASSERT_GE(ms.GetPosition(), 48);
    ASSERT_LE(ms.GetPosition(), 55); // Accommodate for varying-sized newline (Windows)
    ASSERT_EQ(ms.GetLength(), ms.GetPosition());
    ms.SetPosition(0);
    const char* ini = reinterpret_cast<const char*>(ms.ReadString());
    ASSERT_STREQ(
        ini,
        "[OpenRCT1]" PLATFORM_NEWLINE PLATFORM_NEWLINE "[OpenRCT2]" PLATFORM_NEWLINE PLATFORM_NEWLINE
        "[OpenRCT3]" PLATFORM_NEWLINE PLATFORM_NEWLINE "[OpenRCT4]" PLATFORM_NEWLINE);
    Memory::Free(ini);
}

TEST_F(IniWriterTest, create_loose_bool_entry)
{
    OpenRCT2::MemoryStream ms(1000);
    auto iw = CreateIniWriter(&ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteBoolean("boolval", true);
    uint8_t null_terminator = 0;
    ms.Write(&null_terminator, 1);
    ASSERT_GE(ms.GetPosition(), 16);
    ASSERT_LE(ms.GetPosition(), 17); // Accommodate for varying-sized newline (Windows)
    ASSERT_EQ(ms.GetLength(), ms.GetPosition());
    ms.SetPosition(0);
    const char* ini = reinterpret_cast<const char*>(ms.ReadString());
    ASSERT_STREQ(ini, "boolval = true" PLATFORM_NEWLINE);
    Memory::Free(ini);
}

TEST_F(IniWriterTest, create_loose_enum_entry)
{
    OpenRCT2::MemoryStream ms(1000);
    auto iw = CreateIniWriter(&ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteEnum("by_string", "stringval");
    iw->WriteEnum<int32_t>("int32_t", 0, Enum_Currency);
    uint8_t null_terminator = 0;
    ms.Write(&null_terminator, 1);
    ASSERT_GE(ms.GetPosition(), 34);
    ASSERT_LE(ms.GetPosition(), 37); // Accommodate for varying-sized newline (Windows)
    ASSERT_EQ(ms.GetLength(), ms.GetPosition());
    ms.SetPosition(0);
    const char* ini = reinterpret_cast<const char*>(ms.ReadString());
    ASSERT_STREQ(ini, "by_string = stringval" PLATFORM_NEWLINE "int32_t = 0" PLATFORM_NEWLINE);
    Memory::Free(ini);
}

TEST_F(IniWriterTest, create_loose_float_entry)
{
    OpenRCT2::MemoryStream ms(1000);
    auto iw = CreateIniWriter(&ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteFloat("one", 1.);
    uint8_t null_terminator = 0;
    ms.Write(&null_terminator, 1);
    ASSERT_GE(ms.GetPosition(), 16);
    ASSERT_LE(ms.GetPosition(), 17); // Accommodate for varying-sized newline (Windows)
    ASSERT_EQ(ms.GetLength(), ms.GetPosition());
    ms.SetPosition(0);
    const char* ini = reinterpret_cast<const char*>(ms.ReadString());
    // This will be non-fatal due to float.
    EXPECT_STREQ(ini, "one = 1.000000" PLATFORM_NEWLINE);
    Memory::Free(ini);
}

TEST_F(IniWriterTest, create_loose_int32_t_entry)
{
    OpenRCT2::MemoryStream ms(1000);
    auto iw = CreateIniWriter(&ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteInt32("one", 1);
    iw->WriteInt32("zero", 0);
    iw->WriteInt32("minusone", -1);
    iw->WriteInt32("intmin", (std::numeric_limits<int32_t>::min)());
    iw->WriteInt32("intmax", (std::numeric_limits<int32_t>::max)());
    uint8_t null_terminator = 0;
    ms.Write(&null_terminator, 1);
    ASSERT_GE(ms.GetPosition(), 73);
    ASSERT_LE(ms.GetPosition(), 78); // Accommodate for varying-sized newline (Windows)
    ASSERT_EQ(ms.GetLength(), ms.GetPosition());
    ms.SetPosition(0);
    const char* ini = reinterpret_cast<const char*>(ms.ReadString());
    ASSERT_STREQ(
        ini,
        "one = 1" PLATFORM_NEWLINE "zero = 0" PLATFORM_NEWLINE "minusone = -1" PLATFORM_NEWLINE
        "intmin = -2147483648" PLATFORM_NEWLINE "intmax = 2147483647" PLATFORM_NEWLINE);
    Memory::Free(ini);
}

TEST_F(IniWriterTest, create_loose_string_entry)
{
    OpenRCT2::MemoryStream ms(1000);
    auto iw = CreateIniWriter(&ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteString("path", u8"C:'\\some/dir\\here/神鷹暢遊");
    uint8_t null_terminator = 0;
    ms.Write(&null_terminator, 1);
    ASSERT_GE(ms.GetPosition(), 43);
    ASSERT_LE(ms.GetPosition(), 44); // Accommodate for varying-sized newline (Windows)
    ASSERT_EQ(ms.GetLength(), ms.GetPosition());
    ms.SetPosition(0);
    const char* ini = reinterpret_cast<const char*>(ms.ReadString());
    ASSERT_STREQ(ini, "path = \"C:'\\\\some/dir\\\\here/\xE7\xA5\x9E\xE9\xB7\xB9\xE6\x9A\xA2\xE9\x81\x8A\"" PLATFORM_NEWLINE);
    Memory::Free(ini);
}

TEST_F(IniWriterTest, create_multiple_section_with_values)
{
    OpenRCT2::MemoryStream ms(1000);
    auto iw = CreateIniWriter(&ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteSection("bool");
    iw->WriteBoolean("boolval", true);
    iw->WriteSection("int");
    iw->WriteInt32("one", 1);
    iw->WriteInt32("zero", 0);
    iw->WriteSection("string");
    iw->WriteString("path", u8"C:'\\some/dir\\here/神鷹暢遊");
    uint8_t null_terminator = 0;
    ms.Write(&null_terminator, 1);
    ASSERT_GE(ms.GetPosition(), 99);
    ASSERT_LE(ms.GetPosition(), 108); // Accommodate for varying-sized newline (Windows)
    ASSERT_EQ(ms.GetLength(), ms.GetPosition());
    ms.SetPosition(0);
    const char* ini = reinterpret_cast<const char*>(ms.ReadString());
    ASSERT_STREQ(
        ini,
        "[bool]" PLATFORM_NEWLINE "boolval = true" PLATFORM_NEWLINE PLATFORM_NEWLINE "[int]" PLATFORM_NEWLINE
        "one = 1" PLATFORM_NEWLINE "zero = 0" PLATFORM_NEWLINE PLATFORM_NEWLINE "[string]" PLATFORM_NEWLINE "path = "
        "\"C:'\\\\some/dir\\\\here/\xE7\xA5\x9E\xE9\xB7\xB9\xE6\x9A\xA2\xE9\x81\x8A\"" PLATFORM_NEWLINE);
    Memory::Free(ini);
}

TEST_F(IniWriterTest, create_duplicate_sections)
{
    OpenRCT2::MemoryStream ms(1000);
    auto iw = CreateIniWriter(&ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteSection("section");
    iw->WriteSection("section");
    iw->WriteSection("section");
    uint8_t null_terminator = 0;
    ms.Write(&null_terminator, 1);
    ASSERT_GE(ms.GetPosition(), 33);
    ASSERT_LE(ms.GetPosition(), 43); // Accommodate for varying-sized newline (Windows)
    ASSERT_EQ(ms.GetLength(), ms.GetPosition());
    ms.SetPosition(0);
    const char* ini = reinterpret_cast<const char*>(ms.ReadString());
    ASSERT_STREQ(
        ini,
        "[section]" PLATFORM_NEWLINE PLATFORM_NEWLINE "[section]" PLATFORM_NEWLINE PLATFORM_NEWLINE
        "[section]" PLATFORM_NEWLINE);
    Memory::Free(ini);
}
