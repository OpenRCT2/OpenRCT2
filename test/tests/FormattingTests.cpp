/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "openrct2/localisation/Formatting.h"

#include <gtest/gtest.h>
#include <memory>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/StringIds.h>
#include <sstream>
#include <string>

using namespace OpenRCT2;

class FmtStringTests : public testing::Test
{
};

TEST_F(FmtStringTests, string_owned)
{
    auto fmt = FmtString(std::string("{BLACK}Guests: {INT32}"));
    ASSERT_EQ("Guests: ", fmt.WithoutFormatTokens());
}

TEST_F(FmtStringTests, iteration)
{
    std::string actual;

    auto fmt = FmtString("{BLACK}Guests: {INT32}");
    for (const auto& t : fmt)
    {
        actual += String::StdFormat("[%d:%s]", t.kind, std::string(t.text).c_str());
    }

    ASSERT_EQ("[29:{BLACK}][1:Guests: ][8:{INT32}]", actual);
}

TEST_F(FmtStringTests, iteration_escaped)
{
    std::string actual;

    auto fmt = FmtString("This is an {{ESCAPED}} string.");
    for (const auto& t : fmt)
    {
        actual += String::StdFormat("[%d:%s]", t.kind, std::string(t.text).c_str());
    }

    ASSERT_EQ("[1:This is an ][2:{{][1:ESCAPED][2:}}][1: string.]", actual);
}

TEST_F(FmtStringTests, without_format_tokens)
{
    auto fmt = FmtString("{BLACK}Guests: {INT32}");
    ASSERT_EQ("Guests: ", fmt.WithoutFormatTokens());
}

class FormattingTests : public testing::Test
{
private:
    static std::shared_ptr<IContext> _context;

protected:
    static void SetUpTestCase()
    {
        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;
        _context = CreateContext();
        ASSERT_TRUE(_context->Initialise());

        LanguageOpen(LANGUAGE_ENGLISH_UK);
    }

    static void TearDownTestCase()
    {
        _context = {};
    }
};

std::shared_ptr<IContext> FormattingTests::_context;

TEST_F(FormattingTests, no_args)
{
    auto actual = FormatString("test string");
    ASSERT_EQ("test string", actual);
}

TEST_F(FormattingTests, missing_arg)
{
    auto actual = FormatString("test {STRING} arg");
    ASSERT_EQ("test  arg", actual);
}

TEST_F(FormattingTests, integer)
{
    auto actual = FormatString("Guests: {INT32}", 32);
    ASSERT_EQ("Guests: 32", actual);
}

TEST_F(FormattingTests, integer_integer)
{
    auto actual = FormatString("Guests: {INT32}, Staff: {INT32}", 32, 10);
    ASSERT_EQ("Guests: 32, Staff: 10", actual);
}

TEST_F(FormattingTests, comma)
{
    auto actual = FormatString("Guests: {COMMA16}", 12534);
    ASSERT_EQ("Guests: 12,534", actual);
}

TEST_F(FormattingTests, comma_0)
{
    auto actual = FormatString("Guests: {COMMA16}", 0);
    ASSERT_EQ("Guests: 0", actual);
}

TEST_F(FormattingTests, comma_large_negative)
{
    auto actual = FormatString("{COMMA16}", std::numeric_limits<int64_t>::min());
    ASSERT_EQ("-9,223,372,036,854,775,808", actual);
}

TEST_F(FormattingTests, comma_large)
{
    auto actual = FormatString("{COMMA16}", std::numeric_limits<uint64_t>::max());
    ASSERT_EQ("18,446,744,073,709,551,615", actual);
}

TEST_F(FormattingTests, currency)
{
    gConfigGeneral.CurrencyFormat = CurrencyType::Pounds;
    ASSERT_EQ(u8"-£251", FormatString("{CURRENCY}", -2510));
    ASSERT_EQ(u8"£1", FormatString("{CURRENCY}", 4));
    ASSERT_EQ(u8"£1", FormatString("{CURRENCY}", 5));
    ASSERT_EQ(u8"£1", FormatString("{CURRENCY}", 10));
    ASSERT_EQ(u8"£2", FormatString("{CURRENCY}", 11));
    ASSERT_EQ(u8"£112", FormatString("{CURRENCY}", 1111));
}

