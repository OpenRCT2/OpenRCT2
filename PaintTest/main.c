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

#include "../src/paint/paint.h"
#include "../src/ride/track_data.h"
#include "../src/ride/track_paint.h"
#include "../src/interface/viewport.h"
#include "../src/hook.h"


#define gRideEntries                RCT2_ADDRESS(RCT2_ADDRESS_RIDE_ENTRIES,                rct_ride_entry*)
#define gCurrentRotation        RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8)


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

extern const utf8string RideNames[91];
extern const utf8string TrackNames[256];
extern const utf8string FlatTrackNames[256];

typedef struct
{
	utf8string name;
	uint32 image_id;
	rct_xy16 offset;
	rct_xyz16 bound_box_length;
	sint16 z_offset;
	rct_xyz16 bound_box_offset;
	uint32 rotation;
} function_call;

static uint8 callCount;
static function_call calls[256];

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
		.name = "sub_98196C",
		.image_id = image_id,
		.offset = {x_offset, y_offset},
		.bound_box_length = {bound_box_length_x, bound_box_length_y, bound_box_length_z},
		.z_offset = z_offset,
		.rotation = rotation
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
		.name = "sub_98197C",
		.image_id = image_id,
		.offset = {x_offset, y_offset},
		.bound_box_length = {bound_box_length_x, bound_box_length_y, bound_box_length_z},
		.bound_box_offset = {bound_box_offset_x, bound_box_offset_y, bound_box_offset_z},
		.z_offset = z_offset,
		.rotation = rotation
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
		.name = "sub_98198C",
		.image_id = image_id,
		.offset = {x_offset, y_offset},
		.bound_box_length = {bound_box_length_x, bound_box_length_y, bound_box_length_z},
		.bound_box_offset = {bound_box_offset_x, bound_box_offset_y, bound_box_offset_z},
		.z_offset = z_offset,
		.rotation = rotation
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
		.name = "sub_98199C",
		.image_id = image_id,
		.offset = {x_offset, y_offset},
		.bound_box_length = {bound_box_length_x, bound_box_length_y, bound_box_length_z},
		.bound_box_offset = {bound_box_offset_x, bound_box_offset_y, bound_box_offset_z},
		.z_offset = z_offset,
		.rotation = rotation
	};

	calls[callCount] = call;
	callCount++;

	return NULL;
}

bool wooden_a_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags, bool *underground) {
	return false;
}

bool wooden_b_supports_paint_setup(int supportType, int special, int height, uint32 imageColourFlags) {
	return false;
}

bool metal_a_supports_paint_setup(int supportType, int segment, int special, int height, uint32 imageColourFlags) {
	return false;
}

bool metal_b_supports_paint_setup(int supportType, uint8 segment, int special, int height, uint32 imageColourFlags) {
	return false;
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

bool testTrackElement(uint8 rideType, uint8 trackType, utf8string *error) {
	if (rideType == RIDE_TYPE_CHAIRLIFT) {
		if (trackType == TRACK_ELEM_BEGIN_STATION || trackType == TRACK_ELEM_MIDDLE_STATION || trackType == TRACK_ELEM_END_STATION) {
			// These rides chechk neighbouring tiles for tracks
			*error = "Skipped";
			return false;
		}
	}
	uint8 rideIndex = 0;
	rct_map_element mapElement = {};

	g_currently_drawn_item = &mapElement;

	gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
	RCT2_GLOBAL(0x00F44198, uint32) = COLOUR_GREY << 19 | COLOUR_WHITE << 24 | 0xA0000000;
	RCT2_GLOBAL(0x00F441A0, uint32) = COLOUR_DARK_PURPLE << 19 | COLOUR_LIGHT_PURPLE << 24 | 0xA0000000;
	RCT2_GLOBAL(0x00F441A4, uint32) = COLOUR_BRIGHT_PURPLE << 19 | COLOUR_DARK_BLUE << 24 | 0xA0000000;
	RCT2_GLOBAL(0x00F4419C, uint32) = COLOUR_LIGHT_BLUE << 19 | COLOUR_ICY_BLUE << 24 | 0xA0000000;

	rct_drawpixelinfo dpi = {.zoom_level = 1};
	unk_140E9A8 = &dpi;

	rct_vehicle vehicle = {};
	rct_ride ride = {};

	rct_ride_entry rideEntry = {};
	rct_ride_entry_vehicle vehicleEntry = {.base_image_id = 0x70000};
	rideEntry.vehicles[0] = vehicleEntry;


	gRideList[0] = ride;
	gRideEntries[0] = &rideEntry;

	int height = 48;

	TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];
	int sequenceCount = getTrackSequenceCount(rideType, trackType);
	for (int currentRotation = 0; currentRotation < 4; currentRotation++) {
		gCurrentRotation = currentRotation;
		for (int direction = 0; direction < 4; direction++) {
			TRACK_PAINT_FUNCTION newPaintFunction = newPaintGetter(trackType, direction);
			for (int trackSequence = 0; trackSequence < sequenceCount; trackSequence++) {

				callCount = 0;
				bzero(calls, sizeof(calls));

				TRACK_PAINT_FUNCTION **trackTypeList = (TRACK_PAINT_FUNCTION **) RideTypeTrackPaintFunctionsOld[rideType];
				uint32 *trackDirectionList = (uint32 *) trackTypeList[trackType];

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

				const uint8 oldCallCount = callCount;
				const function_call oldCalls[256];
				bcopy(oldCalls, calls, sizeof(calls));

				callCount = 0;

				newPaintFunction(rideIndex, trackSequence, direction, height, &mapElement);

				const uint8 newCallCount = callCount;
				const function_call newCalls[256];
				bcopy(newCalls, calls, sizeof(calls));

				if (newCallCount != oldCallCount) {
					// TODO: array with errors?
					sprintf(*error, "Call counts don't match [direction:%d trackSequence:%d]", direction, trackSequence);
					return false;
				}

			}
		}
	}

	return true;
}

