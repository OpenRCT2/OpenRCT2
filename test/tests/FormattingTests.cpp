/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "openrct2/localisation/Formatting.h"

#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/config/Config.h>

using namespace OpenRCT2;

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
        bool initialised = _context->Initialise();
        ASSERT_TRUE(initialised);

        // load_from_sv6(parkPath.c_str());
        // game_load_init();

        // Changed in some tests. Store to restore its value
        // _gScreenFlags = gScreenFlags;
        SUCCEED();
    }

    static void TearDownTestCase()
    {
        if (_context)
            _context.reset();

        // gScreenFlags = _gScreenFlags;
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
    gConfigGeneral.measurement_format = MeasurementFormat::Imperial;
    auto actual = FormatString("Train is going at {VELOCITY}.", 1024);
    ASSERT_EQ("Train is going at 1,024 mph.", actual);
}

TEST_F(FormattingTests, velocity_kph)
{
    gConfigGeneral.measurement_format = MeasurementFormat::Metric;
    auto actual = FormatString("Train is going at {VELOCITY}.", 1024);
    ASSERT_EQ("Train is going at 1,648 km/h.", actual);
}

TEST_F(FormattingTests, velocity_mps)
{
    gConfigGeneral.measurement_format = MeasurementFormat::SI;
    auto actual = FormatString("Train is going at {VELOCITY}.", 1024);
    ASSERT_EQ("Train is going at 457.7 m/s.", actual);
}