TEST_F(FormattingTests, currency2dp)
{
    gConfigGeneral.CurrencyFormat = CurrencyType::Pounds;
    ASSERT_EQ(u8"-£251.00", FormatString("{CURRENCY2DP}", -2510));
    ASSERT_EQ(u8"£0.40", FormatString("{CURRENCY2DP}", 4));
    ASSERT_EQ(u8"£0.50", FormatString("{CURRENCY2DP}", 5));
    ASSERT_EQ(u8"£1.00", FormatString("{CURRENCY2DP}", 10));
    ASSERT_EQ(u8"£1.10", FormatString("{CURRENCY2DP}", 11));
    ASSERT_EQ(u8"£111.10", FormatString("{CURRENCY2DP}", 1111));
}

TEST_F(FormattingTests, currency_yen)
{
    gConfigGeneral.CurrencyFormat = CurrencyType::Yen;
    ASSERT_EQ(u8"-¥25,100", FormatString("{CURRENCY}", -2510));
    ASSERT_EQ(u8"¥40", FormatString("{CURRENCY2DP}", 4));
    ASSERT_EQ(u8"¥50", FormatString("{CURRENCY2DP}", 5));
    ASSERT_EQ(u8"¥100", FormatString("{CURRENCY2DP}", 10));
    ASSERT_EQ(u8"¥110", FormatString("{CURRENCY2DP}", 11));
    ASSERT_EQ(u8"¥11,110", FormatString("{CURRENCY2DP}", 1111));
}

TEST_F(FormattingTests, currency2dp_yen)
{
    gConfigGeneral.CurrencyFormat = CurrencyType::Yen;
    ASSERT_EQ(u8"-¥25,100", FormatString("{CURRENCY2DP}", -2510));
    ASSERT_EQ(u8"¥40", FormatString("{CURRENCY2DP}", 4));
    ASSERT_EQ(u8"¥50", FormatString("{CURRENCY2DP}", 5));
    ASSERT_EQ(u8"¥100", FormatString("{CURRENCY2DP}", 10));
    ASSERT_EQ(u8"¥110", FormatString("{CURRENCY2DP}", 11));
    ASSERT_EQ(u8"¥11,110", FormatString("{CURRENCY2DP}", 1111));
}

TEST_F(FormattingTests, currency_pts)
{
    gConfigGeneral.CurrencyFormat = CurrencyType::Peseta;
    ASSERT_EQ("-251Pts", FormatString("{CURRENCY}", -2510));
    ASSERT_EQ("112Pts", FormatString("{CURRENCY}", 1111));
}

TEST_F(FormattingTests, currency2dp_pts)
{
    gConfigGeneral.CurrencyFormat = CurrencyType::Peseta;
    ASSERT_EQ("-251.00Pts", FormatString("{CURRENCY2DP}", -2510));
    ASSERT_EQ("0.40Pts", FormatString("{CURRENCY2DP}", 4));
    ASSERT_EQ("111.10Pts", FormatString("{CURRENCY2DP}", 1111));
}

TEST_F(FormattingTests, string)
{
    auto actual = FormatString("{RED}{STRING} has broken down.", "Woodchip");
    ASSERT_EQ("{RED}Woodchip has broken down.", actual);
}

TEST_F(FormattingTests, escaped_braces)
{
    auto actual = FormatString("--{{ESCAPED}}--", 0);
    ASSERT_EQ("--{{ESCAPED}}--", actual);
}

TEST_F(FormattingTests, velocity_mph)
{
    gConfigGeneral.MeasurementFormat = MeasurementFormat::Imperial;
    auto actual = FormatString("Train is going at {VELOCITY}.", 1024);
    ASSERT_EQ("Train is going at 1,024 mph.", actual);
}

