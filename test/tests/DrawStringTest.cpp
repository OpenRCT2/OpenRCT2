/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/platform/Platform.h>

TEST(DrawStringTests, basic)
{
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;

    Platform::CoreInit();

    auto context = OpenRCT2::CreateContext();
    bool initialised = context->Initialise();
    ASSERT_TRUE(initialised);

    int32_t x{};
    int32_t y{};
    int32_t width{};
    int32_t height{};
    ZoomLevel zoom_level{};

    rct_drawpixelinfo dpi{};
    dpi.x = x;
    dpi.y = y;
    dpi.width = width;
    dpi.height = height;
    dpi.pitch = (dpi.width + 31) % 32;
    dpi.zoom_level = zoom_level;

    std::unique_ptr<uint8_t[]> bits = std::make_unique<uint8_t[]>(dpi.height * (dpi.width + dpi.pitch));
    dpi.bits = bits.get();
    int32_t colour{};
    const ScreenCoordsXY coords{};
    bool noFormatting{};
    FontSpriteBase fontSpriteBase{};
    const_utf8string text = "Hello world!";
    ttf_draw_string(&dpi, text, colour, coords, noFormatting, fontSpriteBase, context.get());
}
