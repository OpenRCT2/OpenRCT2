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

#define gRideEntries RCT2_ADDRESS(0x009ACFA4, rct_ride_entry*)
#define gSupportSegments RCT2_ADDRESS(0x0141E9B4, support_height)
#define gWoodenSupportsPrependTo RCT2_GLOBAL(0x009DEA58, paint_struct*)
#define g_currently_drawn_item RCT2_GLOBAL(0x009DE578, void*)
#define gPaintInteractionType RCT2_GLOBAL(0x009DE570, uint8_t)
#define gSupportSegments RCT2_ADDRESS(0x0141E9B4, support_height)
#define gSupport RCT2_GLOBAL(0x0141E9D8, support_height)
#define gWoodenSupportsPrependTo RCT2_GLOBAL(0x009DEA58, paint_struct*)
#define gPaintMapPosition RCT2_GLOBAL(0x009DE574, LocationXY16)
#define gLeftTunnels RCT2_ADDRESS(0x009E3138, tunnel_entry)
#define gLeftTunnelCount RCT2_GLOBAL(0x0141F56A, uint8_t)
#define gRightTunnels RCT2_ADDRESS(0x009E30B6, tunnel_entry)
#define gRightTunnelCount RCT2_GLOBAL(0x0141F56B, uint8_t)
#define gVerticalTunnelHeight RCT2_GLOBAL(0x009E323C, uint8_t)
#define gSurfaceElement RCT2_GLOBAL(0x009E3250, TileElement*)
#define gDidPassSurface RCT2_GLOBAL(0x009DE57C, bool)
#define g141E9DB RCT2_GLOBAL(0x0141E9DB, uint8_t)
#define gTrackColours RCT2_ADDRESS(0x00F44198, uint32_t)
#define RCT2_CurrentViewportFlags RCT2_GLOBAL(0x0141E9E4, uint32_t)
#define RCT2_CurrentRotation RCT2_GLOBAL(0x0141E9E0, uint8_t)
#define RCT2_ScenarioTicks RCT2_GLOBAL(0x00F663AC, uint32_t)
#define RCT2_Rides RCT2_ADDRESS(0x013628F8, rct2_ride)
#define RCT2_Unk140E9A8 RCT2_GLOBAL(0x0140E9A8, rct_drawpixelinfo*)
#define RCT2_PaintBoundBoxOffsetX RCT2_GLOBAL(0x009DEA52, int16_t)
#define RCT2_PaintBoundBoxOffsetY RCT2_GLOBAL(0x009DEA54, int16_t)
#define RCT2_PaintBoundBoxOffsetZ RCT2_GLOBAL(0x009DEA56, int16_t)

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

extern Ride gRideList[RCT12_MAX_RIDES_IN_PARK];

int generatePaintCode(uint8_t rideType);