TEST_F(FormattingTests, velocity_kph)
{
    gConfigGeneral.MeasurementFormat = MeasurementFormat::Metric;
    auto actual = FormatString("Train is going at {VELOCITY}.", 1024);
    ASSERT_EQ("Train is going at 1,648 km/h.", actual);
}

TEST_F(FormattingTests, velocity_mps)
{
    gConfigGeneral.MeasurementFormat = MeasurementFormat::SI;
    auto actual = FormatString("Train is going at {VELOCITY}.", 1024);
    ASSERT_EQ("Train is going at 457.7 m/s.", actual);
}

TEST_F(FormattingTests, length_imperial)
{
    gConfigGeneral.MeasurementFormat = MeasurementFormat::Imperial;
    auto actual = FormatString("Height: {LENGTH}", 1024);
    ASSERT_EQ("Height: 3,360 ft", actual);
}

TEST_F(FormattingTests, length_metric)
{
    gConfigGeneral.MeasurementFormat = MeasurementFormat::Metric;
    auto actual = FormatString("Height: {LENGTH}", 1024);
    ASSERT_EQ("Height: 1,024 m", actual);
}

TEST_F(FormattingTests, length_si)
{
    gConfigGeneral.MeasurementFormat = MeasurementFormat::SI;
    auto actual = FormatString("Height: {LENGTH}", 2048);
    ASSERT_EQ("Height: 2,048 m", actual);
}

TEST_F(FormattingTests, minssecs)
{
    ASSERT_EQ("0secs", FormatString("{DURATION}", 0));
    ASSERT_EQ("1sec", FormatString("{DURATION}", 1));
    ASSERT_EQ("4secs", FormatString("{DURATION}", 4));
    ASSERT_EQ("1min:0secs", FormatString("{DURATION}", 60));
    ASSERT_EQ("1min:1sec", FormatString("{DURATION}", 60 + 1));
    ASSERT_EQ("1min:59secs", FormatString("{DURATION}", 60 + 59));
    ASSERT_EQ("2mins:0secs", FormatString("{DURATION}", 120));
    ASSERT_EQ("2mins:1sec", FormatString("{DURATION}", 120 + 1));
    ASSERT_EQ("2mins:2secs", FormatString("{DURATION}", 120 + 2));
}

TEST_F(FormattingTests, hoursmins)
{
    ASSERT_EQ("0mins", FormatString("{REALTIME}", 0));
    ASSERT_EQ("1min", FormatString("{REALTIME}", 1));
    ASSERT_EQ("4mins", FormatString("{REALTIME}", 4));
    ASSERT_EQ("1hour:0mins", FormatString("{REALTIME}", 60));
    ASSERT_EQ("1hour:1min", FormatString("{REALTIME}", 60 + 1));
    ASSERT_EQ("1hour:59mins", FormatString("{REALTIME}", 60 + 59));
    ASSERT_EQ("2hours:0mins", FormatString("{REALTIME}", 120));
    ASSERT_EQ("2hours:1min", FormatString("{REALTIME}", 120 + 1));
    ASSERT_EQ("2hours:2mins", FormatString("{REALTIME}", 120 + 2));
}

TEST_F(FormattingTests, month)
{
    ASSERT_EQ("The month is March", FormatString("The month is {MONTH}", 0));
    ASSERT_EQ("The month is October", FormatString("The month is {MONTH}", 7));
    ASSERT_EQ("The month is April", FormatString("The month is {MONTH}", 9));
}

TEST_F(FormattingTests, monthyear)
{
    ASSERT_EQ("Date: March, Year 1", FormatString("Date: {MONTHYEAR}", 0));
    ASSERT_EQ("Date: October, Year 1", FormatString("Date: {MONTHYEAR}", 7));
    ASSERT_EQ("Date: April, Year 2", FormatString("Date: {MONTHYEAR}", 9));
}

TEST_F(FormattingTests, two_level_format)
{
    constexpr StringId strDefault = STR_RIDE_NAME_DEFAULT;
    constexpr StringId strBoatHire = STR_RIDE_NAME_BOAT_HIRE;
    auto actual = FormatString("Queuing for {STRINGID}", strDefault, strBoatHire, 2);
    ASSERT_EQ("Queuing for Boat Hire 2", actual);
}

