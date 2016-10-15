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
static const support_height DefaultSegmentHeight[9] = {
	BLANK_SUPPORT, BLANK_SUPPORT, BLANK_SUPPORT,
	BLANK_SUPPORT, BLANK_SUPPORT, BLANK_SUPPORT,
	BLANK_SUPPORT, BLANK_SUPPORT, BLANK_SUPPORT
};

extern const utf8string RideNames[91];
extern const utf8string TrackNames[256];
extern const utf8string FlatTrackNames[256];

static bool _woodenSupports;
static uint8 callCount;
static function_call calls[256];

void intercept_clear_calls()
{
	callCount = 0;
	memset(calls, 0, sizeof(calls));
}

int intercept_get_calls(function_call * buffer)
{
	memcpy(buffer, calls, 256);
	return callCount;
}

bool paint_attach_to_previous_ps(uint32 image_id, uint16 x, uint16 y) {
	return false;
}

paint_struct *sub_98196C(
	uint32 image_id,
	sint8 x_offset, sint8 y_offset,
	sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
	sint16 z_offset,
	uint32 rotation
) {
	function_call call = {
		.function = PAINT_98196C,
		.paint = {
			.image_id = image_id,
			.offset = {x_offset, y_offset},
			.bound_box_length = {bound_box_length_x, bound_box_length_y, bound_box_length_z},
			.z_offset = z_offset,
			.rotation = rotation
		},
	};

	calls[callCount] = call;
	callCount++;

	return NULL;
}

paint_struct *sub_98197C(
	uint32 image_id,
	sint8 x_offset, sint8 y_offset,
	sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
	sint16 z_offset,
	sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z,
	uint32 rotation
) {
	function_call call = {
		.function = PAINT_98197C,
		.paint = {
			.image_id = image_id,
			.offset = {x_offset, y_offset},
			.bound_box_length = {bound_box_length_x, bound_box_length_y, bound_box_length_z},
			.bound_box_offset = {bound_box_offset_x, bound_box_offset_y, bound_box_offset_z},
			.z_offset = z_offset,
			.rotation = rotation,
		},
	};

	calls[callCount] = call;
	callCount++;

	return NULL;
}

paint_struct *sub_98198C(
	uint32 image_id,
	sint8 x_offset, sint8 y_offset,
	sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
	sint16 z_offset,
	sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z,
	uint32 rotation
) {
	function_call call = {
		.function = PAINT_98198C,
		.paint = {
			.image_id = image_id,
			.offset = {x_offset, y_offset},
			.bound_box_length = {bound_box_length_x, bound_box_length_y, bound_box_length_z},
			.bound_box_offset = {bound_box_offset_x, bound_box_offset_y, bound_box_offset_z},
			.z_offset = z_offset,
			.rotation = rotation,
		},
	};

	calls[callCount] = call;
	callCount++;

	return NULL;
}

paint_struct *sub_98199C(
	uint32 image_id,
	sint8 x_offset, sint8 y_offset,
	sint16 bound_box_length_x, sint16 bound_box_length_y, sint8 bound_box_length_z,
	sint16 z_offset,
	sint16 bound_box_offset_x, sint16 bound_box_offset_y, sint16 bound_box_offset_z,
	uint32 rotation
) {
	function_call call = {
		.function = PAINT_98199C,
		.paint = {
			.image_id = image_id,
			.offset = {x_offset, y_offset},
			.bound_box_length = {bound_box_length_x, bound_box_length_y, bound_box_length_z},
			.bound_box_offset = {bound_box_offset_x, bound_box_offset_y, bound_box_offset_z},
			.z_offset = z_offset,
			.rotation = rotation,
		},
	};

	calls[callCount] = call;
	callCount++;

	return NULL;
}

bool wooden_a_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, bool *underground) {

	function_call call = {
		.function = SUPPORTS_WOOD_A,
		.supports = {
			.type = supportType,
			.special = special,
			.height = height,
			.colour_flags = imageColourFlags,
		}
	};

	calls[callCount] = call;
	callCount++;
	return _woodenSupports;
}

bool wooden_b_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, bool *underground) {

	function_call call = {
		.function = SUPPORTS_WOOD_B,
		.supports = {
			.type = supportType,
			.special = special,
			.height = height,
			.colour_flags = imageColourFlags,
		}
	};

	calls[callCount] = call;
	callCount++;
	return _woodenSupports;
}

