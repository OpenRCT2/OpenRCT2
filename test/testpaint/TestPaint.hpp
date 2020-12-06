/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Addresses.h"

#include <openrct2/common.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/paint/Paint.h>
#include <openrct2/paint/tile_element/Paint.TileElement.h>
#include <openrct2/rct12/RCT12.h>
#include <vector>

enum
{
    TEST_SUCCESS,
    TEST_FAILED,
    TEST_SKIPPED,
};

namespace TestPaint
{
    static const uint32_t DEFAULT_SCHEME_TRACK = SPRITE_ID_PALETTE_COLOUR_2(COLOUR_GREY, COLOUR_WHITE);
    static const uint32_t DEFAULT_SCHEME_SUPPORTS = SPRITE_ID_PALETTE_COLOUR_2(COLOUR_LIGHT_BLUE, COLOUR_ICY_BLUE);
    static const uint32_t DEFAULT_SCHEME_MISC = SPRITE_ID_PALETTE_COLOUR_2(COLOUR_DARK_PURPLE, COLOUR_LIGHT_PURPLE);
    static const uint32_t DEFAULT_SCHEME_3 = SPRITE_ID_PALETTE_COLOUR_2(COLOUR_BRIGHT_PURPLE, COLOUR_DARK_BLUE);

    void ResetEnvironment();
    void ResetTunnels();
    void ResetSupportHeights();

    void testClearIgnore();
    void testIgnore(uint8_t direction, uint8_t trackSequence);
    void testIgnoreAll();
    bool testIsIgnored(uint8_t direction, uint8_t trackSequence);
} // namespace TestPaint

enum Verbosity
{
    QUIET,
    NORMAL,
};

struct rct_ride_entry;
extern Ride gRideList[RCT12_MAX_RIDES_IN_PARK];
void set_ride_entry(rct_ride_entry* entry);

int generatePaintCode(uint8_t rideType);
