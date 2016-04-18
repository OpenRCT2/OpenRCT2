/*****************************************************************************
* Copyright (c) 2014 Ted John
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* This file is part of OpenRCT2.
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "../addresses.h"
#include "../config.h"
#include "../drawing/drawing.h"
#include "../drawing/supports.h"
#include "../interface/viewport.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../world/map.h"
#include "../world/sprite.h"
#include "ride_data.h"
#include "track_data.h"
#include "track_paint.h"

enum {
	DF_NE = 1 << 0,
	DF_SE = 1 << 1,
	DF_SW = 1 << 2,
	DF_NW = 1 << 3,
};

static rct_xy8 GetEntranceCheckOffset(int direction, int rotation)
{
	const static rct_xy8 entranceCheckOffsets[] = {
		{ -1,  0 },
		{  0, -1 },
		{  1,  0 },
		{  0,  1 },

		{  0,  1 },
		{ -1,  0 },
		{  0, -1 },
		{  1,  0 },

		{  1,  0 },
		{  0,  1 },
		{ -1,  0 },
		{  0, -1 },

		{  0, -1 },
		{  1,  0 },
		{  0,  1 },
		{ -1,  0 },
	};

#if DEBUG
	assert(direction >= 0 && direction <= 3);
	assert(rotation >= 0 && rotation <= 3);
#endif
	return entranceCheckOffsets[(direction * 4) + rotation];
}

static void floor_paint_setup(int floorType, int imageOR, int height)
{
	assert(floorType >= 0 && floorType <= 3);

	uint32 imageId = (22134 + floorType) | imageOR;
	RCT2_GLOBAL(0x009DEA52, uint16) = 0;
	RCT2_GLOBAL(0x009DEA54, uint16) = 0;
	RCT2_GLOBAL(0x009DEA56, uint16) = height;
	sub_98197C(imageId, 0, 0, 32, 32, 1, height, get_current_rotation());
}

static void fence_paint_setup(int direction, int height)
{
	uint32 imageId;
	switch (direction) {
	case 0:
		imageId = 20564 | RCT2_GLOBAL(0x00F44198, uint32);
		RCT2_GLOBAL(0x009DEA52, uint16) = 2;
		RCT2_GLOBAL(0x009DEA54, uint16) = 0;
		RCT2_GLOBAL(0x009DEA56, uint16) = height + 2;
		sub_98199C(imageId, 0, 0, 1, 32, 7, height + 2, get_current_rotation());
		break;
	case 1:
		imageId = 20565 | RCT2_GLOBAL(0x00F44198, uint32);
		RCT2_GLOBAL(0x009DEA52, uint16) = 0;
		RCT2_GLOBAL(0x009DEA54, uint16) = 30;
		RCT2_GLOBAL(0x009DEA56, uint16) = height + 2;
		sub_98199C(imageId, 0, 0, 32, 1, 7, height + 2, get_current_rotation());
		break;
	case 2:
		imageId = 20566 | RCT2_GLOBAL(0x00F44198, uint32);
		RCT2_GLOBAL(0x009DEA52, uint16) = 30;
		RCT2_GLOBAL(0x009DEA54, uint16) = 0;
		RCT2_GLOBAL(0x009DEA56, uint16) = height + 2;
		sub_98199C(imageId, 0, 0, 1, 32, 7, height + 2, get_current_rotation());
		break;
	case 3:
		imageId = 20567 | RCT2_GLOBAL(0x00F44198, uint32);
		RCT2_GLOBAL(0x009DEA52, uint16) = 0;
		RCT2_GLOBAL(0x009DEA54, uint16) = 2;
		RCT2_GLOBAL(0x009DEA56, uint16) = height + 2;
		sub_98199C(imageId, 0, 0, 32, 1, 7, height + 2, get_current_rotation());
		break;
	}
}

static void fence_with_check_paint_setup(int direction, int height, rct_map_element *mapElement)
{
	uint8 currentRotation = get_current_rotation();
	rct_xy8 checkOffset = GetEntranceCheckOffset(direction, currentRotation);

	rct_xy8 fenceCheckPosition = {
		.x = RCT2_GLOBAL(0x009DE56A, uint16) >> 5,
		.y = RCT2_GLOBAL(0x009DE56E, uint16) >> 5
	};
	fenceCheckPosition.x += checkOffset.x;
	fenceCheckPosition.y += checkOffset.y;

	uint8 stationId = map_get_station(mapElement);
	rct_ride *ride = get_ride(mapElement->properties.track.ride_index);
	if (fenceCheckPosition.xy != ride->entrances[stationId] && fenceCheckPosition.xy != ride->exits[stationId]) {
		fence_paint_setup(direction, height);
	}
}

static void fences_with_check_paint_setup(int directions, int direction, int height, rct_map_element *mapElement)
{
	uint8 rotation = get_current_rotation();
	for (int i = 0; i < 4; i++) {
		if (directions & (1 << i)) {
			fence_with_check_paint_setup((direction + i) & 3, height, mapElement);
		}
	}
}

static void update_tile_max_height(sint16 height, uint8 byte_0141E9DA)
{
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x0141E9DA, uint8) = 32;
	}
}

/**
 *
 *  rct2: 0x0076750D
 */