TEST_F(FormattingTests, any_string_int_string)
{
    auto actual = FormatStringAny(
        "{RED}{STRING} {INT32} has broken down due to '{STRING}'.", { "Twist", 2, "Mechanical failure" });
    ASSERT_EQ("{RED}Twist 2 has broken down due to 'Mechanical failure'.", actual);
}

TEST_F(FormattingTests, any_two_level_format)
{
    constexpr StringId strDefault = STR_RIDE_NAME_DEFAULT;
    constexpr StringId strBoatHire = STR_RIDE_NAME_BOAT_HIRE;
    auto actual = FormatStringAny("Queuing for {STRINGID}", { strDefault, strBoatHire, 2 });
    ASSERT_EQ("Queuing for Boat Hire 2", actual);
}

TEST_F(FormattingTests, to_fixed_buffer)
{
    char buffer[16];
    std::memset(buffer, '\xFF', sizeof(buffer));
    auto len = FormatStringId(buffer, 8, STR_GUEST_X, 123);
    ASSERT_EQ(len, 9u);
    ASSERT_STREQ("Guest 1", buffer);

    // Ensure rest of the buffer was not overwritten
    for (size_t i = 8; i < sizeof(buffer); i++)
    {
        ASSERT_EQ('\xFF', buffer[i]);
    }
}

TEST_F(FormattingTests, using_legacy_buffer_args)
{
    auto ft = Formatter();
    ft.Add<StringId>(STR_RIDE_NAME_DEFAULT);
    ft.Add<StringId>(STR_RIDE_NAME_BOAT_HIRE);
    ft.Add<uint16_t>(2);

    char buffer[32]{};
    auto len = FormatStringLegacy(buffer, sizeof(buffer), STR_QUEUING_FOR, ft.Data());
    ASSERT_EQ(len, 23u);
    ASSERT_STREQ("Queuing for Boat Hire 2", buffer);
}

TEST_F(FormattingTests, format_number_basic)
{
    FormatBuffer ss;
    // test basic integral conversion
    FormatArgument<int32_t>(ss, FormatToken::UInt16, 123);
    ASSERT_STREQ("123", ss.data());
}

TEST_F(FormattingTests, format_number_basic_int32)
{
    FormatBuffer ss;
    // test that case fallthrough works
    FormatArgument<int32_t>(ss, FormatToken::Int32, 123);
    ASSERT_STREQ("123", ss.data());
}

TEST_F(FormattingTests, format_number_negative)
{
    FormatBuffer ss;
    // test negative conversion
    FormatArgument<int32_t>(ss, FormatToken::Int32, -123);
    ASSERT_STREQ("-123", ss.data());
}

TEST_F(FormattingTests, format_number_comma16_basic)
{
    FormatBuffer ss;
    // test separator formatter
    // test base case separator formatter
    FormatArgument<int32_t>(ss, FormatToken::Comma16, 123);
    ASSERT_STREQ("123", ss.data());
}

TEST_F(FormattingTests, format_number_comma16_negative)
{
    FormatBuffer ss;
    // test separator formatter
    // test base case separator formatter
    FormatArgument<int32_t>(ss, FormatToken::Comma16, -123);
    ASSERT_STREQ("-123", ss.data());
}

TEST_F(FormattingTests, format_number_comma16_large)
{
    FormatBuffer ss;
    // test larger value for separator formatter
    FormatArgument<int32_t>(ss, FormatToken::Comma16, 123456789);
    ASSERT_STREQ("123,456,789", ss.data());
}

TEST_F(FormattingTests, format_number_comma16_large_negative)
{
    FormatBuffer ss;
    // test larger value for separator formatter with negative
    FormatArgument<int32_t>(ss, FormatToken::Comma16, -123456789);
    ASSERT_STREQ("-123,456,789", ss.data());
}