static void check_support_height()
{
	// First get last known support height state
	if (memcmp(gSupportSegments, &DefaultSegmentHeight, sizeof(support_height) * 9) == 0) {
		// Nothing changed
		return;
	}

	function_call call = {
		.function = SET_SEGMENT_HEIGHT
	};

	calls[callCount] = call;
	callCount++;
}

bool metal_a_supports_paint_setup(int supportType, int segment, int special, int height, uint32 imageColourFlags) {

	check_support_height();

	function_call call = {
		.function = SUPPORTS_METAL_A,
		.supports = {
			.type = supportType,
			.segment = segment,
			.special = special,
			.height = height,
			.colour_flags = imageColourFlags,
		}
	};

	calls[callCount] = call;
	callCount++;
	return false;
}

bool metal_b_supports_paint_setup(int supportType, uint8 segment, int special, int height, uint32 imageColourFlags) {

	check_support_height();

	function_call call = {
		.function = SUPPORTS_METAL_B,
		.supports = {
			.type = supportType,
			.segment = segment,
			.special = special,
			.height = height,
			.colour_flags = imageColourFlags,
		}
	};

	calls[callCount] = call;
	callCount++;
	return false;
}

enum {
	SPRITEGROUP_NONE,

	SPRITEGROUP_FENCE_METAL_A, // 14568
	SPRITEGROUP_FENCE_METAL_B, // 14990
	SPRITEGROUP_FENCE_SPIRAL_SLIDE, // 20564
	SPRITEGROUP_FLOOR_CORK, // 22134
	SPRITEGROUP_FENCE_ROPE, // 22138

};

static int getSpriteGroup(uint16 spriteIndex) {
	if (spriteIndex >= 14568 && spriteIndex <= 14571) {
		return SPRITEGROUP_FENCE_METAL_A;
	}

	if (spriteIndex >= 14990 && spriteIndex <= 14993) {
		return SPRITEGROUP_FENCE_METAL_B;
	}

	if (spriteIndex >= 20564 && spriteIndex <= 20567) {
		return SPRITEGROUP_FENCE_SPIRAL_SLIDE;
	}

	if (spriteIndex >= 22134 && spriteIndex <= 22137) {
		return SPRITEGROUP_FLOOR_CORK;
	}

	if (spriteIndex >= 22138 && spriteIndex <= 22141) {
		return SPRITEGROUP_FENCE_ROPE;
	}

	return SPRITEGROUP_NONE;
}

static void canonicalizeFunctionCall(function_call *call) {
	if (call->function != PAINT_98197C) return;
	if (call->paint.offset.x != call->paint.bound_box_offset.x) return;
	if (call->paint.offset.y != call->paint.bound_box_offset.y) return;
	if (call->paint.z_offset != call->paint.bound_box_offset.z) return;

	call->function = PAINT_98196C;
}

bool assertFunctionCallEquals(function_call expected, function_call actual) {
	canonicalizeFunctionCall(&actual);
	canonicalizeFunctionCall(&expected);

	if (expected.function != actual.function) {
		return false;
	}

	uint8 function = expected.function;

	if (function == SUPPORTS_WOOD_A || function == SUPPORTS_WOOD_B) {
		if (expected.supports.type != actual.supports.type) return false;
		if (expected.supports.special != actual.supports.special) return false;
		if (expected.supports.height != actual.supports.height) return false;
		if (expected.supports.colour_flags != actual.supports.colour_flags) return false;

		return true;
	}

	if (function == SUPPORTS_METAL_A || function == SUPPORTS_METAL_B) {
		if (expected.supports.type != actual.supports.type) return false;
		if (expected.supports.segment != actual.supports.segment) return false;
		if (expected.supports.special != actual.supports.special) return false;
		if (expected.supports.height != actual.supports.height) return false;
		if (expected.supports.colour_flags != actual.supports.colour_flags) return false;

		return true;
	}

	if (function == SET_SEGMENT_HEIGHT) {
		return true;
	}

	if (expected.paint.image_id != actual.paint.image_id) {
		int expectedSpriteGroup = getSpriteGroup(expected.paint.image_id & 0x7FFFF);
		int actualSpriteGroup = getSpriteGroup(actual.paint.image_id & 0x7FFFF);

		if (expectedSpriteGroup != actualSpriteGroup) return false;

		if (expectedSpriteGroup == SPRITEGROUP_NONE) return false;

		return true;
	}

	if (expected.paint.offset.x != actual.paint.offset.x) return false;
	if (expected.paint.offset.y != actual.paint.offset.y) return false;
	if (expected.paint.bound_box_length.x != actual.paint.bound_box_length.x) return false;
	if (expected.paint.bound_box_length.y != actual.paint.bound_box_length.y) return false;
	if (expected.paint.bound_box_length.z != actual.paint.bound_box_length.z) return false;
	if (function != PAINT_98196C) {
		if (expected.paint.bound_box_offset.x != actual.paint.bound_box_offset.x) return false;
		if (expected.paint.bound_box_offset.y != actual.paint.bound_box_offset.y) return false;
		if (expected.paint.bound_box_offset.z != actual.paint.bound_box_offset.z) return false;
	}
	if (expected.paint.z_offset != actual.paint.z_offset) return false;
	if (expected.paint.rotation != actual.paint.rotation) return false;

	return true;
}

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