static void top_spin_paint_vehicle(sint8 al, sint8 cl, uint8 rideIndex, uint8 direction, int height, rct_map_element* mapElement)
{
	/**
	 *
	 *  rct2: 0x0142811C
	 * Can be calculated as Rounddown(34*sin(x)+0.5)
	 * where x is in 7.5 deg segments.
	 */
	const static sint8 TopSpinSeatPositionOffset[] = {
		  0,   4,   9,  13,  17,  21,  24,  27,  29,  31,  33,  34,  34,  34,  33,  31,
		 29,  27,  24,  21,  17,  13,   9,   4,   0,  -3,  -8, -12, -16, -20, -23, -26,
		-28, -30, -32, -33, -33, -33, -32, -30, -28, -26, -23, -20, -16, -12,  -8,  -3,
		  0
	};

	// As we will be drawing a vehicle we need to backup the mapElement that
	// is assigned to the drawings.
	rct_map_element* curMapElement = RCT2_GLOBAL(0x009DE578, rct_map_element*);

	height += 3;

	rct_ride* ride = get_ride(rideIndex);
	rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
	rct_vehicle* vehicle = NULL;

	uint8 seatRotation = 0;
	sint8 armRotation = 0;

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK &&
		ride->vehicles[0] != SPRITE_INDEX_NULL) {
		vehicle = GET_VEHICLE(ride->vehicles[0]);

		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_SPRITE;
		RCT2_GLOBAL(0x009DE578, rct_vehicle*) = vehicle;

		armRotation = vehicle->vehicle_sprite_type;
		seatRotation = vehicle->bank_rotation;
	}

	RCT2_GLOBAL(0x009DEA52, sint16) = al + 16;
	RCT2_GLOBAL(0x009DEA54, sint16) = cl + 16;
	RCT2_GLOBAL(0x009DEA56, sint16) = height;

	//di
	uint8 lengthX = 24;
	//si
	uint8 lengthY = 24;

	uint32 image_id = RCT2_GLOBAL(0x00F441A0, uint32);
	if (image_id == 0x20000000) {
		image_id =
			0xA0000000 |
			(ride->track_colour_main[0] << 19) |
			(ride->track_colour_supports[0] << 24);
	}

	image_id += (direction & 1) << 1;
	image_id += rideEntry->vehicles[0].base_image_id;
	// Left back bottom support
	image_id += 572;

	sub_98197C(image_id, al, cl, lengthX, lengthY, 90, height, get_current_rotation());

	image_id = RCT2_GLOBAL(0x00F441A0, uint32);
	if (image_id == 0x20000000) {
		image_id =
			0xA0000000 |
			(ride->track_colour_main[0] << 19) |
			(ride->track_colour_additional[0] << 24);
	}

	sint32 var_1F = armRotation;
	if (direction & 2) {
		var_1F = -var_1F;
		if (var_1F != 0)
			var_1F += 48;
	}
	image_id += var_1F;
	image_id += (direction & 1) * 48;
	image_id += rideEntry->vehicles[0].base_image_id;
	// Left hand arm
	image_id += 380;

	sub_98199C(
		image_id,
		al,
		cl,
		lengthX,
		lengthY,
		90,
		height,
		0);

	uint32 seatImageId;

	if (vehicle != NULL && vehicle->restraints_position >= 64) {
		// Open Restraints
		image_id = (vehicle->restraints_position - 64) >> 6;
		image_id += direction * 3;
		image_id += rideEntry->vehicles[0].base_image_id;
		image_id += 64;
		seatImageId = image_id;
	}
	else {
		image_id = direction * 16;
		// Var_20 Rotation of seats
		image_id += seatRotation;
		image_id += rideEntry->vehicles[0].base_image_id;
		seatImageId = image_id;
	}

	image_id = RCT2_GLOBAL(0x00F441A0, uint32);
	if (image_id == 0x20000000) {
		image_id =
			0xA0000000 |
			(ride->vehicle_colours[0].body_colour << 19) |
			(ride->vehicle_colours[0].trim_colour << 24);
	}
	image_id += seatImageId;

	rct_xyz16 seatCoords = {
		.x = al,
		.y = cl,
		.z = height
	};
	seatCoords.z += RCT2_ADDRESS(0x14280BC, sint16)[armRotation];

	assert(armRotation < sizeof(TopSpinSeatPositionOffset));
	switch (direction) {
	case 0:
		seatCoords.x -= TopSpinSeatPositionOffset[armRotation];
		break;
	case 1:
		seatCoords.y += TopSpinSeatPositionOffset[armRotation];
		break;
	case 2:
		seatCoords.x += TopSpinSeatPositionOffset[armRotation];
		break;
	case 3:
		seatCoords.y -= TopSpinSeatPositionOffset[armRotation];
		break;
	}

	RCT2_GLOBAL(0x014280B8, sint8) = (sint8)seatCoords.x;
	RCT2_GLOBAL(0x014280B9, sint8) = (sint8)seatCoords.y;
	RCT2_GLOBAL(0x014280BA, sint16) = seatCoords.z;

	sub_98199C(
		image_id,
		(sint8)seatCoords.x,
		(sint8)seatCoords.y,
		lengthX,
		lengthY,
		90,
		seatCoords.z,
		0);

	rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);
	if (dpi->zoom_level < 2 && vehicle != NULL && vehicle->num_peeps != 0) {
		image_id =
			(vehicle->peep_tshirt_colours[0] << 19) |
			(vehicle->peep_tshirt_colours[1] << 24);
		image_id += seatImageId;
		image_id += 0xA0000000;
		image_id += 76;

		sub_98199C(image_id, (sint8)seatCoords.x, (sint8)seatCoords.y, lengthX, lengthY, 90, seatCoords.z, 0);

		if (vehicle->num_peeps > 2) {
			image_id =
				(vehicle->peep_tshirt_colours[2] << 19) |
				(vehicle->peep_tshirt_colours[3] << 24);
			image_id += seatImageId;
			image_id += 0xA0000000;
			image_id += 152;

			sub_98199C(image_id, (sint8)seatCoords.x, (sint8)seatCoords.y, lengthX, lengthY, 90, seatCoords.z, 0);
		}

		if (vehicle->num_peeps > 4) {
			image_id =
				(vehicle->peep_tshirt_colours[4] << 19) |
				(vehicle->peep_tshirt_colours[5] << 24);
			image_id += seatImageId;
			image_id += 0xA0000000;
			image_id += 228;

			sub_98199C(image_id, (sint8)seatCoords.x, (sint8)seatCoords.y, lengthX, lengthY, 90, seatCoords.z, 0);
		}

		if (vehicle->num_peeps > 6) {
			image_id =
				(vehicle->peep_tshirt_colours[6] << 19) |
				(vehicle->peep_tshirt_colours[7] << 24);
			image_id += seatImageId;
			image_id += 0xA0000000;
			image_id += 304;

			sub_98199C(image_id, (sint8)seatCoords.x, (sint8)seatCoords.y, lengthX, lengthY, 90, seatCoords.z, 0);
		}
	}

	image_id = RCT2_GLOBAL(0x00F441A0, uint32);
	if (image_id == 0x20000000) {
		image_id =
			0xA0000000 |
			(ride->track_colour_main[0] << 19) |
			(ride->track_colour_additional[0] << 24);
	}

	image_id += var_1F;
	image_id += (direction & 1) * 48;
	image_id += rideEntry->vehicles[0].base_image_id;
	// Right hand arm
	image_id += 476;

	sub_98199C(
		image_id,
		al,
		cl,
		lengthX,
		lengthY,
		90,
		height,
		0);

	image_id = RCT2_GLOBAL(0x00F441A0, uint32);
	if (image_id == 0x20000000) {
		image_id =
			0xA0000000 |
			(ride->track_colour_main[0] << 19) |
			(ride->track_colour_supports[0] << 24);
	}

	image_id += (direction & 1) << 1;
	image_id += rideEntry->vehicles[0].base_image_id;
	// Right back bottom support
	image_id += 573;

	sub_98199C(
		image_id,
		al,
		cl,
		lengthX,
		lengthY,
		90,		
		height,
		0);

	RCT2_GLOBAL(0x009DE578, rct_map_element*) = curMapElement;
	RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_RIDE;
}

