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

#include <algorithm>
#include <vector>

#include "GeneralSupportHeightCall.hpp"
#include "Printer.hpp"
#include "SegmentSupportHeightCall.hpp"
#include "TestPaint.hpp"
#include "Utils.hpp"

extern "C" {
    #include <openrct2/paint/paint.h>
    #include <openrct2/paint/supports.h>
    #include <openrct2/ride/track_data.h>
    #include <openrct2/interface/viewport.h>
    #include <openrct2/rct2/hook.h>
}

namespace TestPaint
{
    void ResetEnvironment() {
        gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
        gPaintSession.InteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;

        gTrackColours[SCHEME_TRACK] = DEFAULT_SCHEME_TRACK;
        gTrackColours[SCHEME_SUPPORTS] = DEFAULT_SCHEME_SUPPORTS;
        gTrackColours[SCHEME_MISC] = DEFAULT_SCHEME_MISC;
        gTrackColours[SCHEME_3] = DEFAULT_SCHEME_3;

        rct_drawpixelinfo dpi = { 0 };
        dpi.zoom_level = 1;
        unk_140E9A8 = &dpi;
        gPaintSession.Unk140E9A8 = &dpi;

        rct_ride ride = {0};
        ride.entrance_style = RIDE_ENTRANCE_STYLE_PLAIN;

        rct_ride_entry rideEntry = {0};
        rct_ride_entry_vehicle vehicleEntry { 0 };
        vehicleEntry.base_image_id = 0x70000;
        rideEntry.vehicles[0] = vehicleEntry;

        gRideList[0] = ride;
        gRideEntries[0] = &rideEntry;

        g141E9DB = G141E9DB_FLAG_1 | G141E9DB_FLAG_2;
        gPaintSession.Unk141E9DB = G141E9DB_FLAG_1 | G141E9DB_FLAG_2;
    }

    void ResetTunnels() {
        gLeftTunnelCount = 0;
        gRightTunnelCount = 0;
        gPaintSession.LeftTunnelCount = 0;
        gPaintSession.RightTunnelCount = 0;

        for (int i = 0; i < TUNNEL_MAX_COUNT; i++) {
            gLeftTunnels[i].height = 0;
            gLeftTunnels[i].type = 0;
            gRightTunnels[i].height = 0;
            gRightTunnels[i].type = 0;
            gPaintSession.LeftTunnels[i].height = 0;
            gPaintSession.LeftTunnels[i].type = 0;
            gPaintSession.RightTunnels[i].height = 0;
            gPaintSession.RightTunnels[i].type = 0;
        }

        gLeftTunnels[0].height = 0xFF;
        gLeftTunnels[0].type = 0xFF;
        gRightTunnels[0].height = 0xFF;
        gRightTunnels[0].type = 0xFF;
        gPaintSession.LeftTunnels[0].height = 0xFF;
        gPaintSession.LeftTunnels[0].type = 0xFF;
        gPaintSession.RightTunnels[0].height = 0xFF;
        gPaintSession.RightTunnels[0].type = 0xFF;
    }

    void ResetSupportHeights() {
        for (int s = 0; s < 9; ++s)
        {
            gSupportSegments[s].height = 0;
            gSupportSegments[s].slope = 0xFF;
            gPaintSession.SupportSegments[s].height = 0;
            gPaintSession.SupportSegments[s].slope = 0xFF;
        }

        gSupport.height = 0;
        gSupport.slope = 0xFF;
        gPaintSession.Support.height = 0;
        gPaintSession.Support.slope = 0xFF;
    }

    struct IgnoredEntry
    {
        uint8 Direction;
        uint8 TrackSequence;
    };

    static bool _ignoredAll;
    static std::vector<IgnoredEntry> _ignoredEntries;

    static void testClearIgnore()
    {
        _ignoredAll = false;
        _ignoredEntries.clear();
    }

    static void testIgnore(uint8 direction, uint8 trackSequence)
    {
        _ignoredEntries.push_back({ direction, trackSequence });
    }

    static void testIgnoreAll()
    {
        _ignoredAll = true;
    }

    static bool testIsIgnored(uint8 direction, uint8 trackSequence)
    {
        if (_ignoredAll) return true;
        for (const IgnoredEntry &entry : _ignoredEntries)
        {
            if (entry.Direction == direction &&
                entry.TrackSequence == trackSequence)
            {
                return true;
            }
        }
        return false;
    }
}

extern "C"
{
    void testpaint_clear_ignore()
    {
        TestPaint::testClearIgnore();
    }

    void testpaint_ignore(uint8 direction, uint8 trackSequence)
    {
        TestPaint::testIgnore(direction, trackSequence);
    }

    void testpaint_ignore_all()
    {
        TestPaint::testIgnoreAll();
    }

    bool testpaint_is_ignored(uint8 direction, uint8 trackSequence)
    {
        return TestPaint::testIsIgnored(direction, trackSequence);
    }
}