static void printImageId(uint32 input, utf8string out, size_t len) {
	uint32 image = input & 0x7FFFF;
	uint32 palette = input & ~0x7FFFF;

	utf8string paletteName;
	if (palette == DEFAULT_SCHEME_TRACK)paletteName = "SCHEME_TRACK";
	else if (palette == DEFAULT_SCHEME_SUPPORTS)paletteName = "SCHEME_SUPPORTS";
	else if (palette == DEFAULT_SCHEME_MISC)paletteName = "SCHEME_MISC";
	else if (palette == DEFAULT_SCHEME_3)paletteName = "SCHEME_3";
	else {
		paletteName = malloc(16);
		snprintf(paletteName, 16, "0x%08X", palette);
	}

	if (image == 0) {
		snprintf(out, len, "%s", paletteName);
	} else if (image & 0x70000) {
		snprintf(out, len, "%s | vehicle.base_image_id + %d", paletteName, image & ~0x70000);
	} else {
		snprintf(out, len, "%s | %d", paletteName, image);
	}
}

static void printFunctionCall(utf8string out, size_t len, function_call call) {
	utf8string imageId = malloc(64);
	printImageId(call.supports.colour_flags, imageId, 64);
	switch (call.function) {
	case SUPPORTS_WOOD_A:
		snprintf(out, len, "wooden_a_supports_paint_setup(%d, %d, %d, %s)", call.supports.type, call.supports.special, call.supports.height, imageId);
		return;
	case SUPPORTS_WOOD_B:
		snprintf(out, len, "wooden_b_supports_paint_setup(%d, %d, %d, %s)", call.supports.type, call.supports.special, call.supports.height, imageId);
		return;

	case SUPPORTS_METAL_A:
		snprintf(out, len, "metal_a_supports_paint_setup(%d, %d, %d, %d, %s)", call.supports.type, call.supports.segment, call.supports.special, call.supports.height, imageId);
		return;
	case SUPPORTS_METAL_B:
		snprintf(out, len, "metal_b_supports_paint_setup(%d, %d, %d, %d, %s)", call.supports.type, call.supports.segment, call.supports.special, call.supports.height, imageId);
		return;

	case SET_SEGMENT_HEIGHT:
		snprintf(out, len, "paint_util_set_segment_support_height");
		return;
	}

	utf8string name = "_default";
	switch (call.function) {
	case PAINT_98196C:
		name = "sub_98196C";
		break;
	case PAINT_98197C:
		name = "sub_98197C";
		break;
	case PAINT_98198C:
		name = "sub_98198C";
		break;
	case PAINT_98199C:
		name = "sub_98199C";
		break;
	}

	size_t slen;

	printImageId(call.paint.image_id, imageId, 64);
	slen = snprintf(
		out,
		len,
		"%s(%s, %d, %d, %d, %d, %d, %d, ",
		name,
		imageId,
		call.paint.offset.x, call.paint.offset.y,
		call.paint.bound_box_length.x, call.paint.bound_box_length.y, call.paint.bound_box_length.z,
		call.paint.z_offset
	);
	if (slen >= len) return;

	if (call.function != PAINT_98196C) {
		if (slen < len)
			slen += snprintf(
				out + slen,
				len - slen,
				"%d, %d, %d, ",
				call.paint.bound_box_offset.x, call.paint.bound_box_offset.y, call.paint.bound_box_offset.z
			);
	}

	if (slen < len)
		slen += snprintf(out + slen, len - slen, "%d)", call.paint.rotation);

	if (call.function != PAINT_98196C) {
		if (slen < len)
			snprintf(out + slen, len - slen, "    = { %d, %d, %d }, { %d, %d, %d }, { %d, %d, %d }",
				call.paint.offset.x, call.paint.offset.y, call.paint.z_offset - 48,
				call.paint.bound_box_offset.x, call.paint.bound_box_offset.y, call.paint.bound_box_offset.z - 48,
				call.paint.bound_box_length.x, call.paint.bound_box_length.y, call.paint.bound_box_length.z);
	}
}

