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

    static bool SortSegmentSupportCalls(SegmentSupportCall lhs, SegmentSupportCall rhs)
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
        for (size_t i = 0; i < lhs.size(); ++i) {
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

    static bool tunnelCallsLineUp(TunnelCall tunnelCalls[4][4])
    {
        for (int side = 0; side < 4; ++side) {
            TunnelCall * referenceCall = nullptr;
            for (int direction = 0; direction < 4; ++direction) {
                if (tunnelCalls[direction][side].call == TUNNELCALL_SKIPPED) {
                    continue;
                }

                if (referenceCall == nullptr) {
                    referenceCall = &tunnelCalls[direction][side];
                    continue;
                }

                if (referenceCall->call != tunnelCalls[direction][side].call) return false;

                if (referenceCall->call == TUNNELCALL_CALL) {
                    if (referenceCall->type != tunnelCalls[direction][side].type) return false;
                    if (referenceCall->offset != tunnelCalls[direction][side].offset) return false;
                }
            }
        }

        return true;
    }

    static void getTunnelCallReferencePattern(TunnelCall tunnelCalls[4][4], TunnelCall (*out)[4])
    {
        for (int side = 0; side < 4; ++side) {
            for (int direction = 0; direction < 4; ++direction) {
                if (tunnelCalls[direction][side].call == TUNNELCALL_SKIPPED) {
                    continue;
                }

                (*out)[side].call = tunnelCalls[direction][side].call;
                (*out)[side].type = tunnelCalls[direction][side].type;
                (*out)[side].offset = tunnelCalls[direction][side].offset;
            }
        }
    }

    static utf8string getTunnelEdgeString(TunnelCall edge)
    {
        utf8string out = new utf8[32];

        switch (edge.call) {
            case TUNNELCALL_SKIPPED:
                sprintf(out, "%s", "     ");
                break;

            case TUNNELCALL_NONE:
                sprintf(out, "%s", "  -  ");
                break;

            case TUNNELCALL_CALL:
                if (edge.offset == 0) {
                    sprintf(out, "  0/%X ", edge.type);
                } else {
                    utf8string offset = new utf8[16];
                    if (edge.offset < 0) {
                        sprintf(offset, "%d", edge.offset);
                    } else {
                        sprintf(offset, "+%d", edge.offset);
                    }

                    sprintf(out, "%3s/%X ", offset, edge.type);

                    delete[] offset;
                }
                break;
        }

        return out;
    }

    static void printTunnelCalls(TunnelCall tunnelCalls[4][4])
    {
        for (int direction = 0; direction < 4; ++direction) {
            printf("        +        ");
        }
        printf("\n");

        for (int direction = 0; direction < 4; ++direction) {
            utf8string tlEdge = getTunnelEdgeString(tunnelCalls[direction][2]);
            utf8string trEdge = getTunnelEdgeString(tunnelCalls[direction][3]);
            printf("   %s %s   ", tlEdge, trEdge);
            delete [] tlEdge;
            delete [] trEdge;
        }
        printf("\n");

        for (int direction = 0; direction < 4; ++direction) {
            printf("  +           +  ");
        }
        printf("\n");

        for (int direction = 0; direction < 4; ++direction) {
            utf8string brEdge = getTunnelEdgeString(tunnelCalls[direction][0]);
            utf8string blEdge = getTunnelEdgeString(tunnelCalls[direction][1]);
            printf("   %s %s   ", blEdge, brEdge);
            delete [] blEdge;
            delete [] brEdge;
        }
        printf("\n");

        for (int direction = 0; direction < 4; ++direction) {
            printf("        +        ");
        }
        printf("\n");

        for (int direction = 0; direction < 4; ++direction) {
            printf("   direction %d   ", direction);
        }
        printf("\n");
    }

    static bool testSupportSegments(uint8 rideType, uint8 trackType)
    {
        uint8 rideIndex = 0;
        rct_map_element mapElement = {0};
        mapElement.flags |= MAP_ELEMENT_FLAG_LAST_TILE;
        mapElement.properties.track.type = trackType;
        mapElement.base_height = 3;

        g_currently_drawn_item = &mapElement;

        rct_map_element surfaceElement = {0};
        surfaceElement.flags |= MAP_ELEMENT_FLAG_LAST_TILE;
        surfaceElement.type = MAP_ELEMENT_TYPE_SURFACE;
        surfaceElement.base_height = 2;

        gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
        gTrackColours[SCHEME_TRACK] = DEFAULT_SCHEME_TRACK;
        gTrackColours[SCHEME_SUPPORTS] = DEFAULT_SCHEME_SUPPORTS;
        gTrackColours[SCHEME_MISC] = DEFAULT_SCHEME_MISC;
        gTrackColours[SCHEME_3] = DEFAULT_SCHEME_3;

        rct_drawpixelinfo dpi = { 0 };
        dpi.zoom_level = 1;
        unk_140E9A8 = &dpi;

        rct_vehicle vehicle = {0};
        rct_ride ride = {0};

        rct_ride_entry rideEntry = {0};
        rct_ride_entry_vehicle vehicleEntry { 0 };
        vehicleEntry.base_image_id = 0x70000;
        rideEntry.vehicles[0] = vehicleEntry;


        gRideList[0] = ride;
        gRideEntries[0] = &rideEntry;

        int height = 48;

        TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];
        int sequenceCount = getTrackSequenceCount(rideType, trackType);

        for (int chainLift = 0; chainLift < 2; chainLift++) {
            if (chainLift == 0) {
                mapElement.type &= ~0x80;
            } else {
                mapElement.type |= 0x80;
            }

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
                g141E9DB = G141E9DB_FLAG_1 | G141E9DB_FLAG_2;

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
                printf("Original segment calls didn't match. [trackSequence:%d chainLift:%d]\n", trackSequence, chainLift);
                continue;
            }

            for (int direction = 0; direction < 4; direction++) {
                for (int s = 0; s < 9; ++s) {
                    gSupportSegments[s].height = 0;
                    gSupportSegments[s].slope = 0xFF;
                }

                testpaint_clear_ignore();
                TRACK_PAINT_FUNCTION newPaintFunction = newPaintGetter(trackType, direction);
                newPaintFunction(rideIndex, trackSequence, direction, height, &mapElement);
                if (testpaint_is_ignored(direction, trackSequence)) {
                    continue;
                }

                std::vector<SegmentSupportCall> newCalls = getSegmentCalls(gSupportSegments, direction);

                if (!SegmentCallEquals(tileSegmentSupportCalls[0], newCalls)) {
                    // TODO put this into *error
                    utf8string diff = new utf8[2048];
                    sprintf(diff, "<<< EXPECTED\n");
                    printSegmentSupports(&diff, tileSegmentSupportCalls[0]);
                    sprintf(diff + strlen(diff), "====\n");
                    printSegmentSupports(&diff, newCalls);
                    sprintf(diff + strlen(diff), ">>> ACTUAL\n");

                    printf("Segment support heights didn't match. [direction:%d trackSequence:%d chainLift:%d]\n", direction,
                           trackSequence, chainLift);
                    printf("%s", diff);
                    delete[] diff;
                    return false;
                }
            }

            if (!supportCallsMatch(tileGeneralSupportCalls)) {
                // TODO: if 3 directions do share the output, use that.
                printf("Original support calls didn't match. [trackSequence:%d chainLift:%d]\n", trackSequence, chainLift);
                continue;
            }

            SupportCall referenceGeneralSupportCall = tileGeneralSupportCalls[0];


            for (int direction = 0; direction < 4; direction++) {
                gSupport.height = 0;
                gSupport.slope = 0xFF;

                testpaint_clear_ignore();
                TRACK_PAINT_FUNCTION newPaintFunction = newPaintGetter(trackType, direction);
                newPaintFunction(rideIndex, trackSequence, direction, height, &mapElement);
                if (testpaint_is_ignored(direction, trackSequence)) {
                    continue;
                }

                if (referenceGeneralSupportCall.height != -1) {
                    if (gSupport.height != referenceGeneralSupportCall.height) {
                        printf("General support heights didn't match. (expected height + %d, actual: height + %d) [direction:%d trackSequence:%d chainLift:%d]\n",
                               referenceGeneralSupportCall.height - height,
                               gSupport.height - height,
                               direction,
                               trackSequence,
                               chainLift);
                        return false;
                    }
                }
                if (referenceGeneralSupportCall.slope != -1) {
                    if (gSupport.slope != referenceGeneralSupportCall.slope) {
                        printf("General support slopes didn't match. [direction:%d trackSequence:%d chainLift:%d]\n", direction,
                               trackSequence, chainLift);
                        return false;
                    }
                }
            }

        }
        }
        return true;
    }

    static bool tunnelPatternsMatch(TunnelCall expected[4], TunnelCall actual[4])
    {
        for (int side = 0; side < 4; side++) {
            if (expected[side].call != actual[side].call) return false;

            if (expected[side].call == TUNNELCALL_CALL) {
                if (expected[side].type != actual[side].type) return false;
                if (expected[side].offset != actual[side].offset) return false;
            }
        }

        return true;
    }

    static sint16 getTunnelOffset(uint32 baseHeight, tunnel_entry calls[3])
    {
        for (sint16 offset = -56; offset <= 56; offset += 8) {
            if (calls[0].height != (baseHeight - 8 + offset) / 16) continue;
            if (calls[1].height != (baseHeight + 0 + offset) / 16) continue;
            if (calls[2].height != (baseHeight + 8 + offset) / 16) continue;

            return offset;
        }

        log_error("Unknown tunnel height. (%d, %d, %d)", calls[0].height, calls[1].height, calls[2].height);
        return 0;
    }

    static bool testTunnels(uint8 rideType, uint8 trackType)
    {
        uint8 rideIndex = 0;
        rct_map_element mapElement = {0};
        mapElement.flags |= MAP_ELEMENT_FLAG_LAST_TILE;
        mapElement.properties.track.type = trackType;
        mapElement.base_height = 3;

        g_currently_drawn_item = &mapElement;

        rct_map_element surfaceElement = {0};
        surfaceElement.type = MAP_ELEMENT_TYPE_SURFACE;
        surfaceElement.base_height = 2;

        gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
        gTrackColours[SCHEME_TRACK] = DEFAULT_SCHEME_TRACK;
        gTrackColours[SCHEME_SUPPORTS] = DEFAULT_SCHEME_SUPPORTS;
        gTrackColours[SCHEME_MISC] = DEFAULT_SCHEME_MISC;
        gTrackColours[SCHEME_3] = DEFAULT_SCHEME_3;

        rct_drawpixelinfo dpi { 0 };
        dpi.zoom_level = 1;
        unk_140E9A8 = &dpi;

        rct_vehicle vehicle = {0};
        rct_ride ride = {0};

        rct_ride_entry rideEntry = {0};
        rct_ride_entry_vehicle vehicleEntry { 0 };
        vehicleEntry.base_image_id = 0x70000;
        rideEntry.vehicles[0] = vehicleEntry;


        gRideList[0] = ride;
        gRideEntries[0] = &rideEntry;

        int height = 48;

        TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];
        int sequenceCount = getTrackSequenceCount(rideType, trackType);


        for (int trackSequence = 0; trackSequence < sequenceCount; trackSequence++) {
            TunnelCall tileTunnelCalls[4][4];

            for (int direction = 0; direction < 4; direction++) {
                gLeftTunnelCount = 0;
                gRightTunnelCount = 0;

                uint32 *trackDirectionList = (uint32 *)RideTypeTrackPaintFunctionsOld[rideType][trackType];

                for (int offset = -8; offset <= 8; offset += 8) {
                    // Have to call from this point as it pushes esi and expects callee to pop it
                    RCT2_CALLPROC_X(
                        0x006C4934,
                        rideType,
                        (int) trackDirectionList,
                        direction,
                        height + offset,
                        (int) &mapElement,
                        rideIndex * sizeof(rct_ride),
                        trackSequence
                    );
                }

                uint8 rightIndex = (4 - direction) % 4;
                uint8 leftIndex = (rightIndex + 1) % 4;

                for (int i = 0; i < 4; ++i) {
                    tileTunnelCalls[direction][i].call = TUNNELCALL_SKIPPED;
                }
                if (gRightTunnelCount == 0) {
                    tileTunnelCalls[direction][rightIndex].call = TUNNELCALL_NONE;
                } else if (gRightTunnelCount == 3) {
                    tileTunnelCalls[direction][rightIndex].call = TUNNELCALL_CALL;
                    tileTunnelCalls[direction][rightIndex].offset = getTunnelOffset(height, gRightTunnels);
                    tileTunnelCalls[direction][rightIndex].type = gRightTunnels[0].type;
                } else {
                    printf("Multiple tunnels on one side aren't supported.\n");
                    return false;
                }

                if (gLeftTunnelCount == 0) {
                    tileTunnelCalls[direction][leftIndex].call = TUNNELCALL_NONE;
                } else if (gLeftTunnelCount == 3) {
                    tileTunnelCalls[direction][leftIndex].call = TUNNELCALL_CALL;
                    tileTunnelCalls[direction][leftIndex].offset = getTunnelOffset(height, gLeftTunnels);
                    tileTunnelCalls[direction][leftIndex].type = gLeftTunnels[0].type;
                } else {
                    printf("Multiple tunnels on one side aren't supported.\n");
                    return false;
                }
            }

            TunnelCall newTileTunnelCalls[4][4];
            for (int direction = 0; direction < 4; direction++) {
                gLeftTunnelCount = 0;
                gRightTunnelCount = 0;

                testpaint_clear_ignore();
                TRACK_PAINT_FUNCTION newPaintFunction = newPaintGetter(trackType, direction);

                for (int offset = -8; offset <= 8; offset += 8) {
                    // TODO: move tunnel pushing to interface so we don't have to check the output 3 times
                    newPaintFunction(rideIndex, trackSequence, direction, height + offset, &mapElement);
                }

                uint8 rightIndex = (4 - direction) % 4;
                uint8 leftIndex = (rightIndex + 1) % 4;

                for (int i = 0; i < 4; ++i) {
                    newTileTunnelCalls[direction][i].call = TUNNELCALL_SKIPPED;
                }

                if (gRightTunnelCount == 0) {
                    newTileTunnelCalls[direction][rightIndex].call = TUNNELCALL_NONE;
                } else if (gRightTunnelCount == 3) {
                    newTileTunnelCalls[direction][rightIndex].call = TUNNELCALL_CALL;
                    newTileTunnelCalls[direction][rightIndex].offset = getTunnelOffset(height, gRightTunnels);
                    newTileTunnelCalls[direction][rightIndex].type = gRightTunnels[0].type;
                } else {
                    printf("Multiple tunnels on one side aren't supported.\n");
                    return false;
                }

                if (gLeftTunnelCount == 0) {
                    newTileTunnelCalls[direction][leftIndex].call = TUNNELCALL_NONE;
                } else if (gLeftTunnelCount == 3) {
                    newTileTunnelCalls[direction][leftIndex].call = TUNNELCALL_CALL;
                    newTileTunnelCalls[direction][leftIndex].offset = getTunnelOffset(height, gLeftTunnels);
                    newTileTunnelCalls[direction][leftIndex].type = gLeftTunnels[0].type;
                } else {
                    printf("Multiple tunnels on one side aren't supported.\n");
                    return false;
                }
            }


            if (!tunnelCallsLineUp(tileTunnelCalls)) {
                printf("Original tunnel calls don\'t line up. Skipping tunnel validation [trackSequence:%d].\n",
                       trackSequence);
                printTunnelCalls(tileTunnelCalls);

                if (!tunnelCallsLineUp(newTileTunnelCalls)) {
                    printf("Decompiled tunnel calls don\'t line up. [trackSequence:%d].\n", trackSequence);
                    printTunnelCalls(newTileTunnelCalls);
                    return false;
                }
                continue;
            }

            TunnelCall referencePattern[4];
            getTunnelCallReferencePattern(tileTunnelCalls, &referencePattern);

            TunnelCall actualPattern[4];
            getTunnelCallReferencePattern(newTileTunnelCalls, &actualPattern);

            if (!tunnelPatternsMatch(referencePattern, actualPattern)) {
                printf("Tunnel calls don't match expected pattern. [trackSequence:%d]\n", trackSequence);
                printf("expected:\n");
                printTunnelCalls(tileTunnelCalls);
                printf("actual:\n");
                printTunnelCalls(newTileTunnelCalls);
                return false;
            }

        }
        return true;
    }

    static bool verticalTunnelHeightIsConsistent(uint8 heights[4])
    {
        for (int i = 1; i < 4; ++i) {
            if (heights[i] != heights[0]) return false;
        }

        return true;
    }

    static void printRelativeHeight(utf8string out, sint16 height)
    {
        if (height == 0) {
            sprintf(out, "height");
            return;
        }

        if (height > 0) {
            sprintf(out, "height + %d", height);
            return;
        }

        if (height < 0) {
            sprintf(out, "height - %d", int(abs(height)));
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

        rct_map_element surfaceElement = {0};
        surfaceElement.type = MAP_ELEMENT_TYPE_SURFACE;
        surfaceElement.base_height = 2;

        gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
        gTrackColours[SCHEME_TRACK] = DEFAULT_SCHEME_TRACK;
        gTrackColours[SCHEME_SUPPORTS] = DEFAULT_SCHEME_SUPPORTS;
        gTrackColours[SCHEME_MISC] = DEFAULT_SCHEME_MISC;
        gTrackColours[SCHEME_3] = DEFAULT_SCHEME_3;

        rct_drawpixelinfo dpi = { 0 };
        dpi.zoom_level = 1;
        unk_140E9A8 = &dpi;

        rct_vehicle vehicle = {0};
        rct_ride ride = {0};

        rct_ride_entry rideEntry = {0};
        rct_ride_entry_vehicle vehicleEntry = { 0 };
        vehicleEntry.base_image_id = 0x70000;
        rideEntry.vehicles[0] = vehicleEntry;


        gRideList[0] = ride;
        gRideEntries[0] = &rideEntry;

        int height = 48;

        TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];
        int sequenceCount = getTrackSequenceCount(rideType, trackType);


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
                    printRelativeHeight(strExpectedTunnelHeight, (referenceHeight * 16) - 48);
                    printRelativeHeight(strActualTunnelHeight, (gVerticalTunnelHeight * 16) - 48);

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
    bool testSupportSegments(uint8 rideType, uint8 trackType)
    {
        return Intercept2::testSupportSegments(rideType, trackType);
    }

    bool testTunnels(uint8 rideType, uint8 trackType)
    {
        return Intercept2::testTunnels(rideType, trackType);
    }

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
}