void testRide(int rideType) {
	TRACK_PAINT_FUNCTION_GETTER newPaintGetter = RideTypeTrackPaintFunctions[rideType];
	if (newPaintGetter == 0) {
		return;
	}

	printf("- %s (%d)\n", RideNames[rideType], rideType);
	for (int trackType = 0; trackType < 256; trackType++) {
		if (newPaintGetter(trackType, 0) == NULL) {
			continue;
		}

		utf8string error = malloc(256);
		bool success = testTrackElement(rideType, trackType, &error);

		if (!success) {
			printf(ANSI_COLOR_RED);
		}
		int sequenceCount = getTrackSequenceCount(rideType, trackType);
		if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_FLAT_RIDE)) {
			printf("  - %s (%d)", FlatTrackNames[trackType], sequenceCount);
		} else {
			printf("  - %s (%d)", TrackNames[trackType], sequenceCount);
		}


		if (!success) {
			printf(" FAILED!\n    %s", error);
		}
		printf(ANSI_COLOR_RESET "\n");

	}
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

	rct_xyz16 boundOffset = {RCT2_GLOBAL(0x009DEA52, sint16), RCT2_GLOBAL(0x009DEA54, sint16), RCT2_GLOBAL(0x009DEA56, sint16)};

	return (int) sub_98197C(ebx, regs.al, regs.cl, regs.di, regs.si, regs.ah, regs.dx, boundOffset.x, boundOffset.y, boundOffset.z, regs.ebp & 0x03);
}

static int intercept_draw_9c(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx, uint32 esi, uint32 edi, uint32 ebp) {
	registers regs = {.eax =eax, .ebx = ebx, .ecx = ecx, .edx = edx, .esi = esi, .edi = edi, .ebp = ebp};
	if ((ebp & 0x03) != get_current_rotation()) {
		// Log error
		log_error("Ebp is different from current rotation");
	}

	rct_xyz16 boundOffset = {RCT2_GLOBAL(0x009DEA52, sint16), RCT2_GLOBAL(0x009DEA54, sint16), RCT2_GLOBAL(0x009DEA56, sint16)};

	return (int) sub_98199C(ebx, regs.al, regs.cl, regs.di, regs.si, regs.ah, regs.dx, boundOffset.x, boundOffset.y, boundOffset.z, regs.ebp & 0x03);
}

static void intercept_wooden_a_supports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp) {
	registers regs = {.eax =eax, .ebx = ebx, .edx = edx, .edi = edi, .ebp = ebp};
}

static void intercept_wooden_b_supports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp) {
	registers regs = {.eax =eax, .ebx = ebx, .edx = edx, .edi = edi, .ebp = ebp};
}

static void intercept_metal_a_supports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp) {
	registers regs = {.eax =eax, .ebx = ebx, .edx = edx, .edi = edi, .ebp = ebp};
}

static void intercept_metal_b_supports(uint32 eax, uint32 ebx, uint32 edx, uint32 edi, uint32 ebp) {
	registers regs = {.eax =eax, .ebx = ebx, .edx = edx, .edi = edi, .ebp = ebp};
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

	addhook(0x006629BC, (int) intercept_wooden_a_supports, 0, (int[]) {EAX, EBX, EDX, EDI, EBP, END}, 0, 0);
	addhook(0x00662D5C, (int) intercept_wooden_b_supports, 0, (int[]) {EAX, EBX, EDX, EDI, EBP, END}, 0, 0);

	addhook(0x00663105, (int) intercept_metal_a_supports, 0, (int[]) {EAX, EBX, EDX, EDI, EBP, END}, 0, 0);
	addhook(0x00663584, (int) intercept_metal_b_supports, 0, (int[]) {EAX, EBX, EDX, EDI, EBP, END}, 0, 0);
}

int main(int argc, const char *argv[]) {
	initHooks();
	for (int i = 0; i < 91; i++) {
		testRide(i);
	}
	return 0;
}