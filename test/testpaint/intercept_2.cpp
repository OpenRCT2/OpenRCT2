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

extern "C" {
    #include "intercept.h"
    #include "../../src/paint/paint.h"
    #include "../../src/paint/supports.h"
    #include "../../src/ride/track_data.h"
    #include "../../src/interface/viewport.h"
    #include "../../src/hook.h"
}

namespace Intercept2
{

    static const uint32 PALETTE_98 = COLOUR_GREY << 19 | COLOUR_WHITE << 24 | 0xA0000000;
    static const uint32 PALETTE_9C = COLOUR_LIGHT_BLUE << 19 | COLOUR_ICY_BLUE << 24 | 0xA0000000;
    static const uint32 PALETTE_A0 = COLOUR_DARK_PURPLE << 19 | COLOUR_LIGHT_PURPLE << 24 | 0xA0000000;
    static const uint32 PALETTE_A4 = COLOUR_BRIGHT_PURPLE << 19 | COLOUR_DARK_BLUE << 24 | 0xA0000000;

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

    bool SortSegmentSupportCalls(SegmentSupportCall lhs, SegmentSupportCall rhs)
    {
        if (lhs.height != rhs.height) {
            return lhs.height < rhs.height;
        }

        if (lhs.slope != rhs.slope) {
            return lhs.slope < rhs.slope;
        }

        return lhs.segments < rhs.segments;
    }

    static std::vector<SegmentSupportCall> getSegmentCalls(support_height supports[9], uint8 rotation)
    {
        uint16 positionsRemaining = SEGMENTS_ALL;

        for (int i = 0; i < 9; i++) {
            if (supports[i].height == 0 && supports[i].slope == 0xFF) {
                positionsRemaining &= ~segment_offsets[i];
            }
        }

        std::vector<SegmentSupportCall> calls;

        while (positionsRemaining != 0) {
            SegmentSupportCall call = {0};
            call.height = -1;
            call.slope = -1;

            support_height referenceSupport;

            for (int i = 0; i < 9; i++) {
                if (positionsRemaining & segment_offsets[i]) {
                    referenceSupport = supports[i];
                    if (supports[i].height != 0) {
                        call.height = supports[i].height;
                    }
                    if (supports[i].slope != 0xFF) {
                        call.slope = supports[i].slope;
                    }
                    break;
                }
            }

            uint16 positionsMatched = 0;
            for (int i = 0; i < 9; i++) {
                if (supports[i].height == referenceSupport.height && supports[i].slope == referenceSupport.slope) {
                    positionsMatched |= segment_offsets[i];
                }
            }
            positionsRemaining &= ~positionsMatched;

            call.segments = paint_util_rotate_segments(positionsMatched, (4 - rotation) % 4);

            calls.push_back(call);
        }

        if (calls.size() > 1) {
            std::sort(calls.begin(), calls.end(), SortSegmentSupportCalls);
        }

        return calls;
    }

    static bool SegmentCallEquals(std::vector<SegmentSupportCall> lhs, std::vector<SegmentSupportCall> rhs)
    {
        if (lhs.size() != rhs.size()) return false;
        for (int i = 0; i < lhs.size(); ++i) {
            if (lhs[i].segments != rhs[i].segments)
                return false;
            if (lhs[i].height != rhs[i].height)
                return false;
            if (lhs[i].slope != rhs[i].slope)
                return false;
        }

        return true;
    }

    static bool segmentCallsMatch(std::vector<SegmentSupportCall> tileSegmentSupportCalls[4])
    {
        std::vector<SegmentSupportCall> baseCallList = tileSegmentSupportCalls[0];
        for (int i = 1; i < 4; i++) {
            if (!SegmentCallEquals(baseCallList, tileSegmentSupportCalls[i])) {
                return false;
            }
        }

        return true;
    }

    static bool supportCallsMatch(SupportCall tileSupportCalls[4])
    {
        SupportCall baseCall = tileSupportCalls[0];
        for (int i = 1; i < 4; i++) {
            if (tileSupportCalls[i].height != baseCall.height) return false;
            if (tileSupportCalls[i].slope != baseCall.slope) return false;
        }

        return true;
    }

    static void printSegmentSupports(utf8string * out, std::vector<SegmentSupportCall> segmentCalls)
    {
        for (auto &&call : segmentCalls) {
            int segmentsPrinted = 0;
            for (int i = 0; i < 9; i++) {
                if (call.segments & segment_offsets[i]) {
                    if (segmentsPrinted > 0) {
                        sprintf(*out + strlen(*out), " | ");
                    }
                    sprintf(*out + strlen(*out), "SEGMENT_%02X", 0xB4 + 4 * i);
                    segmentsPrinted++;
                }
            }

            if (call.height == 0xFFFF) {
                sprintf(*out + strlen(*out), ", 0xFFFF");
            } else {
                sprintf(*out + strlen(*out), ", %d", call.height);
            }

            sprintf(*out + strlen(*out), ", 0x%02X\n", call.slope);
        }
    }