static void printFunctionCallArray(utf8string out, size_t len, function_call calls[], uint8 count) {
	for (int i = 0; i < count; i++) {
		utf8string callOut = malloc(1024);
		printFunctionCall(callOut, 1024, calls[i]);
		size_t slen = strlen(out);
		if (slen < len)
			snprintf(out + slen, len - slen, "%s\n", callOut);
	}
}

int getTrackSequenceCount(uint8 rideType, uint8 trackType) {
	int sequenceCount = 0;
	const rct_preview_track **trackBlocks;

	if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_FLAT_RIDE)) {
		trackBlocks = FlatRideTrackBlocks;
	} else {
		trackBlocks = TrackBlocks;
	}

	for (int i = 0; i < 256; i++) {
		if (trackBlocks[trackType][i].index == 0xFF) {
			break;
		}

		sequenceCount++;
	}

	return sequenceCount;
}

bool rideSupportsTrackType(int rideType, int trackType)
{
	TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];

	if (newPaintGetter == NULL) {
		return false;
	}

	if (newPaintGetter(trackType, 0) == NULL) {
		return false;
	}

	if (RideTypeTrackPaintFunctionsOld[rideType][trackType] == 0) {
		return false;
	}

	return true;
}


extern bool testSupportSegments(uint8 rideType, uint8 trackType);
extern bool testTunnels(uint8 rideType, uint8 trackType);
extern bool testVerticalTunnels(uint8 rideType, uint8 trackType);

static bool testTrackElement(uint8 rideType, uint8 trackType, utf8string error, size_t len) {
	if (rideType == RIDE_TYPE_CHAIRLIFT) {
		if (trackType == TRACK_ELEM_BEGIN_STATION || trackType == TRACK_ELEM_MIDDLE_STATION || trackType == TRACK_ELEM_END_STATION) {
			// These rides chechk neighbouring tiles for tracks
			snprintf(error, len, "Skipped");
			return false;
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

	gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
	gTrackColours[SCHEME_TRACK] = DEFAULT_SCHEME_TRACK;
	gTrackColours[SCHEME_SUPPORTS] = DEFAULT_SCHEME_SUPPORTS;
	gTrackColours[SCHEME_MISC] = DEFAULT_SCHEME_MISC;
    gTrackColours[SCHEME_3] = DEFAULT_SCHEME_3;

	rct_drawpixelinfo dpi = {.zoom_level = 1};
	unk_140E9A8 = &dpi;

	rct_ride ride = { 0 };
	ride.entrance_style = RIDE_ENTRANCE_STYLE_CANVAS_TENT;

	rct_ride_entry rideEntry = { 0 };
	rct_ride_entry_vehicle vehicleEntry = {.base_image_id = 0x70000};
	rideEntry.vehicles[0] = vehicleEntry;


	gRideList[0] = ride;
	gRideEntries[0] = &rideEntry;

	int height = 48;

	snprintf(error, len, "rct2: 0x%08X\n", RideTypeTrackPaintFunctionsOld[rideType][trackType]);

	TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];
	int sequenceCount = getTrackSequenceCount(rideType, trackType);

	for (int supports = 0; supports < 2; supports++) {
		if (supports == 0) {
			_woodenSupports = false;
		} else {
			_woodenSupports = true;
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
				g141E9DB = G141E9DB_FLAG_1 | G141E9DB_FLAG_2;

				callCount = 0;
				memset(&calls, 0, sizeof(calls));

				memcpy(gSupportSegments, DefaultSegmentHeight, sizeof(support_height) * 9);

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

				uint8 oldCallCount = callCount;
				function_call oldCalls[256];
				memcpy(&oldCalls, &calls, sizeof(calls));

				callCount = 0;

				testpaint_clear_ignore();
				memcpy(gSupportSegments, DefaultSegmentHeight, sizeof(support_height) * 9);
				newPaintFunction(rideIndex, trackSequence, direction, height, &mapElement);
				if (testpaint_is_ignored(direction, trackSequence)) {
					snprintf(error, len, "[  IGNORED ]   [direction:%d trackSequence:%d chainLift:%d inverted:%d]\n",
						direction, trackSequence, chainLift, inverted);
					continue;
				}

				uint8 newCallCount = callCount;
				function_call newCalls[256];
				memcpy(&newCalls, &calls, sizeof(calls));

				if (!assertFunctionCallArrayEquals(oldCalls, oldCallCount, newCalls, newCallCount)) {
					utf8string diff = malloc(2048);

					snprintf(diff, 2048, "<<< EXPECTED\n");
					printFunctionCallArray(diff, 2048, oldCalls, oldCallCount);
					
					size_t slen = strlen(diff);
					if (slen < 2048)
						snprintf(diff + slen, 2048 - slen, "====\n");
					printFunctionCallArray(diff, 2048, newCalls, newCallCount);
					
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

					return false;
				}

			}
		}
	}
	}
	}
	}

	bool segmentSuccess = testSupportSegments(rideType, trackType);
	if (!segmentSuccess) {
		return false;
	}

	bool tunnelSuccess = testTunnels(rideType, trackType);
	if (!tunnelSuccess) {
		return false;
	}

	bool verticalTunnelSuccess = testVerticalTunnels(rideType, trackType);
	if (!verticalTunnelSuccess) {
		return false;
	}
	return true;
}