TEST_F(FormattingTests, format_number_comma16_uneven)
{
    FormatBuffer ss;
    // test non-multiple of 3
    FormatArgument<int32_t>(ss, FormatToken::Comma16, 12345678);
    ASSERT_STREQ("12,345,678", ss.data());
}

TEST_F(FormattingTests, format_number_comma16_uneven_negative)
{
    FormatBuffer ss;
    // test non-multiple of 3 with negative
    FormatArgument<int32_t>(ss, FormatToken::Comma16, -12345678);
    ASSERT_STREQ("-12,345,678", ss.data());
}

TEST_F(FormattingTests, format_number_comma16_zero)
{
    FormatBuffer ss;
    // test zero
    FormatArgument<int32_t>(ss, FormatToken::Comma16, 0);
    ASSERT_STREQ("0", ss.data());
}

TEST_F(FormattingTests, format_number_comma1dp16_zero)
{
    FormatBuffer ss;
    // zero case
    FormatArgument<int32_t>(ss, FormatToken::Comma1dp16, 0);
    ASSERT_STREQ("0.0", ss.data());
}

TEST_F(FormattingTests, format_number_comma1dp16_leading_zero)
{
    FormatBuffer ss;
    // test leading zero
    FormatArgument<int32_t>(ss, FormatToken::Comma1dp16, 5);
    ASSERT_STREQ("0.5", ss.data());
}

TEST_F(FormattingTests, format_number_comma1dp16_leading_zero_negative)
{
    FormatBuffer ss;
    // test leading zero with negative value
    FormatArgument<int32_t>(ss, FormatToken::Comma1dp16, -5);
    ASSERT_STREQ("-0.5", ss.data());
}

TEST_F(FormattingTests, format_number_comma1dp16_small_value)
{
    FormatBuffer ss;
    // test small value
    FormatArgument<int32_t>(ss, FormatToken::Comma1dp16, 75);
    ASSERT_STREQ("7.5", ss.data());
}

TEST_F(FormattingTests, format_number_comma1dp16_small_value_negative)
{
    FormatBuffer ss;
    // test small value with negative
    FormatArgument<int32_t>(ss, FormatToken::Comma1dp16, -75);
    ASSERT_STREQ("-7.5", ss.data());
}

TEST_F(FormattingTests, format_number_comma1dp16_trailing_zeros)
{
    FormatBuffer ss;
    // test value with trailing zero, no commas
    FormatArgument<int32_t>(ss, FormatToken::Comma1dp16, 1000);
    ASSERT_STREQ("100.0", ss.data());
}

TEST_F(FormattingTests, format_number_comma1dp16_trailing_zeros_negative)
{
    FormatBuffer ss;
    // test value with trailing zero, no commas
    FormatArgument<int32_t>(ss, FormatToken::Comma1dp16, -1000);
    ASSERT_STREQ("-100.0", ss.data());
}

TEST_F(FormattingTests, format_number_comma1dp16_large_trailing_zeros)
{
    FormatBuffer ss;
    // test value with commas and trailing zeros
    FormatArgument<int32_t>(ss, FormatToken::Comma1dp16, 10000000);
    ASSERT_STREQ("1,000,000.0", ss.data());
}

TEST_F(FormattingTests, format_number_comma1dp16_large_trailing_zeros_negative)
{
    FormatBuffer ss;
    // test value with commas and trailing zeros
    FormatArgument<int32_t>(ss, FormatToken::Comma1dp16, -10000000);
    ASSERT_STREQ("-1,000,000.0", ss.data());
}

TEST_F(FormattingTests, format_number_comma1dp16_large_value)
{
    FormatBuffer ss;
    // test large value
    FormatArgument<int32_t>(ss, FormatToken::Comma1dp16, 123456789);
    ASSERT_STREQ("12,345,678.9", ss.data());
}

TEST_F(FormattingTests, format_number_comma1dp16_large_value_negative)
{
    FormatBuffer ss;
    // test large value
    FormatArgument<int32_t>(ss, FormatToken::Comma1dp16, -123456789);
    ASSERT_STREQ("-12,345,678.9", ss.data());
}

