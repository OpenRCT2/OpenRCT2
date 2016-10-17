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

#include <vector>
#include <algorithm>

#include "intercept.h"
#include "GeneralSupportHeightCall.hpp"
#include "Printer.hpp"
#include "SegmentSupportHeightCall.hpp"
#include "Utils.hpp"

extern "C" {
    #include "../../src/paint/paint.h"
    #include "../../src/paint/supports.h"
    #include "../../src/ride/track_data.h"
    #include "../../src/interface/viewport.h"
    #include "../../src/hook.h"
}

namespace Intercept2
{
    void ResetEnvironment() {
        gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
        gTrackColours[SCHEME_TRACK] = DEFAULT_SCHEME_TRACK;
        gTrackColours[SCHEME_SUPPORTS] = DEFAULT_SCHEME_SUPPORTS;
        gTrackColours[SCHEME_MISC] = DEFAULT_SCHEME_MISC;
        gTrackColours[SCHEME_3] = DEFAULT_SCHEME_3;

        rct_drawpixelinfo dpi = { 0 };
        dpi.zoom_level = 1;
        unk_140E9A8 = &dpi;

        rct_ride ride = {0};
        ride.entrance_style = RIDE_ENTRANCE_STYLE_PLAIN;

        rct_ride_entry rideEntry = {0};
        rct_ride_entry_vehicle vehicleEntry { 0 };
        vehicleEntry.base_image_id = 0x70000;
        rideEntry.vehicles[0] = vehicleEntry;

        gRideList[0] = ride;
        gRideEntries[0] = &rideEntry;

        g141E9DB = G141E9DB_FLAG_1 | G141E9DB_FLAG_2;
    }

    void ResetTunnels() {
        gLeftTunnelCount = 0;
        gRightTunnelCount = 0;

        for (int i = 0; i < TUNNEL_MAX_COUNT; i++) {
            gLeftTunnels[i].height = 0;
            gLeftTunnels[i].type = 0;
            gRightTunnels[i].height = 0;
            gRightTunnels[i].type = 0;
        }

        gLeftTunnels[0].height = 0xFF;
        gLeftTunnels[0].type = 0xFF;
        gRightTunnels[0].height = 0xFF;
        gRightTunnels[0].type = 0xFF;
    }

    void ResetSupportHeights() {
        for (int s = 0; s < 9; ++s)
        {
            gSupportSegments[s].height = 0;
            gSupportSegments[s].slope = 0xFF;
        }

        gSupport.height = 0;
        gSupport.slope = 0xFF;
    }

    static bool verticalTunnelHeightIsConsistent(uint8 heights[4])
    {
        for (int i = 1; i < 4; ++i) {
            if (heights[i] != heights[0]) return false;
        }

        return true;
    }

    static void printRelativeHeight(utf8string out, size_t len, sint16 height)
    {
        if (height == 0) {
            snprintf(out, len, "height");
            return;
        }

        if (height > 0) {
            snprintf(out, len, "height + %d", height);
            return;
        }

        if (height < 0) {
            snprintf(out, len, "height - %d", int(abs(height)));
            return;
        }
    }

    static bool testVerticalTunnels(uint8 rideType, uint8 trackType)
    {
        uint8 rideIndex = 0;
        rct_map_element mapElement = {0};
        mapElement.flags |= MAP_ELEMENT_FLAG_LAST_TILE;
        mapElement.properties.track.type = trackType;
        mapElement.base_height = 3;

        g_currently_drawn_item = &mapElement;

        ResetEnvironment();

        int height = 48;

        TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];
        int sequenceCount = Utils::getTrackSequenceCount(rideType, trackType);


        for (int trackSequence = 0; trackSequence < sequenceCount; trackSequence++) {
            uint8 verticalTunnelHeight[4];

            for (int direction = 0; direction < 4; direction++) {
                gVerticalTunnelHeight = 0;

                uint32 *trackDirectionList = (uint32 *)RideTypeTrackPaintFunctionsOld[rideType][trackType];

                // Have to call from this point as it pushes esi and expects callee to pop it
                RCT2_CALLPROC_X(
                    0x006C4934,
                    rideType,
                    (int) trackDirectionList,
                    direction,
                    height,
                    (int) &mapElement,
                    rideIndex * sizeof(rct_ride),
                    trackSequence
                );

                verticalTunnelHeight[direction] = gVerticalTunnelHeight;
            }

            if (!verticalTunnelHeightIsConsistent(verticalTunnelHeight)) {
                printf(
                    "Original vertical tunnel height is inconsistent, skipping test. [trackSequence:%d]\n",
                    trackSequence
                );
                continue;
            }

            uint8 referenceHeight = verticalTunnelHeight[0];
            for (int direction = 0; direction < 4; direction++) {
                gVerticalTunnelHeight = 0;

                testpaint_clear_ignore();
                TRACK_PAINT_FUNCTION newPaintFunction = newPaintGetter(trackType, direction);
                newPaintFunction(rideIndex, trackSequence, direction, height, &mapElement);
                if (testpaint_is_ignored(direction, trackSequence)) {
                    continue;
                }

                if (gVerticalTunnelHeight != referenceHeight) {
                    if (referenceHeight == 0) {
                        printf(
                            "Expected no vertical tunnel. [trackSequence:%d direction:%d]\n",
                            trackSequence,
                            direction
                        );

                        return false;
                    }

                    utf8string strExpectedTunnelHeight = new utf8[16];
                    utf8string strActualTunnelHeight = new utf8[16];
                    printRelativeHeight(strExpectedTunnelHeight, 16, (referenceHeight * 16) - 48);
                    printRelativeHeight(strActualTunnelHeight, 16, (gVerticalTunnelHeight * 16) - 48);

                    printf(
                        "Expected vertical tunnel height to be `%s`, was `%s`. [trackSequence:%d direction:%d]\n",
                        strExpectedTunnelHeight,
                        strActualTunnelHeight,
                        trackSequence,
                        direction
                    );

                    delete []strExpectedTunnelHeight;
                    delete []strActualTunnelHeight;

                    return false;
                }
            }
        }

        return true;
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
    bool testVerticalTunnels(uint8 rideType, uint8 trackType)
    {
        return Intercept2::testVerticalTunnels(rideType, trackType);
    }

    void testpaint_clear_ignore()
    {
        Intercept2::testClearIgnore();
    }

    void testpaint_ignore(uint8 direction, uint8 trackSequence)
    {
        Intercept2::testIgnore(direction, trackSequence);
    }

    void testpaint_ignore_all()
    {
        Intercept2::testIgnoreAll();
    }

    bool testpaint_is_ignored(uint8 direction, uint8 trackSequence)
    {
        return Intercept2::testIsIgnored(direction, trackSequence);
    }

    void intercept_reset_environment() {
        Intercept2::ResetEnvironment();
    }

    void intercept_reset_segment_heights() {
        Intercept2::ResetSupportHeights();
    }

    void intercept_reset_tunnels() {
        Intercept2::ResetTunnels();
    }


}
