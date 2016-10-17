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

#include <string>
#include <vector>

#include "intercept.h"
#include "FunctionCall.hpp"
#include "GeneralSupportHeightCall.hpp"
#include "Printer.hpp"
#include "SegmentSupportHeightCall.hpp"
#include "SideTunnelCall.hpp"
#include "String.hpp"
#include "TestTrack.hpp"
#include "Utils.hpp"

extern "C" {
#include "../../src/ride/ride.h"
#include "../../src/ride/track.h"
#include "../../src/ride/track_data.h"
}

static void CallOriginal(
    uint8 rideType,
    uint8 trackType,
    uint8 direction,
    uint8 trackSequence,
    uint16 height,
    rct_map_element *mapElement
) {
    uint32 *trackDirectionList = (uint32 *) RideTypeTrackPaintFunctionsOld[rideType][trackType];
    const uint8 rideIndex = 0;

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
}

static void CallNew(
    uint8 rideType,
    uint8 trackType,
    uint8 direction,
    uint8 trackSequence,
    uint16 height,
    rct_map_element *mapElement
) {
    TRACK_PAINT_FUNCTION_GETTER newPaintFunctionGetter = RideTypeTrackPaintFunctions[rideType];
    TRACK_PAINT_FUNCTION newPaintFunction = newPaintFunctionGetter(trackType, direction);

    newPaintFunction(0, trackSequence, direction, height, mapElement);
}

typedef uint8 (*TestFunction)(uint8, uint8, uint8, std::string *);

static uint8 TestTrackElementPaintCalls(uint8 rideType, uint8 trackType, uint8 trackSequence, std::string *error);

static uint8 TestTrackElementSegmentSupportHeight(uint8 rideType, uint8 trackType, uint8 trackSequence, std::string *error);

static uint8 TestTrackElementGeneralSupportHeight(uint8 rideType, uint8 trackType, uint8 trackSequence, std::string *error);

static uint8 TestTrackElementSideTunnels(uint8 rideType, uint8 trackType, uint8 trackSequence, std::string *error);

uint8 TestTrack::TestPaintTrackElement(uint8 rideType, uint8 trackType) {
    if (!Utils::rideSupportsTrackType(rideType, trackType)) {
        return TEST_FAILED;
    }

    if (rideType == RIDE_TYPE_CHAIRLIFT) {
        if (trackType == TRACK_ELEM_BEGIN_STATION || trackType == TRACK_ELEM_MIDDLE_STATION ||
            trackType == TRACK_ELEM_END_STATION) {
            // These rides check neighbouring tiles for tracks
            return TEST_SKIPPED;
        }
    }

    int sequenceCount = Utils::getTrackSequenceCount(rideType, trackType);
    std::string error = String::Format("rct2: 0x%08X\n", RideTypeTrackPaintFunctionsOld[rideType][trackType]);

    uint8 retVal = TEST_SUCCESS;

    static TestFunction functions[] = {
        TestTrackElementPaintCalls,
        TestTrackElementSegmentSupportHeight,
        TestTrackElementGeneralSupportHeight,
        TestTrackElementSideTunnels,
    };

    for (int trackSequence = 0; trackSequence < sequenceCount; trackSequence++) {
        for (auto &&function : functions) {
            retVal = function(rideType, trackType, trackSequence, &error);
        }

        if (retVal != TEST_SUCCESS) {
            printf("%s\n", error.c_str());
            return retVal;
        }
    }

    return retVal;
}