bool rideIsImplemented(int rideType) {
	TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];
	return (newPaintGetter != 0);
}

bool testTrackPainting(int rideType, int trackType) {
	TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];
	if (newPaintGetter == NULL) {
		return false;
	}

	if (newPaintGetter(trackType, 0) == NULL) {
		return false;
	}

	utf8string error = malloc(2048);
	bool success = testTrackElement(rideType, trackType, error, 2048);
	if (!success) {
		printf("%s\n", error);
	}
	free(error);

	return success;
}

static int intercept_draw_6c(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp) {
	registers regs = {.eax =eax, .ebx = ebx, .ecx = ecx, .edx = edx, .esi = esi, .edi = edi, .ebp = ebp};
	if ((ebp & 0x03) != get_current_rotation()) {
		// Log error
		log_error("Ebp is different from current rotation");
	}

	return (int) sub_98196C(ebx, regs.al, regs.cl, regs.di, regs.si, regs.ah, regs.dx, regs.ebp & 0x03);
}

static int intercept_draw_7c(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp) {
	registers regs = {.eax =eax, .ebx = ebx, .ecx = ecx, .edx = edx, .esi = esi, .edi = edi, .ebp = ebp};
	if ((ebp & 0x03) != get_current_rotation()) {
		// Log error
		log_error("Ebp is different from current rotation");
	}

	rct_xyz16 boundOffset = {
		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_X, sint16),
		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_Y, sint16),
		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_Z, sint16)
	};

	return (int) sub_98197C(ebx, regs.al, regs.cl, regs.di, regs.si, regs.ah, regs.dx, boundOffset.x, boundOffset.y, boundOffset.z, regs.ebp & 0x03);
}

static int intercept_draw_9c(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp) {
	registers regs = {.eax =eax, .ebx = ebx, .ecx = ecx, .edx = edx, .esi = esi, .edi = edi, .ebp = ebp};
	if ((ebp & 0x03) != get_current_rotation()) {
		// Log error
		log_error("Ebp is different from current rotation");
	}

	rct_xyz16 boundOffset = {
		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_X, sint16),
		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_Y, sint16),
		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_BOUNDBOX_OFFSET_Z, sint16)
	};

	return (int) sub_98199C(ebx, regs.al, regs.cl, regs.di, regs.si, regs.ah, regs.dx, boundOffset.x, boundOffset.y, boundOffset.z, regs.ebp & 0x03);
}

