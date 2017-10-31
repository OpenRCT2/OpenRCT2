#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <vector>

#include <openrct2/common.h>

extern "C"
{
    #include <openrct2/drawing/drawing.h>
    #include <openrct2/interface/colour.h>
    #include <openrct2/paint/paint.h>
    #include <openrct2/paint/tile_element/tile_element.h>
}

#define gRideEntries            RCT2_ADDRESS(RCT2_ADDRESS_RIDE_ENTRIES, rct_ride_entry*)
#define gCurrentRotation        RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8)

enum {
    TEST_SUCCESS,
    TEST_FAILED,
    TEST_SKIPPED,
};

namespace TestPaint
{
    static const uint32 DEFAULT_SCHEME_TRACK = SPRITE_ID_PALETTE_COLOUR_2(COLOUR_GREY, COLOUR_WHITE);
    static const uint32 DEFAULT_SCHEME_SUPPORTS = SPRITE_ID_PALETTE_COLOUR_2(COLOUR_LIGHT_BLUE, COLOUR_ICY_BLUE);
    static const uint32 DEFAULT_SCHEME_MISC = SPRITE_ID_PALETTE_COLOUR_2(COLOUR_DARK_PURPLE, COLOUR_LIGHT_PURPLE);
    static const uint32 DEFAULT_SCHEME_3 = SPRITE_ID_PALETTE_COLOUR_2(COLOUR_BRIGHT_PURPLE, COLOUR_DARK_BLUE);

    void ResetEnvironment();
    void ResetTunnels();
    void ResetSupportHeights();
}

enum Verbosity {
    QUIET,
    NORMAL,
};

extern "C"
{
int generatePaintCode(uint8 rideType);
}