    static bool testSupportSegments(uint8 rideType, uint8 trackType)
    {
        uint8 rideIndex = 0;
        rct_map_element mapElement = {0};
        mapElement.properties.track.type = trackType;
        mapElement.base_height = 3;

        g_currently_drawn_item = &mapElement;

        rct_map_element surfaceElement = {0};
        surfaceElement.type = MAP_ELEMENT_TYPE_SURFACE;
        surfaceElement.base_height = 2;

        gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
        RCT2_GLOBAL(0x00F44198, uint32) = PALETTE_98;
        RCT2_GLOBAL(0x00F441A0, uint32) = PALETTE_A0;
        RCT2_GLOBAL(0x00F441A4, uint32) = PALETTE_A4;
        RCT2_GLOBAL(0x00F4419C, uint32) = PALETTE_9C;

        rct_drawpixelinfo dpi = {.zoom_level = 1};
        unk_140E9A8 = &dpi;

        rct_vehicle vehicle = {0};
        rct_ride ride = {0};

        rct_ride_entry rideEntry = {0};
        rct_ride_entry_vehicle vehicleEntry = {.base_image_id = 0x70000};
        rideEntry.vehicles[0] = vehicleEntry;


        gRideList[0] = ride;
        gRideEntries[0] = &rideEntry;

        int height = 48;

        TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];
        int sequenceCount = getTrackSequenceCount(rideType, trackType);


        for (int trackSequence = 0; trackSequence < sequenceCount; trackSequence++) {
            std::vector<SegmentSupportCall> tileSegmentSupportCalls[4];
            SupportCall tileGeneralSupportCalls[4];

            for (int direction = 0; direction < 4; direction++) {
                for (int s = 0; s < 9; ++s) {
                    gSupportSegments[s].height = 0;
                    gSupportSegments[s].slope = 0xFF;
                }

                gSupport.height = 0;
                gSupport.slope = 0xFF;

                TRACK_PAINT_FUNCTION ** trackTypeList = (TRACK_PAINT_FUNCTION **) RideTypeTrackPaintFunctionsOld[rideType];
                uint32 * trackDirectionList = (uint32 *) trackTypeList[trackType];

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

                tileSegmentSupportCalls[direction] = getSegmentCalls(gSupportSegments, direction);

                tileGeneralSupportCalls[direction].height = -1;
                tileGeneralSupportCalls[direction].slope = -1;
                if (gSupport.height != 0) {
                    tileGeneralSupportCalls[direction].height = gSupport.height;
                }
                if (gSupport.slope != 0xFF) {
                    tileGeneralSupportCalls[direction].height = gSupport.height;
                }
            }

            if (!segmentCallsMatch(tileSegmentSupportCalls)) {
                // TODO: if 3 directions do share the same mask, use that call list as a reference.
                printf("Original segment calls didn't match. [trackSequence:%d]\n", trackSequence);
                continue;
            }

            for (int direction = 0; direction < 4; direction++) {
                for (int s = 0; s < 9; ++s) {
                    gSupportSegments[s].height = 0;
                    gSupportSegments[s].slope = 0xFF;
                }

                TRACK_PAINT_FUNCTION newPaintFunction = newPaintGetter(trackType, direction);
                newPaintFunction(rideIndex, trackSequence, direction, height, &mapElement);

                std::vector<SegmentSupportCall> newCalls = getSegmentCalls(gSupportSegments, direction);

                if (!SegmentCallEquals(tileSegmentSupportCalls[0], newCalls)) {
                    // TODO put this into *error
                    utf8string diff = new utf8[2048];
                    sprintf(diff, "<<< EXPECTED\n");
                    printSegmentSupports(&diff, tileSegmentSupportCalls[0]);
                    sprintf(diff + strlen(diff), "====\n");
                    printSegmentSupports(&diff, newCalls);
                    sprintf(diff + strlen(diff), ">>> ACTUAL\n");

                    printf("Segment support heights didn't match. [direction:%d trackSequence:%d]\n", direction,
                           trackSequence);
                    printf("%s", diff);
                    delete[] diff;
                    return false;
                }
            }

            if (!supportCallsMatch(tileGeneralSupportCalls)) {
                // TODO: if 3 directions do share the output, use that.
                printf("Original support calls didn't match. [trackSequence:%d]\n", trackSequence);
                continue;
            }

            SupportCall referenceGeneralSupportCall = tileGeneralSupportCalls[0];


            for (int direction = 0; direction < 4; direction++) {
                gSupport.height = 0;
                gSupport.slope = 0xFF;

                TRACK_PAINT_FUNCTION newPaintFunction = newPaintGetter(trackType, direction);
                newPaintFunction(rideIndex, trackSequence, direction, height, &mapElement);

                if (referenceGeneralSupportCall.height != -1) {
                    if (gSupport.height != referenceGeneralSupportCall.height) {
                        printf("General support heights didn't match. [direction:%d trackSequence:%d]\n", direction,
                               trackSequence);
                        return false;
                    }
                }
                if (referenceGeneralSupportCall.slope != -1) {
                    if (gSupport.slope != referenceGeneralSupportCall.slope) {
                        printf("General support slopes didn't match. [direction:%d trackSequence:%d]\n", direction,
                               trackSequence);
                        return false;
                    }
                }
            }

        }

        return true;
    }
}


extern "C"
{
    bool testSupportSegments(uint8 rideType, uint8 trackType)
    {
        return Intercept2::testSupportSegments(rideType, trackType);
    }
}