static uint8 TestTrackElementPaintCalls(uint8 rideType, uint8 trackType, uint8 trackSequence, std::string *error) {
    uint8 rideIndex = 0;
    uint16 height = 3 * 16;

    rct_map_element mapElement = {0};
    mapElement.flags |= MAP_ELEMENT_FLAG_LAST_TILE;
    mapElement.properties.track.type = trackType;
    mapElement.base_height = height / 16;
    g_currently_drawn_item = &mapElement;

    rct_map_element surfaceElement = {0};
    surfaceElement.type = MAP_ELEMENT_TYPE_SURFACE;
    surfaceElement.base_height = 2;
    gSurfaceElement = &surfaceElement;
    gDidPassSurface = true;

    Intercept2::ResetEnvironment();
    Intercept2::ResetTunnels();

    function_call callBuffer[256] = {0};
    int callCount = 0;

    int chainLift = 0;
    int inverted = 0;

    // TODO: test chainlift
    // TODO: test inverted
    // TODO: test supports
    // TODO: test entrance styles

    for (int currentRotation = 0; currentRotation < 4; currentRotation++) {
        gCurrentRotation = currentRotation;
        for (int direction = 0; direction < 4; direction++) {
            RCT2_GLOBAL(0x009DE56A, sint16) = 64; // x
            RCT2_GLOBAL(0x009DE56E, sint16) = 64; // y

            std::string caseName = String::Format(
                "[direction:%d trackSequence:%d chainLift:%d inverted:%d]",
                direction, trackSequence, chainLift, inverted
            );

            intercept_clear_calls();
            Intercept2::ResetSupportHeights();

            CallOriginal(rideType, trackType, direction, trackSequence, height, &mapElement);

            callCount = intercept_get_calls(callBuffer);
            std::vector<function_call> oldCalls;
            oldCalls.insert(oldCalls.begin(), callBuffer, callBuffer + callCount);

            intercept_clear_calls();
            testpaint_clear_ignore();
            Intercept2::ResetSupportHeights();

            CallNew(rideType, trackType, direction, trackSequence, height, &mapElement);

            if (testpaint_is_ignored(direction, trackSequence)) {
                *error += String::Format("[  IGNORED ]   %s\n", caseName.c_str());
                continue;
            }

            callCount = intercept_get_calls(callBuffer);
            std::vector<function_call> newCalls;
            newCalls.insert(newCalls.begin(), callBuffer, callBuffer + callCount);

            bool sucess = true;
            if (oldCalls.size() != newCalls.size()) {
                *error += String::Format(
                    "Call counts don't match (was %d, expected %d). %s\n",
                    newCalls.size(), oldCalls.size(), caseName.c_str()
                );
                sucess = false;
            } else if (!FunctionCall::AssertsEquals(oldCalls, newCalls)) {
                *error += String::Format("Calls don't match. %s\n", caseName.c_str());
                sucess = false;
            }

            if (!sucess) {
                *error += " Expected:\n";
                *error += Printer::PrintFunctionCalls(oldCalls, height);
                *error += "   Actual:\n";
                *error += Printer::PrintFunctionCalls(newCalls, height);

                return TEST_FAILED;
            }
        }
    }

    return TEST_SUCCESS;
}

static uint8 TestTrackElementSegmentSupportHeight(uint8 rideType, uint8 trackType, uint8 trackSequence, std::string *error) {
    uint8 rideIndex = 0;
    uint16 height = 3 * 16;

    rct_map_element mapElement = {0};
    mapElement.flags |= MAP_ELEMENT_FLAG_LAST_TILE;
    mapElement.properties.track.type = trackType;
    mapElement.base_height = height / 16;
    g_currently_drawn_item = &mapElement;

    rct_map_element surfaceElement = {0};
    surfaceElement.type = MAP_ELEMENT_TYPE_SURFACE;
    surfaceElement.base_height = 2;
    gSurfaceElement = &surfaceElement;
    gDidPassSurface = true;

    Intercept2::ResetEnvironment();
    Intercept2::ResetTunnels();

    // TODO: Test Chainlift
    // TODO: Test Maze
    // TODO: Allow skip

    std::string state = String::Format("[trackSequence:%d chainLift:%d]", trackSequence, 0);

    std::vector<SegmentSupportCall> tileSegmentSupportCalls[4];

    for (int direction = 0; direction < 4; direction++) {
        Intercept2::ResetSupportHeights();

        CallOriginal(rideType, trackType, direction, trackSequence, height, &mapElement);

        tileSegmentSupportCalls[direction] = SegmentSupportHeightCall::getSegmentCalls(gSupportSegments, direction);
    }

    std::vector<SegmentSupportCall> referenceCalls = tileSegmentSupportCalls[0];

    if (!SegmentSupportHeightCall::CallsMatch(tileSegmentSupportCalls)) {
        std::vector<SegmentSupportCall> *found = SegmentSupportHeightCall::FindMostCommonSupportCall(
            tileSegmentSupportCalls);
        if (found != nullptr) {
            referenceCalls = *found;
        } else {
            *error += String::Format("Original segment calls didn't match. %s\n", state.c_str());
            for (int direction = 0; direction < 4; direction++) {
                *error += String::Format("# %d\n", direction);
                *error += Printer::PrintSegmentSupportHeightCalls(tileSegmentSupportCalls[direction]);
            }
            return TEST_FAILED;
        }
    }

    for (int direction = 0; direction < 4; direction++) {
        Intercept2::ResetSupportHeights();

        testpaint_clear_ignore();
        CallNew(rideType, trackType, direction, trackSequence, height, &mapElement);
        if (testpaint_is_ignored(direction, trackSequence)) {
            continue;
        }

        std::vector<SegmentSupportCall> newCalls = SegmentSupportHeightCall::getSegmentCalls(gSupportSegments,
                                                                                             direction);
        if (!SegmentSupportHeightCall::CallsEqual(referenceCalls, newCalls)) {
            *error += String::Format(
                "Segment support heights didn't match. [direction:%d] %s\n",
                direction, state.c_str()
            );
            *error += " Expected:\n";
            *error += Printer::PrintSegmentSupportHeightCalls(referenceCalls);
            *error += "   Actual:\n";
            *error += Printer::PrintSegmentSupportHeightCalls(newCalls);

            return TEST_FAILED;
        }
    }

    return TEST_SUCCESS;
}