static uint32 intercept_wooden_a_supports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp) {
	registers regs = {.eax =eax, .ebx = ebx, .edx = edx, .edi = edi, .ebp = ebp};
	bool output = wooden_a_supports_paint_setup(regs.edi, (sint16) regs.ax, regs.dx, (uint32) regs.ebp, NULL);

	return output ? 1 : 0;
}

static uint32 intercept_wooden_b_supports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp) {
	registers regs = {.eax =eax, .ebx = ebx, .edx = edx, .edi = edi, .ebp = ebp};
	bool output = wooden_b_supports_paint_setup(regs.edi, (sint16) regs.ax, regs.dx, (uint32) regs.ebp, NULL);

	return output ? 1 : 0;
}

static uint32 intercept_metal_a_supports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp) {
	registers regs = {.eax =eax, .ebx = ebx, .edx = edx, .edi = edi, .ebp = ebp};
	metal_a_supports_paint_setup(regs.edi, regs.ebx, (sint16) regs.ax, regs.dx, (uint32) regs.ebp);

	return 0;
}

static uint32 intercept_metal_b_supports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp) {
	registers regs = {.eax =eax, .ebx = ebx, .edx = edx, .edi = edi, .ebp = ebp};
	metal_b_supports_paint_setup(regs.edi, regs.ebx, (sint16) regs.ax, regs.dx, (uint32) regs.ebp);

	return 0;
}


void initHooks() {
	addhook(0x00686806, (int) intercept_draw_7c, 0, (int[]) {EAX, EBX, ECX, EDX, ESI, EDI, EBP, END}, 0, EBP);
	addhook(0x006869B2, (int) intercept_draw_7c, 0, (int[]) {EAX, EBX, ECX, EDX, ESI, EDI, EBP, END}, 0, EBP);
	addhook(0x00686B6F, (int) intercept_draw_7c, 0, (int[]) {EAX, EBX, ECX, EDX, ESI, EDI, EBP, END}, 0, EBP);
	addhook(0x00686D31, (int) intercept_draw_7c, 0, (int[]) {EAX, EBX, ECX, EDX, ESI, EDI, EBP, END}, 0, EBP);

	addhook(0x006861AC, (int) intercept_draw_6c, 0, (int[]) {EAX, EBX, ECX, EDX, ESI, EDI, EBP, END}, 0, EBP);
	addhook(0x00686337, (int) intercept_draw_6c, 0, (int[]) {EAX, EBX, ECX, EDX, ESI, EDI, EBP, END}, 0, EBP);
	addhook(0x006864D0, (int) intercept_draw_6c, 0, (int[]) {EAX, EBX, ECX, EDX, ESI, EDI, EBP, END}, 0, EBP);
	addhook(0x0068666B, (int) intercept_draw_6c, 0, (int[]) {EAX, EBX, ECX, EDX, ESI, EDI, EBP, END}, 0, EBP);

	addhook(0x006874B0, (int) intercept_draw_9c, 0, (int[]) {EAX, EBX, ECX, EDX, ESI, EDI, EBP, END}, 0, EBP);
	addhook(0x00687618, (int) intercept_draw_9c, 0, (int[]) {EAX, EBX, ECX, EDX, ESI, EDI, EBP, END}, 0, EBP);
	addhook(0x0068778C, (int) intercept_draw_9c, 0, (int[]) {EAX, EBX, ECX, EDX, ESI, EDI, EBP, END}, 0, EBP);
	addhook(0x00687902, (int) intercept_draw_9c, 0, (int[]) {EAX, EBX, ECX, EDX, ESI, EDI, EBP, END}, 0, EBP);

	addhook(0x006629BC, (int) intercept_wooden_a_supports, 0, (int[]) {EAX, EBX, EDX, EDI, EBP, END}, 0, EAX);
	addhook(0x00662D5C, (int) intercept_wooden_b_supports, 0, (int[]) {EAX, EBX, EDX, EDI, EBP, END}, 0, EAX);

	addhook(0x00663105, (int) intercept_metal_a_supports, 0, (int[]) {EAX, EBX, EDX, EDI, EBP, END}, 0, EAX);
	addhook(0x00663584, (int) intercept_metal_b_supports, 0, (int[]) {EAX, EBX, EDX, EDI, EBP, END}, 0, EAX);
}
