/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TestPaint.hpp"

#include "GeneralSupportHeightCall.hpp"
#include "Hook.h"
#include "Printer.hpp"
#include "SegmentSupportHeightCall.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <openrct2/interface/Viewport.h>
#include <openrct2/paint/Paint.h>
#include <openrct2/paint/Supports.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/scenario/Scenario.h>
#include <vector>

namespace TestPaint
{
    void ResetEnvironment()
    {
        gPaintInteractionType = EnumValue(ViewportInteractionItem::Ride);
        gPaintSession.InteractionType = ViewportInteractionItem::Ride;

        gTrackColours[SCHEME_TRACK] = DEFAULT_SCHEME_TRACK;
        gTrackColours[SCHEME_SUPPORTS] = DEFAULT_SCHEME_SUPPORTS;
        gTrackColours[SCHEME_MISC] = DEFAULT_SCHEME_MISC;
        gTrackColours[SCHEME_3] = DEFAULT_SCHEME_3;

        gPaintSession.TrackColours[SCHEME_TRACK] = DEFAULT_SCHEME_TRACK;
        gPaintSession.TrackColours[SCHEME_SUPPORTS] = DEFAULT_SCHEME_SUPPORTS;
        gPaintSession.TrackColours[SCHEME_MISC] = DEFAULT_SCHEME_MISC;
        gPaintSession.TrackColours[SCHEME_3] = DEFAULT_SCHEME_3;

        rct_drawpixelinfo dpi = {};
        dpi.zoom_level = 1;
        RCT2_Unk140E9A8 = &dpi;
        gPaintSession.DPI = dpi;

        {
            static rct_ride_entry rideEntry = {};
            rct_ride_entry_vehicle vehicleEntry{};
            vehicleEntry.base_image_id = 0x70000;
            rideEntry.vehicles[0] = vehicleEntry;
            gRideList[0] = {};
            gRideEntries[0] = &rideEntry;
        }
        {
            rct2_ride ride = {};
            ride.entrance_style = 0;
            RCT2_Rides[0] = ride;
        }

        g141E9DB = G141E9DB_FLAG_1 | G141E9DB_FLAG_2;
        gPaintSession.Unk141E9DB = G141E9DB_FLAG_1 | G141E9DB_FLAG_2;

        RCT2_CurrentViewportFlags = 0;

        gScenarioTicks = 0;
        RCT2_ScenarioTicks = 0;
    }

    void ResetTunnels()
    {
        gLeftTunnelCount = 0;
        gRightTunnelCount = 0;
        gPaintSession.LeftTunnelCount = 0;
        gPaintSession.RightTunnelCount = 0;

        for (int i = 0; i < TUNNEL_MAX_COUNT; i++)
        {
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

    void ResetSupportHeights()
    {
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
        uint8_t Direction;
        uint8_t TrackSequence;
    };

    static bool _ignoredAll;
    static std::vector<IgnoredEntry> _ignoredEntries;

    void testClearIgnore()
    {
        _ignoredAll = false;
        _ignoredEntries.clear();
    }

    void testIgnore(uint8_t direction, uint8_t trackSequence)
    {
        _ignoredEntries.push_back({ direction, trackSequence });
    }

    void testIgnoreAll()
    {
        _ignoredAll = true;
    }

    bool testIsIgnored(uint8_t direction, uint8_t trackSequence)
    {
        if (_ignoredAll)
            return true;
        for (const IgnoredEntry& entry : _ignoredEntries)
        {
            if (entry.Direction == direction && entry.TrackSequence == trackSequence)
            {
                return true;
            }
        }
        return false;
    }
} // namespace TestPaint

void testpaint_clear_ignore()
{
    TestPaint::testClearIgnore();
}

void testpaint_ignore(uint8_t direction, uint8_t trackSequence)
{
    TestPaint::testIgnore(direction, trackSequence);
}

void testpaint_ignore_all()
{
    TestPaint::testIgnoreAll();
}

bool testpaint_is_ignored(uint8_t direction, uint8_t trackSequence)
{
    return TestPaint::testIsIgnored(direction, trackSequence);
}