static uint8 TestTrackElementGeneralSupportHeight(uint8 rideType, uint8 trackType, uint8 trackSequence, std::string *error) {
    uint8 rideIndex = 0;
    uint16 height = 3 * 16;

    rct_map_element mapElement = {0};
    mapElement.flags |= MAP_ELEMENT_FLAG_LAST_TILE;
    mapElement.properties.track.type = trackType;
    mapElement.base_height = height / 16;
    g_currently_drawn_item = &mapElement;

    rct_map_element surfaceElement = {0};
    surfaceElement.type = MAP_ELEMENT_TYPE_SURFACE;
    surfaceElement.base_height = 2;
    gSurfaceElement = &surfaceElement;
    gDidPassSurface = true;

    Intercept2::ResetEnvironment();
    Intercept2::ResetTunnels();

    // TODO: Test Chainlift
    // TODO: Test Maze
    // TODO: Allow skip

    std::string state = String::Format("[trackSequence:%d chainLift:%d]", trackSequence, 0);

    SupportCall tileGeneralSupportCalls[4];
    for (int direction = 0; direction < 4; direction++) {
        Intercept2::ResetSupportHeights();

        CallOriginal(rideType, trackType, direction, trackSequence, height, &mapElement);

        tileGeneralSupportCalls[direction].height = -1;
        tileGeneralSupportCalls[direction].slope = -1;
        if (gSupport.height != 0) {
            tileGeneralSupportCalls[direction].height = gSupport.height;
        }
        if (gSupport.slope != 0xFF) {
            tileGeneralSupportCalls[direction].slope = gSupport.slope;
        }
    }

    SupportCall referenceGeneralSupportCall = tileGeneralSupportCalls[0];
    if (!GeneralSupportHeightCall::CallsMatch(tileGeneralSupportCalls)) {
        SupportCall *found = GeneralSupportHeightCall::FindMostCommonSupportCall(tileGeneralSupportCalls);
        if (found == nullptr) {
            *error += String::Format("Original support calls didn't match. %s\n", state.c_str());
            for (int i = 0; i < 4; ++i) {
                *error += String::Format("[%d, 0x%02X] ", tileGeneralSupportCalls[i].height, tileGeneralSupportCalls[i].slope);
            }
            *error += "\n";
            return TEST_FAILED;
        }
        referenceGeneralSupportCall = *found;
    }

    for (int direction = 0; direction < 4; direction++) {
        Intercept2::ResetSupportHeights();

        testpaint_clear_ignore();
        CallNew(rideType, trackType, direction, trackSequence, height, &mapElement);
        if (testpaint_is_ignored(direction, trackSequence)) {
            continue;
        }


        if (referenceGeneralSupportCall.height != -1) {
            if (gSupport.height != referenceGeneralSupportCall.height) {
                *error += String::Format(
                    "General support heights didn't match. (expected height + %d, actual: height + %d) [direction:%d] %s\n",
                    referenceGeneralSupportCall.height - height,
                    gSupport.height - height,
                    direction,
                    state.c_str()
                );
                return TEST_FAILED;
            }
        }
        if (referenceGeneralSupportCall.slope != -1) {
            if (gSupport.slope != referenceGeneralSupportCall.slope) {
                *error += String::Format(
                    "General support slopes didn't match. (expected 0x%02X, actual: 0x%02X) [direction:%d] %s\n",
                    referenceGeneralSupportCall.slope,
                    gSupport.slope,
                    direction,
                    state.c_str()
                );
                return TEST_FAILED;
            }
        }
    }

    return TEST_SUCCESS;
}

