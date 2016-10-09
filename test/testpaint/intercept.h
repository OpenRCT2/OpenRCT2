#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#ifndef _TEST_PAINT_INTERCEPT_H_
#define _TEST_PAINT_INTERCEPT_H_

#include "../../src/common.h"

#ifdef __cplusplus
extern "C"
{
#endif
    #include "../../src/interface/colour.h"
    #include "../../src/paint/paint.h"
    #include "../../src/paint/map_element/map_element.h"
#ifdef __cplusplus
}
#endif

#define gRideEntries                RCT2_ADDRESS(RCT2_ADDRESS_RIDE_ENTRIES,                rct_ride_entry*)
#define gCurrentRotation        RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8)

enum
{
    PAINT_98196C,
    PAINT_98197C,
    PAINT_98198C,
    PAINT_98199C,

    SUPPORTS_METAL_A,
    SUPPORTS_METAL_B,
    SUPPORTS_WOOD_A,
    SUPPORTS_WOOD_B,
};

typedef struct
{
    uint8 function;
    struct paint
    {
        uint32 image_id;
        rct_xy16 offset;
        rct_xyz16 bound_box_length;
        sint16 z_offset;
        rct_xyz16 bound_box_offset;
        uint32 rotation;
    } paint;
    struct supports
    {
        int type;
        uint8 segment;
        int special;
        int height;
        uint32 colour_flags;
    } supports;
} function_call;

#ifdef __cplusplus
extern "C"
{
#endif
    bool testRide(int rideType);
    void initHooks();
    int getTrackSequenceCount(uint8 rideType, uint8 trackType);
    bool rideIsImplemented(int rideType);
    bool rideSupportsTrackType(int rideType, int trackType);
    bool testTrackPainting(int rideType, int trackType);
    bool testSupportSegments(uint8 rideType, uint8 trackType);
    bool testTunnels(uint8 rideType, uint8 trackType);
    bool testVerticalTunnels(uint8 rideType, uint8 trackType);
    void intercept_clear_calls();
    int intercept_get_calls(function_call * buffer);
    bool assertFunctionCallEquals(function_call expected, function_call actual);

    int generatePaintCode(uint8 rideType);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <vector>

namespace Intercept2
{
    static const uint32 DEFAULT_SCHEME_TRACK = COLOUR_GREY << 19 | COLOUR_WHITE << 24 | 0xA0000000;
    static const uint32 DEFAULT_SCHEME_SUPPORTS = COLOUR_LIGHT_BLUE << 19 | COLOUR_ICY_BLUE << 24 | 0xA0000000;
    static const uint32 DEFAULT_SCHEME_MISC = COLOUR_DARK_PURPLE << 19 | COLOUR_LIGHT_PURPLE << 24 | 0xA0000000;
    static const uint32 DEFAULT_SCHEME_3 = COLOUR_BRIGHT_PURPLE << 19 | COLOUR_DARK_BLUE << 24 | 0xA0000000;

    struct SegmentSupportCall
    {
        uint16 segments;
        sint32 height;
        sint16 slope;
    };

    struct SupportCall
    {
        sint32 height;
        sint16 slope;
    };

    enum {
        TUNNELCALL_SKIPPED,
        TUNNELCALL_NONE,
        TUNNELCALL_CALL,
    };

    struct TunnelCall {
        uint8 call;
        sint16 offset;
        uint8 type;
    };

    sint16 getTunnelOffset(uint32 baseHeight, tunnel_entry calls[3]);
    std::vector<SegmentSupportCall> getSegmentCalls(support_height supports[9], uint8 rotation);
}

#endif

#endif // #endif _TEST_PAINT_INTERCEPT_H_
