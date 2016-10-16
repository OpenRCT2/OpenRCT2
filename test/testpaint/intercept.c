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

#include "intercept.h"
#include "../../src/paint/paint.h"
#include "../../src/paint/supports.h"
#include "../../src/ride/track_data.h"
#include "../../src/interface/viewport.h"
#include "../../src/hook.h"

static const uint32 DEFAULT_SCHEME_TRACK = COLOUR_GREY << 19 | COLOUR_WHITE << 24 | 0xA0000000;
static const uint32 DEFAULT_SCHEME_SUPPORTS = COLOUR_LIGHT_BLUE << 19 | COLOUR_ICY_BLUE << 24 | 0xA0000000;
static const uint32 DEFAULT_SCHEME_MISC = COLOUR_DARK_PURPLE << 19 | COLOUR_LIGHT_PURPLE << 24 | 0xA0000000;
static const uint32 DEFAULT_SCHEME_3 = COLOUR_BRIGHT_PURPLE << 19 | COLOUR_DARK_BLUE << 24 | 0xA0000000;

#define BLANK_SUPPORT {.height = 0, .slope = 0xFF}
const support_height DefaultSegmentHeight[9] = {
	BLANK_SUPPORT, BLANK_SUPPORT, BLANK_SUPPORT,
	BLANK_SUPPORT, BLANK_SUPPORT, BLANK_SUPPORT,
	BLANK_SUPPORT, BLANK_SUPPORT, BLANK_SUPPORT
};

extern const utf8string RideNames[91];
extern const utf8string TrackNames[256];
extern const utf8string FlatTrackNames[256];



static bool assertFunctionCallArrayEquals(function_call expected[], uint8 expectedCount, function_call actual[], uint8 actualCount) {
	if (expectedCount != actualCount) {
		return false;
	}

	for (int i = 0; i < expectedCount; i++) {
		function_call expectedCall = expected[i];
		function_call actualCall = actual[i];

		if (!assertFunctionCallEquals(expectedCall, actualCall)) {
			return false;
		}
	}

	return true;
}

static void printFunctionCallArray(utf8string out, size_t len, function_call calls[], uint8 count, uint16 baseHeight) {
	for (int i = 0; i < count; i++) {
		utf8string callOut = malloc(1024);
		printFunctionCall(callOut, 1024, calls[i], baseHeight);
		size_t slen = strlen(out);
		if (slen < len)
			snprintf(out + slen, len - slen, "%s\n", callOut);
	}
}


extern bool testSupportSegments(uint8 rideType, uint8 trackType);
extern bool testTunnels(uint8 rideType, uint8 trackType);
extern bool testVerticalTunnels(uint8 rideType, uint8 trackType);