TEST_F(FormattingTests, format_number_comma2dp32_zero)
{
    FormatBuffer ss;
    // zero case
    FormatArgument<int32_t>(ss, FormatToken::Comma2dp32, 0);
    ASSERT_STREQ("0.00", ss.data());
}

TEST_F(FormattingTests, format_number_comma2dp32_less_sig_figs)
{
    FormatBuffer ss;
    // test leading zero
    FormatArgument<int32_t>(ss, FormatToken::Comma2dp32, 5);
    ASSERT_STREQ("0.05", ss.data());
}

TEST_F(FormattingTests, format_number_comma2dp32_less_sig_figs_negative)
{
    FormatBuffer ss;
    // test leading zero
    FormatArgument<int32_t>(ss, FormatToken::Comma2dp32, -5);
    ASSERT_STREQ("-0.05", ss.data());
}

TEST_F(FormattingTests, format_number_comma2dp32_leading_zero)
{
    FormatBuffer ss;
    // test small value
    FormatArgument<int32_t>(ss, FormatToken::Comma2dp32, 75);
    ASSERT_STREQ("0.75", ss.data());
}

TEST_F(FormattingTests, format_number_comma2dp32_leading_zero_negative)
{
    FormatBuffer ss;
    // test small value
    FormatArgument<int32_t>(ss, FormatToken::Comma2dp32, -75);
    ASSERT_STREQ("-0.75", ss.data());
}

TEST_F(FormattingTests, format_number_comma2dp32_trailing_zeros)
{
    FormatBuffer ss;
    // test value with trailing zero, no commas
    FormatArgument<int32_t>(ss, FormatToken::Comma2dp32, 1000);
    ASSERT_STREQ("10.00", ss.data());
}

TEST_F(FormattingTests, format_number_comma2dp32_trailing_zeros_negative)
{
    FormatBuffer ss;
    // test value with trailing zero, no commas
    FormatArgument<int32_t>(ss, FormatToken::Comma2dp32, -1000);
    ASSERT_STREQ("-10.00", ss.data());
}

TEST_F(FormattingTests, format_number_comma2dp32_large_trailing_zeros)
{
    FormatBuffer ss;
    // test value with commas and trailing zeros
    FormatArgument<int32_t>(ss, FormatToken::Comma2dp32, 10000000);
    ASSERT_STREQ("100,000.00", ss.data());
}

TEST_F(FormattingTests, format_number_comma2dp32_large_trailing_zeros_negative)
{
    FormatBuffer ss;
    // test value with commas and trailing zeros
    FormatArgument<int32_t>(ss, FormatToken::Comma2dp32, -10000000);
    ASSERT_STREQ("-100,000.00", ss.data());
}

TEST_F(FormattingTests, format_number_comma2dp32_large_value)
{
    FormatBuffer ss;
    // test large value
    FormatArgument<int32_t>(ss, FormatToken::Comma2dp32, 123456789);
    ASSERT_STREQ("1,234,567.89", ss.data());
}

TEST_F(FormattingTests, format_number_comma2dp32_large_value_negative)
{
    FormatBuffer ss;
    // test large value
    FormatArgument<int32_t>(ss, FormatToken::Comma2dp32, -123456789);
    ASSERT_STREQ("-1,234,567.89", ss.data());

    // extra note:
    // for some reason the FormatArgument function contains constexpr
    // checks for std::is_floating_point<T>, even though a template
    // specialization for which that would ever be the case is never
    // declared. As such the following line won't be able to find
    // the necessary symbol to link with.
    // FormatArgument<double>(ss, FormatToken::Comma1dp16, 12.372);
}

TEST_F(FormattingTests, buffer_storage_swap)
{
    FormatBufferBase<char, 16> ss;
    ss << "Hello World";
    ASSERT_STREQ(ss.data(), "Hello World");
    ss << ", Exceeding local storage";
    ASSERT_STREQ(ss.data(), "Hello World, Exceeding local storage");
    ss << ", extended";
    ASSERT_STREQ(ss.data(), "Hello World, Exceeding local storage, extended");
}
