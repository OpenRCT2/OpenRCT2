#include <string>
#include <tuple>
#include <gtest/gtest.h>
#include <openrct2/core/String.hpp>

using TCase = std::tuple<std::string, std::string>;

class StringTest : public testing::TestWithParam<TCase>
{
};

INSTANTIATE_TEST_CASE_P(TrimData, StringTest, testing::Values(
    TCase("string", "string"),
    TCase("  string", "string"),
    TCase("string  ", "string"),
    TCase("   some   string  ", "some   string"),
    TCase("      ", ""),
    TCase("", ""),
    TCase("\n", ""),
    TCase("\n\n\n\r\n", ""),
    TCase("\n\n\n\r\nstring\n\n", "string")
));
TEST_P(StringTest, Trim)
{
    auto testCase = GetParam();
    std::string input = std::get<0>(testCase);
    std::string expected = std::get<1>(testCase);
    std::string actual = String::Trim(input);
    ASSERT_EQ(expected, actual);
}