static uint8 TestTrackElementSideTunnels(uint8 rideType, uint8 trackType, uint8 trackSequence, std::string *error) {
    uint8 rideIndex = 0;
    uint16 height = 3 * 16;

    rct_map_element mapElement = {0};
    mapElement.flags |= MAP_ELEMENT_FLAG_LAST_TILE;
    mapElement.properties.track.type = trackType;
    mapElement.base_height = height / 16;
    g_currently_drawn_item = &mapElement;

    rct_map_element surfaceElement = {0};
    surfaceElement.type = MAP_ELEMENT_TYPE_SURFACE;
    surfaceElement.base_height = 2;
    gSurfaceElement = &surfaceElement;
    gDidPassSurface = true;

    Intercept2::ResetEnvironment();
    Intercept2::ResetTunnels();

    TunnelCall tileTunnelCalls[4][4];

    for (int direction = 0; direction < 4; direction++) {
        Intercept2::ResetTunnels();

        for (sint8 offset = -8; offset <= 8; offset += 8) {
            CallOriginal(rideType, trackType, direction, trackSequence, height + offset, &mapElement);
        }

        uint8 rightIndex = (4 - direction) % 4;
        uint8 leftIndex = (rightIndex + 1) % 4;

        for (int i = 0; i < 4; ++i) {
            tileTunnelCalls[direction][i].call = TUNNELCALL_SKIPPED;
        }

        bool err = false;
        tileTunnelCalls[direction][rightIndex] = SideTunnelCall::ExtractTunnelCalls(gRightTunnels, gRightTunnelCount, height,
                                                                                    &err);

        tileTunnelCalls[direction][leftIndex] = SideTunnelCall::ExtractTunnelCalls(gLeftTunnels, gLeftTunnelCount, height,
                                                                                   &err);

        if (err) {
            *error += "Multiple tunnels on one side aren't supported.\n";
            return TEST_FAILED;
        }
    }

    TunnelCall newTileTunnelCalls[4][4];
    for (int direction = 0; direction < 4; direction++) {
        Intercept2::ResetTunnels();

        testpaint_clear_ignore();

        for (sint8 offset = -8; offset <= 8; offset += 8) {
            // TODO: move tunnel pushing to interface so we don't have to check the output 3 times
            CallNew(rideType, trackType, direction, trackSequence, height + offset, &mapElement);
        }

        uint8 rightIndex = (4 - direction) % 4;
        uint8 leftIndex = (rightIndex + 1) % 4;

        for (int i = 0; i < 4; ++i) {
            newTileTunnelCalls[direction][i].call = TUNNELCALL_SKIPPED;
        }

        bool err = false;
        newTileTunnelCalls[direction][rightIndex] = SideTunnelCall::ExtractTunnelCalls(gRightTunnels, gRightTunnelCount, height,
                                                                                       &err);

        newTileTunnelCalls[direction][leftIndex] = SideTunnelCall::ExtractTunnelCalls(gLeftTunnels, gLeftTunnelCount, height,
                                                                                      &err);

        if (err) {
            *error += "Multiple tunnels on one side aren't supported.\n";
            return TEST_FAILED;
        }
    }


    if (!SideTunnelCall::TunnelCallsLineUp(tileTunnelCalls)) {
        *error += String::Format(
            "Original tunnel calls don\'t line up. Skipping tunnel validation [trackSequence:%d].\n",
            trackSequence
        );
        *error += Printer::PrintSideTunnelCalls(tileTunnelCalls);

        if (!SideTunnelCall::TunnelCallsLineUp(newTileTunnelCalls)) {
            *error += String::Format("Decompiled tunnel calls don\'t line up. [trackSequence:%d].\n", trackSequence);
            *error += Printer::PrintSideTunnelCalls(newTileTunnelCalls);
            return TEST_FAILED;
        }

        return TEST_SUCCESS;
    }

    TunnelCall referencePattern[4];
    SideTunnelCall::GetTunnelCallReferencePattern(tileTunnelCalls, &referencePattern);

    TunnelCall actualPattern[4];
    SideTunnelCall::GetTunnelCallReferencePattern(newTileTunnelCalls, &actualPattern);

    if (!SideTunnelCall::TunnelPatternsMatch(referencePattern, actualPattern)) {
        *error += String::Format("Tunnel calls don't match expected pattern. [trackSequence:%d]\n", trackSequence);
        *error += " Expected:\n";
        *error += Printer::PrintSideTunnelCalls(tileTunnelCalls);
        *error += "   Actual:\n";
        *error += Printer::PrintSideTunnelCalls(newTileTunnelCalls);
        return TEST_FAILED;
    }

    return TEST_SUCCESS;
}