static uint16 top_spin_paint_setup_transform(uint16 arg, int height)
{
	if (arg == 2) return height + 2;
	return arg;
}

/**
 *
 *  rct2: 0x007667BC
 *  rct2: 0x007667EC
 *  rct2: 0x0076681C
 *  rct2: 0x0076684C
 */
static void top_spin_paint_setup(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement)
{
	if (trackSequence > 8) return;

	typedef struct {
		uint8 fences;
		sint8 vehicle_offset_x;
		sint8 vehicle_offset_y;
		uint8 max_height;
		uint8 mapDirection[4];
		uint16 unk_args[12];
	} tile_info;

	const static tile_info TopSpinTrackSeqFenceMap[] = {
		{ 0,                  0,   0, 112, 0, 0, 0, 0, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
		{ DF_NE | DF_NW,     32,  32, 110, 1, 3, 7, 6, 2,          32, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,      2,     32,      2,     32, 0xFFFF, 0xFFFF },
		{ DF_NE,              0,   0, 110, 2, 5, 8, 4, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
		{ DF_NE | DF_SE,     32, -32, 110, 3, 7, 6, 1, 0xFFFF, 0xFFFF,      2,     32, 0xFFFF, 0xFFFF, 0xFFFF,      2,     32, 0xFFFF,      2,     32 },
		{ DF_NW,              0,   0, 110, 4, 2, 5, 8, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
		{ DF_SE,              0, -32, 112, 5, 8, 4, 2, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
		{ DF_SW | DF_NW,    -32,  32, 110, 6, 1, 3, 7, 0xFFFF,      2,     32, 0xFFFF, 0xFFFF, 0xFFFF,      2,     32, 0xFFFF,      2,     32, 0xFFFF },
		{ DF_SW | DF_SE,    -32, -32, 110, 7, 6, 1, 3, 0xFFFF, 0xFFFF, 0xFFFF,      2,     32, 0xFFFF, 0xFFFF, 0xFFFF,      2,     32,      2,     32 },
		{ DF_SW,            -32,   0, 112, 8, 4, 2, 5, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
	};

	wooden_a_supports_paint_setup(direction & 1, 0, height, RCT2_GLOBAL(0x00F441A0, uint32), NULL);
	floor_paint_setup(0, RCT2_GLOBAL(0x00F44198, uint32), height);
	uint8 fences = TopSpinTrackSeqFenceMap[trackSequence].fences;
	fences_with_check_paint_setup(fences, direction, height, mapElement);

	int correctedSequence = TopSpinTrackSeqFenceMap[trackSequence].mapDirection[direction];
	const tile_info *ti = &TopSpinTrackSeqFenceMap[correctedSequence];

	if (ti->vehicle_offset_x != 0 || ti->vehicle_offset_y != 0) {
		top_spin_paint_vehicle(ti->vehicle_offset_x, ti->vehicle_offset_y, rideIndex, direction, height, mapElement);
	}

	RCT2_GLOBAL(0x141E9B4, uint16) = top_spin_paint_setup_transform(ti->unk_args[ 0], height);
	RCT2_GLOBAL(0x141E9B6, uint16) = top_spin_paint_setup_transform(ti->unk_args[ 1], height);
	RCT2_GLOBAL(0x141E9B8, uint16) = top_spin_paint_setup_transform(ti->unk_args[ 2], height);
	RCT2_GLOBAL(0x141E9BC, uint16) = top_spin_paint_setup_transform(ti->unk_args[ 3], height);
	RCT2_GLOBAL(0x141E9C0, uint16) = top_spin_paint_setup_transform(ti->unk_args[ 4], height);
	RCT2_GLOBAL(0x141E9C4, uint16) = top_spin_paint_setup_transform(ti->unk_args[ 5], height);
	RCT2_GLOBAL(0x141E9C8, uint16) = top_spin_paint_setup_transform(ti->unk_args[ 6], height);
	RCT2_GLOBAL(0x141E9CA, uint16) = top_spin_paint_setup_transform(ti->unk_args[ 7], height);
	RCT2_GLOBAL(0x141E9CC, uint16) = top_spin_paint_setup_transform(ti->unk_args[ 8], height);
	RCT2_GLOBAL(0x141E9CE, uint16) = top_spin_paint_setup_transform(ti->unk_args[ 9], height);
	RCT2_GLOBAL(0x141E9D0, uint16) = top_spin_paint_setup_transform(ti->unk_args[10], height);
	RCT2_GLOBAL(0x141E9D4, uint16) = top_spin_paint_setup_transform(ti->unk_args[11], height);

	int maxHeightOffset = ti->max_height;
	update_tile_max_height((sint16)(height + maxHeightOffset), 32);
}

/* 0x0076659C */
TRACK_PAINT_FUNCTION get_track_paint_function_topspin(int trackType, int direction)
{
	switch (trackType) {
	case 123: return top_spin_paint_setup;
	}
	return NULL;
}

/**
 *
 *  rct2: 0x00761378
 *  rct2: 0x007614DB
 *  rct2: 0x0076163F
 *  rct2: 0x007617A5
 */
static void shop_paint_setup(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement)
{
	bool hasSupports = wooden_a_supports_paint_setup(direction & 1, 0, height, RCT2_GLOBAL(0x00F441A4, uint32), NULL);

	RCT2_GLOBAL(0x0141E9D0, sint16) = -1;
	RCT2_GLOBAL(0x0141E9C4, sint16) = -1;
	RCT2_GLOBAL(0x0141E9CC, sint16) = -1;
	RCT2_GLOBAL(0x0141E9B8, sint16) = -1;
	RCT2_GLOBAL(0x0141E9BC, sint16) = -1;
	RCT2_GLOBAL(0x0141E9B4, sint16) = -1;
	RCT2_GLOBAL(0x0141E9C0, sint16) = -1;
	RCT2_GLOBAL(0x0141E9C8, sint16) = -1;
	RCT2_GLOBAL(0x0141E9D4, sint16) = -1;

	rct_ride *ride = get_ride(rideIndex);
	rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
	rct_ride_entry_vehicle *firstVehicleEntry = &rideEntry->vehicles[0];

	uint32 imageId = RCT2_GLOBAL(0x00F44198, uint32);
	if (imageId & 0x80000000) {
		imageId &= 0x60FFFFFF;
	}
	imageId += firstVehicleEntry->base_image_id;
	imageId += direction;

	sint16 height16 = (sint16)height;
	int rotation = get_current_rotation();
	if (hasSupports) {
		uint32 foundationImageId = RCT2_GLOBAL(0x00F441A4, uint32);
		foundationImageId |= 3395;

		RCT2_GLOBAL(0x009DEA52, uint16) = 2;
		RCT2_GLOBAL(0x009DEA54, uint16) = 2;
		RCT2_GLOBAL(0x009DEA56, sint16) = height16;
		sub_98197C(foundationImageId, 0, 0, 28, 28, 45, height, rotation);

		RCT2_GLOBAL(0x009DEA52, uint16) = 2;
		RCT2_GLOBAL(0x009DEA54, uint16) = 2;
		RCT2_GLOBAL(0x009DEA56, sint16) = height16;
		sub_98199C(imageId, 0, 0, 28, 28, 45, height, rotation);
	} else {
		RCT2_GLOBAL(0x009DEA52, uint16) = 2;
		RCT2_GLOBAL(0x009DEA54, uint16) = 2;
		RCT2_GLOBAL(0x009DEA56, sint16) = height16;
		sub_98197C(imageId, 0, 0, 28, 28, 45, height, rotation);
	}

	update_tile_max_height(height16 + 48, 32);
}

/* 0x00761160 */
TRACK_PAINT_FUNCTION get_track_paint_function_shop(int trackType, int direction)
{
	switch (trackType) {
	case 118:
	case 121: return shop_paint_setup;
	}
	return NULL;
}

/**
 *
 *  rct2: 0x00763234
 *  rct2: 0x0076338C
 *  rct2: 0x00762F50
 *  rct2: 0x007630DE
 */
static void facility_paint_setup(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement)
{
	bool hasSupports = wooden_a_supports_paint_setup(direction & 1, 0, height, RCT2_GLOBAL(0x00F441A4, uint32), NULL);

	RCT2_GLOBAL(0x0141E9D0, sint16) = -1;
	RCT2_GLOBAL(0x0141E9C4, sint16) = -1;
	RCT2_GLOBAL(0x0141E9CC, sint16) = -1;
	RCT2_GLOBAL(0x0141E9B8, sint16) = -1;
	RCT2_GLOBAL(0x0141E9BC, sint16) = -1;
	RCT2_GLOBAL(0x0141E9B4, sint16) = -1;
	RCT2_GLOBAL(0x0141E9C0, sint16) = -1;
	RCT2_GLOBAL(0x0141E9C8, sint16) = -1;
	RCT2_GLOBAL(0x0141E9D4, sint16) = -1;

	rct_ride *ride = get_ride(rideIndex);
	rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
	rct_ride_entry_vehicle *firstVehicleEntry = &rideEntry->vehicles[0];

	uint32 imageId = RCT2_GLOBAL(0x00F44198, uint32);
	imageId |= firstVehicleEntry->base_image_id;
	imageId += (direction + 2) & 3;

	sint16 height16 = (sint16)height;
	int rotation = get_current_rotation();
	int lengthX = (direction & 1) == 0 ? 28 : 2;
	int lengthY = (direction & 1) == 0 ? 2 : 28;
	if (hasSupports) {
		uint32 foundationImageId = RCT2_GLOBAL(0x00F441A4, uint32);
		foundationImageId |= 3395;

		RCT2_GLOBAL(0x009DEA52, uint16) = direction == 3 ? 28 : 2;
		RCT2_GLOBAL(0x009DEA54, uint16) = direction == 0 ? 28 : 2;
		RCT2_GLOBAL(0x009DEA56, sint16) = height16;
		sub_98197C(foundationImageId, 0, 0, lengthX, lengthY, 29, height, rotation);

		// Door image or base
		RCT2_GLOBAL(0x009DEA52, uint16) = direction == 3 ? 28 : 2;
		RCT2_GLOBAL(0x009DEA54, uint16) = direction == 0 ? 28 : 2;
		RCT2_GLOBAL(0x009DEA56, sint16) = height16;
		sub_98199C(imageId, 0, 0, lengthX, lengthY, 29, height, rotation);
	} else {
		// Door image or base
		RCT2_GLOBAL(0x009DEA52, uint16) = direction == 3 ? 28 : 2;
		RCT2_GLOBAL(0x009DEA54, uint16) = direction == 0 ? 28 : 2;
		RCT2_GLOBAL(0x009DEA56, sint16) = height16;

		sub_98197C(imageId, 0, 0, lengthX, lengthY, 29, height, rotation);
	}

	// Base image if door was drawn
	if (direction == 1) {
		imageId += 2;
		RCT2_GLOBAL(0x009DEA52, uint16) = 28;
		RCT2_GLOBAL(0x009DEA54, uint16) = 2;
		RCT2_GLOBAL(0x009DEA56, sint16) = height16;
		sub_98197C(imageId, 0, 0, 2, 28, 29, height, rotation);
	} else if (direction == 2) {
		imageId += 4;
		RCT2_GLOBAL(0x009DEA52, uint16) = 2;
		RCT2_GLOBAL(0x009DEA54, uint16) = 28;
		RCT2_GLOBAL(0x009DEA56, sint16) = height16;
		sub_98197C(imageId, 0, 0, 28, 2, 29, height, rotation);
	}

	update_tile_max_height(height16 + 32, 32);
}

/* 0x00762D44 */
TRACK_PAINT_FUNCTION get_track_paint_function_facility(int trackType, int direction)
{
	switch (trackType) {
	case 118: return facility_paint_setup;
	}
	return NULL;
}

/**
 *
 *  rct2: 0x0076522A
 */
static void motionsimulator_paint_setup_vehicle(sint8 offsetX, sint8 offsetY, uint8 direction, int height, rct_map_element* mapElement)
{
	rct_ride *ride = get_ride(mapElement->properties.track.ride_index);
	rct_ride_entry *rideEntry = get_ride_entry_by_ride(ride);

	uint32 backup_dword_9DE578 = RCT2_GLOBAL(0x009DE578, uint32);
	// push dword_9DE578
	height += 2;
	rct_vehicle *vehicle = NULL;
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK) {
		uint16 spriteIndex = ride->vehicles[0];
		if (spriteIndex != SPRITE_INDEX_NULL) {
			vehicle = GET_VEHICLE(spriteIndex);
			RCT2_GLOBAL(0x009DE570, uint8) = 2;
			RCT2_GLOBAL(0x009DE578, rct_vehicle*) = vehicle;
		}
	}

	uint32 imageId = rideEntry->vehicles[0].base_image_id + direction;
	if (vehicle != NULL) {
		if (vehicle->restraints_position >= 64) {
			imageId += (vehicle->restraints_position >> 6) << 2;
		} else {
			imageId += vehicle->vehicle_sprite_type * 4;
		}
	}

	RCT2_GLOBAL(0x01428090, uint32) = imageId;
	RCT2_GLOBAL(0x01428094, uint32) = direction;
	imageId = RCT2_GLOBAL(0x00F441A0, uint32);
	if (imageId == 0x20000000) {
		imageId = (IMAGE_TYPE_UNKNOWN | IMAGE_TYPE_USE_PALETTE) << 28;
		imageId |= ride->vehicle_colours[0].trim_colour << 19;
		imageId |= ride->vehicle_colours[0].body_colour << 24;
	}
	imageId |= RCT2_GLOBAL(0x01428090, uint32);
	uint32 simulatorImageId = imageId;

	RCT2_GLOBAL(0x009DEA52, sint16) = offsetX;
	RCT2_GLOBAL(0x009DEA54, sint16) = offsetY;
	RCT2_GLOBAL(0x009DEA56, sint16) = height;

	uint8 currentRotation = get_current_rotation();
	switch (direction) {
	case 0:
		// Simulator
		imageId = simulatorImageId;
		sub_98197C(imageId, offsetX, offsetY, 20, 20, 44, height, currentRotation);
		// Stairs
		imageId = (22154 + direction) | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98199C(imageId, offsetX, offsetY, 20, 20, 44, height, currentRotation);
		// Stairs (rail)
		RCT2_GLOBAL(0x009DEA54, uint16) += 32;
		imageId = (22158 + direction) | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98197C(imageId, offsetX, offsetY, 20, 2, 44, height, currentRotation);
		break;
	case 1:
		// Simulator
		imageId = simulatorImageId;
		sub_98197C(imageId, offsetX, offsetY, 20, 20, 44, height, currentRotation);
		// Stairs
		uint32 imageId = (22154 + direction) | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98199C(imageId, offsetX, offsetY, 20, 20, 44, height, currentRotation);
		// Stairs (rail)
		RCT2_GLOBAL(0x009DEA52, uint16) += 34;
		imageId = (22158 + direction) | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98197C(imageId, offsetX, offsetY, 20, 2, 44, height, currentRotation);
		break;
	case 2:
		// Stairs (rail)
		RCT2_GLOBAL(0x009DEA54, uint16) -= 10;
		imageId = (22158 + direction) | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98197C(imageId, offsetX, offsetY, 20, 2, 44, height, currentRotation);
		// Stairs
		RCT2_GLOBAL(0x009DEA54, uint16) += 15;
		imageId = (22154 + direction) | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98199C(imageId, offsetX, offsetY, 20, 20, 44, height, currentRotation);
		// Simulator
		imageId = simulatorImageId;
		sub_98199C(imageId, offsetX, offsetY, 20, 20, 44, height, currentRotation);
		break;
	case 3:
		// Stairs (rail)
		RCT2_GLOBAL(0x009DEA52, uint16) -= 10;
		imageId = (22158 + direction) | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98197C(imageId, offsetX, offsetY, 20, 2, 44, height, currentRotation);
		// Stairs
		RCT2_GLOBAL(0x009DEA52, uint16) += 15;
		imageId = (22154 + direction) | RCT2_GLOBAL(0x00F441A0, uint32);
		sub_98197C(imageId, offsetX, offsetY, 20, 20, 44, height, currentRotation);
		// Simulator
		imageId = simulatorImageId;
		sub_98199C(imageId, offsetX, offsetY, 20, 20, 44, height, currentRotation);
		break;
	}

	// pop dword_9DE578
	RCT2_GLOBAL(0x009DE578, uint32) = backup_dword_9DE578;
	RCT2_GLOBAL(0x009DE570, uint8) = 3;
}

/**
 *
 *  rct2: 0x0076370C
 *  rct2: 0x00763DD2
 *  rct2: 0x0076449A
 *  rct2: 0x00764B62
 */
static void motionsimulator_paint_setup(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element* mapElement)
{
	const static struct {
		uint8 fences;
		rct_xy8 offsets[4];
	} DirectionInfo[] = {
		{ DF_NW | DF_NE,   0,   0,   0,   0,   0,   0,   0,   0 },
		{ DF_NE | DF_SE,  16, -16, -16, -16, -16,  16,  16,  16 },
		{ DF_NW | DF_SW, -16,  16,  16,  16,  16, -16, -16, -16 },
		{ DF_SW | DF_SE, -16, -16, -16,  16,  16,  16,  16, -16 },
	};

	if (trackSequence > 3) return;

	wooden_a_supports_paint_setup(direction & 1, 0, height, RCT2_GLOBAL(0x00F441A0, uint32), NULL);
	floor_paint_setup(0, RCT2_GLOBAL(0x00F4419C, uint32), height);

	uint8 fences = DirectionInfo[trackSequence].fences;
	fences_with_check_paint_setup(fences, direction, height, mapElement);

	rct_xy8 offset;
	switch (trackSequence) {
	case 1:
	case 2:
	case 3:
		offset = DirectionInfo[trackSequence].offsets[direction];
		motionsimulator_paint_setup_vehicle((sint8)offset.x, (sint8)offset.y, direction, height, mapElement);
		break;
	}

	RCT2_GLOBAL(0x0141E9D4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9C4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9C8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9B8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9BC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9B4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9C0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9CC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x0141E9D0, uint16) = 0xFFFF;

	update_tile_max_height(height + 128, 32);
}

/* 0x00763520 */
TRACK_PAINT_FUNCTION get_track_paint_function_motionsimulator(int trackType, int direction)
{
	switch (trackType) {
	case 110: return motionsimulator_paint_setup;
	}
	return NULL;
}
