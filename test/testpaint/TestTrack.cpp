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
#include "Printer.hpp"
#include "String.hpp"
#include "TestTrack.hpp"
#include "Utils.hpp"
#include "FunctionCall.hpp"

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

uint8 TestTrack::TestPaintTrackElement(uint8 rideType, uint8 trackType) {
    if (!Utils::rideSupportsTrackType(rideType, trackType)) {
        return TEST_FAILED;
    }


    std::string error = "";
    uint8 retVal = TestPaintTrackElementCalls(rideType, trackType, &error);

    if (retVal != TEST_SUCCESS) {
        printf("%s\n", error.c_str());
    }

    return retVal;
}

uint8 TestTrack::TestPaintTrackElementCalls(uint8 rideType, uint8 trackType, std::string *error) {
    if (rideType == RIDE_TYPE_CHAIRLIFT) {
        if (trackType == TRACK_ELEM_BEGIN_STATION || trackType == TRACK_ELEM_MIDDLE_STATION ||
            trackType == TRACK_ELEM_END_STATION) {
            // These rides check neighbouring tiles for tracks
            return TEST_SKIPPED;
        }
    }

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

    intercept_reset_environment();

    int sequenceCount = getTrackSequenceCount(rideType, trackType);

    *error += String::Format("rct2: 0x%08X\n", RideTypeTrackPaintFunctionsOld[rideType][trackType]);

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
            for (int trackSequence = 0; trackSequence < sequenceCount; trackSequence++) {
                RCT2_GLOBAL(0x009DE56A, sint16) = 64; // x
                RCT2_GLOBAL(0x009DE56E, sint16) = 64; // y

                std::string caseName = String::Format(
                    "[direction:%d trackSequence:%d chainLift:%d inverted:%d]",
                    direction, trackSequence, chainLift, inverted
                );

                intercept_clear_calls();
                intercept_reset_segment_heights();

                CallOriginal(rideType, trackType, direction, trackSequence, height, &mapElement);

                callCount = intercept_get_calls(callBuffer);
                std::vector<function_call> oldCalls;
                oldCalls.insert(oldCalls.begin(), callBuffer, callBuffer + callCount);

                intercept_clear_calls();
                testpaint_clear_ignore();
                intercept_reset_segment_heights();

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
    }

    bool segmentSuccess = testSupportSegments(rideType, trackType);
    if (!segmentSuccess) {
        return TEST_FAILED;
    }

    bool tunnelSuccess = testTunnels(rideType, trackType);
    if (!tunnelSuccess) {
        return TEST_FAILED;
    }

    bool verticalTunnelSuccess = testVerticalTunnels(rideType, trackType);
    if (!verticalTunnelSuccess) {
        return TEST_FAILED;
    }

    return TEST_SUCCESS;
}