static uint8 testTrackElement(uint8 rideType, uint8 trackType, utf8string error, size_t len) {
	if (rideType == RIDE_TYPE_CHAIRLIFT) {
		if (trackType == TRACK_ELEM_BEGIN_STATION || trackType == TRACK_ELEM_MIDDLE_STATION || trackType == TRACK_ELEM_END_STATION) {
			// These rides check neighbouring tiles for tracks
			return TEST_SKIPPED;
		}
	}

	uint8 rideIndex = 0;
	rct_map_element mapElement = { 0 };
	mapElement.flags |= MAP_ELEMENT_FLAG_LAST_TILE;
	mapElement.properties.track.type = trackType;
	mapElement.base_height = 3;

	g_currently_drawn_item = &mapElement;

	rct_map_element surfaceElement = { 0 };
	surfaceElement.type = MAP_ELEMENT_TYPE_SURFACE;
	surfaceElement.base_height = 2;

    intercept_reset_environment();

	int height = 48;

	snprintf(error, len, "rct2: 0x%08X\n", RideTypeTrackPaintFunctionsOld[rideType][trackType]);

	TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];
	int sequenceCount = getTrackSequenceCount(rideType, trackType);

	for (int supports = 0; supports < 2; supports++) {
		if (supports == 0) {
			intercept_simulate_wooden_supports(false);
		} else {
			intercept_simulate_wooden_supports(true);
		}
	for (int inverted = 0; inverted < 2; inverted++) {
		if (inverted == 0) {
			mapElement.properties.track.colour &= ~TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
		} else {
			mapElement.properties.track.colour |= TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
		}
	for (int chainLift = 0; chainLift < 2; chainLift++) {
		if (chainLift == 0) {
			mapElement.type &= ~0x80;
		} else {
			mapElement.type |= 0x80;
		}
	for (int currentRotation = 0; currentRotation < 4; currentRotation++) {
		gCurrentRotation = currentRotation;
		for (int direction = 0; direction < 4; direction++) {
			TRACK_PAINT_FUNCTION newPaintFunction = newPaintGetter(trackType, direction);
			for (int trackSequence = 0; trackSequence < sequenceCount; trackSequence++) {
				RCT2_GLOBAL(0x009DE56A, sint16) = 64; // x
				RCT2_GLOBAL(0x009DE56E, sint16) = 64; // y
				gDidPassSurface = true; // Above surface
				gSurfaceElement = &surfaceElement;

				intercept_clear_calls();
				intercept_reset_segment_heights();

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
				// segment heights
				// tunnels

				function_call oldCalls[256];
				int oldCallCount = intercept_get_calls(oldCalls);

				intercept_clear_calls();

				testpaint_clear_ignore();
				intercept_reset_segment_heights();

				newPaintFunction(rideIndex, trackSequence, direction, height, &mapElement);
				if (testpaint_is_ignored(direction, trackSequence)) {
					snprintf(error, len, "[  IGNORED ]   [direction:%d trackSequence:%d chainLift:%d inverted:%d]\n",
						direction, trackSequence, chainLift, inverted);
					continue;
				}

				function_call newCalls[256];
				int newCallCount = intercept_get_calls(newCalls);

				if (!assertFunctionCallArrayEquals(oldCalls, oldCallCount, newCalls, newCallCount)) {
					utf8string diff = malloc(2048);

					snprintf(diff, 2048, "<<< EXPECTED\n");
					printFunctionCallArray(diff, 2048, oldCalls, oldCallCount, height);
					
					size_t slen = strlen(diff);
					if (slen < 2048)
						snprintf(diff + slen, 2048 - slen, "====\n");
					printFunctionCallArray(diff, 2048, newCalls, newCallCount, height);
					
					slen = strlen(diff);
					if (slen < 2048)
						snprintf(diff + slen, 2048 - slen, ">>> ACTUAL\n");
					
					if (oldCallCount != newCallCount) {
						slen = strlen(error);
						if (slen < len)
							snprintf(error + slen, len - slen, "Call counts don't match (was %d, expected %d) [direction:%d trackSequence:%d chainLift:%d inverted:%d]",
								newCallCount, oldCallCount, direction, trackSequence, chainLift, inverted);
					} else {
						slen = strlen(error);
						if (slen < len)
							snprintf(error + slen, len - slen, "Calls don't match [direction:%d trackSequence:%d chainLift:%d inverted:%d]",
								direction, trackSequence, chainLift, inverted);
					}
					
					slen = strlen(error);
					if (slen < len)
						snprintf(error + slen, len - slen, "\n%s", diff);
					
					free(diff);

					return TEST_FAILED;
				}

			}
		}
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

uint8 testTrackPainting(int rideType, int trackType) {
	TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];
	if (newPaintGetter == NULL) {
		return false;
	}

	if (newPaintGetter(trackType, 0) == NULL) {
		return false;
	}

	utf8string error = malloc(2048);
	int retVal = testTrackElement(rideType, trackType, error, 2048);
	if (retVal != TEST_SUCCESS) {
		printf("%s\n", error);
	}
	free(error);

	return retVal;
}
