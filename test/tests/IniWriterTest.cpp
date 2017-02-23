#include <gtest/gtest.h>
#include <limits>
#include "openrct2/config/ConfigEnum.hpp"
#include "openrct2/config/IniWriter.hpp"
#include "openrct2/core/MemoryStream.h"

class IniWriterTest : public testing::Test
{
};

static auto Enum_Currency = ConfigEnum<sint32>({
    ConfigEnumEntry<sint32>("GBP", 1),
});

TEST_F(IniWriterTest, create_empty)
{
    MemoryStream * ms = new MemoryStream(0);
    ASSERT_NE(ms, nullptr);
    ASSERT_EQ(ms->CanRead(), true);
    ASSERT_EQ(ms->CanWrite(), true);
    IIniWriter * iw = CreateIniWriter(ms);
    ASSERT_NE(iw, nullptr);
    delete iw;
    delete ms;
}

TEST_F(IniWriterTest, create_one_section)
{
    MemoryStream * ms = new MemoryStream(1000);
    ASSERT_NE(ms, nullptr);
    IIniWriter * iw = CreateIniWriter(ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteSection("OpenRCT2");
    int8_t null_terminator = 0;
    ms->Write(&null_terminator, 1);
    ASSERT_EQ(ms->GetPosition(), 12);
    ASSERT_EQ(ms->GetLength(), 12);
    ms->SetPosition(0);
    const char * ini = (const char *)ms->ReadString();
    ASSERT_STREQ(ini, "[OpenRCT2]\n");
    Memory::Free(ini);
    delete iw;
    delete ms;
}

TEST_F(IniWriterTest, create_multiple_sections)
{
    MemoryStream * ms = new MemoryStream(1000);
    ASSERT_NE(ms, nullptr);
    IIniWriter * iw = CreateIniWriter(ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteSection("OpenRCT1");
    iw->WriteSection("OpenRCT2");
    iw->WriteSection("OpenRCT3");
    iw->WriteSection("OpenRCT4");
    int8_t null_terminator = 0;
    ms->Write(&null_terminator, 1);
    ASSERT_EQ(ms->GetPosition(), 48);
    ASSERT_EQ(ms->GetLength(), 48);
    ms->SetPosition(0);
    const char * ini = (const char *)ms->ReadString();
    ASSERT_STREQ(ini, "[OpenRCT1]\n\n[OpenRCT2]\n\n[OpenRCT3]\n\n[OpenRCT4]\n");
    Memory::Free(ini);
    delete iw;
    delete ms;
}

TEST_F(IniWriterTest, create_loose_bool_entry)
{
    MemoryStream * ms = new MemoryStream(1000);
    ASSERT_NE(ms, nullptr);
    IIniWriter * iw = CreateIniWriter(ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteBoolean("boolval", true);
    int8_t null_terminator = 0;
    ms->Write(&null_terminator, 1);
    ASSERT_EQ(ms->GetPosition(), 16);
    ASSERT_EQ(ms->GetLength(), 16);
    ms->SetPosition(0);
    const char * ini = (const char *)ms->ReadString();
    ASSERT_STREQ(ini, "boolval = true\n");
    Memory::Free(ini);
    delete iw;
    delete ms;
}

TEST_F(IniWriterTest, create_loose_enum_entry)
{
    MemoryStream * ms = new MemoryStream(1000);
    ASSERT_NE(ms, nullptr);
    IIniWriter * iw = CreateIniWriter(ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteEnum("by_string", "stringval");
    iw->WriteEnum<sint32>("sint32", 0, Enum_Currency);
    int8_t null_terminator = 0;
    ms->Write(&null_terminator, 1);
    ASSERT_EQ(ms->GetPosition(), 34);
    ASSERT_EQ(ms->GetLength(), 34);
    ms->SetPosition(0);
    const char * ini = (const char *)ms->ReadString();
    ASSERT_STREQ(ini, "by_string = stringval\nsint32 = 0\n");
    Memory::Free(ini);
    delete iw;
    delete ms;
}

TEST_F(IniWriterTest, create_loose_float_entry)
{
    MemoryStream * ms = new MemoryStream(1000);
    ASSERT_NE(ms, nullptr);
    IIniWriter * iw = CreateIniWriter(ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteFloat("one", 1.);
    int8_t null_terminator = 0;
    ms->Write(&null_terminator, 1);
    ASSERT_EQ(ms->GetPosition(), 16);
    ASSERT_EQ(ms->GetLength(), 16);
    ms->SetPosition(0);
    const char * ini = (const char *)ms->ReadString();
    // This will be non-fatal due to float.
    EXPECT_STREQ(ini, "one = 1.000000\n");
    Memory::Free(ini);
    delete iw;
    delete ms;
}

TEST_F(IniWriterTest, create_loose_sint32_entry)
{
    MemoryStream * ms = new MemoryStream(1000);
    ASSERT_NE(ms, nullptr);
    IIniWriter * iw = CreateIniWriter(ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteSint32("one", 1);
    iw->WriteSint32("zero", 0);
    iw->WriteSint32("minusone", -1);
    iw->WriteSint32("intmin", std::numeric_limits<sint32>::min());
    iw->WriteSint32("intmax", std::numeric_limits<sint32>::max());
    int8_t null_terminator = 0;
    ms->Write(&null_terminator, 1);
    ASSERT_EQ(ms->GetPosition(), 73);
    ASSERT_EQ(ms->GetLength(), 73);
    ms->SetPosition(0);
    const char * ini = (const char *)ms->ReadString();
    ASSERT_STREQ(ini, "one = 1\nzero = 0\nminusone = -1\nintmin = -2147483648\nintmax = 2147483647\n");
    Memory::Free(ini);
    delete iw;
    delete ms;
}

TEST_F(IniWriterTest, create_loose_string_entry)
{
    MemoryStream * ms = new MemoryStream(1000);
    ASSERT_NE(ms, nullptr);
    IIniWriter * iw = CreateIniWriter(ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteString("path", u8"C:'\\some/dir\\here/神鷹暢遊");
    int8_t null_terminator = 0;
    ms->Write(&null_terminator, 1);
    ASSERT_EQ(ms->GetPosition(), 43);
    ASSERT_EQ(ms->GetLength(), 43);
    ms->SetPosition(0);
    const char * ini = (const char *)ms->ReadString();
    ASSERT_STREQ(ini, "path = \"C:'\\\\some/dir\\\\here/\xE7\xA5\x9E\xE9\xB7\xB9\xE6\x9A\xA2\xE9\x81\x8A\"\n");
    Memory::Free(ini);
    delete iw;
    delete ms;
}

TEST_F(IniWriterTest, create_multiple_section_with_values)
{
    MemoryStream * ms = new MemoryStream(1000);
    ASSERT_NE(ms, nullptr);
    IIniWriter * iw = CreateIniWriter(ms);
    ASSERT_NE(iw, nullptr);
    iw->WriteSection("bool");
    iw->WriteBoolean("boolval", true);
    iw->WriteSection("int");
    iw->WriteSint32("one", 1);
    iw->WriteSint32("zero", 0);
    iw->WriteSection("string");
    iw->WriteString("path", u8"C:'\\some/dir\\here/神鷹暢遊");
    int8_t null_terminator = 0;
    ms->Write(&null_terminator, 1);
    ASSERT_EQ(ms->GetPosition(), 99);
    ASSERT_EQ(ms->GetLength(), 99);
    ms->SetPosition(0);
    const char * ini = (const char *)ms->ReadString();
    ASSERT_STREQ(ini, "[bool]\nboolval = true\n\n[int]\none = 1\nzero = 0\n\n[string]\npath = "
                      "\"C:'\\\\some/dir\\\\here/\xE7\xA5\x9E\xE9\xB7\xB9\xE6\x9A\xA2\xE9\x81\x8A\"\n");
    Memory::Free(ini);
    delete iw;
    delete ms;
}
