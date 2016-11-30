#include "localisation/LanguagePack.h"
#include <gtest/gtest.h>

TEST(LanguagePackTest, create_empty)
{
    ILanguagePack * empty = LanguagePackFactory::FromText(0, "");
    ASSERT_EQ(empty->GetId(), 0);
    ASSERT_EQ(empty->GetCount(), 0);
    delete empty;
}

TEST(LanguagePackTest, create_mutable_id_1)
{
    ILanguagePack * lang = LanguagePackFactory::FromText(1, "STR_0000:\n");
    ASSERT_EQ(lang->GetId(), 1);
    ASSERT_EQ(lang->GetCount(), 1);
    ASSERT_STREQ(lang->GetString(0), "");
    lang->SetString(0, "xx");
    ASSERT_EQ(lang->GetCount(), 1);
    ASSERT_STREQ(lang->GetString(0), "xx");
    delete lang;
}

extern const utf8 * LanguageEnGB;

TEST(LanguagePackTest, language_pack_simple)
{
    ILanguagePack * lang = LanguagePackFactory::FromText(0, LanguageEnGB);
    ASSERT_EQ(lang->GetId(), 0);
    ASSERT_EQ(lang->GetCount(), 4);
    ASSERT_STREQ(lang->GetString(2), "Spiral Roller Coaster");
    ASSERT_EQ(lang->GetScenarioOverrideStringId("Arid Heights", 0), 0x7000);
    ASSERT_STREQ(lang->GetString(0x7000), "Arid Heights scenario string");
    ASSERT_EQ(lang->GetObjectOverrideStringId("CONDORRD", 0), 0x6000);
    ASSERT_STREQ(lang->GetString(0x6000), "my test ride");
    delete lang;
}

const utf8 * LanguageEnGB = "# STR_XXXX part is read and XXXX becomes the string id number.\n"
                            "# Everything after the colon and before the new line will be saved as the "
                            "string.\n"
                            "# Use # at the beginning of a line to leave a comment.\n"
                            "STR_0000    :\n"
                            "STR_0001    :{STRINGID} {COMMA16}\n"
                            "STR_0002    :Spiral Roller Coaster\n"
                            "STR_0003    :Stand-up Roller Coaster\n"
                            "<Arid Heights>\n"
                            "STR_SCNR    :Arid Heights scenario string\n"
                            "STR_PARK    :Arid Heights park string\n"
                            "STR_DTLS    :Free of any financial limits, your challenge is to develop "
                            "this desert park while keeping the guests happy\n"
                            "[CONDORRD]\n"
                            "STR_NAME    :my test ride\n"
                            "STR_DESC    :ride description\n"
                            "STR_CPTY    :ride capacity\n";
