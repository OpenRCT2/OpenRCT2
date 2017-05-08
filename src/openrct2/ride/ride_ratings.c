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

#include "../interface/window.h"
#include "../localisation/date.h"
#include "../rct2.h"
#include "../world/map.h"
#include "ride.h"
#include "ride_data.h"
#include "ride_ratings.h"
#include "track.h"

enum {
	RIDE_RATINGS_STATE_FIND_NEXT_RIDE,
	RIDE_RATINGS_STATE_INITIALISE,
	RIDE_RATINGS_STATE_2,
	RIDE_RATINGS_STATE_CALCULATE,
	RIDE_RATINGS_STATE_4,
	RIDE_RATINGS_STATE_5
};

enum {
	PROXIMITY_WATER_OVER,						// 0x0138B596
	PROXIMITY_WATER_TOUCH,						// 0x0138B598
	PROXIMITY_WATER_LOW,						// 0x0138B59A
	PROXIMITY_WATER_HIGH,						// 0x0138B59C
	PROXIMITY_SURFACE_TOUCH,					// 0x0138B59E
	PROXIMITY_PATH_OVER,						// 0x0138B5A0
	PROXIMITY_PATH_TOUCH_ABOVE,					// 0x0138B5A2
	PROXIMITY_PATH_TOUCH_UNDER,					// 0x0138B5A4
	PROXIMITY_138B5A6,							// 0x0138B5A6
	PROXIMITY_138B5A8,							// 0x0138B5A8
	PROXIMITY_OWN_TRACK_TOUCH_ABOVE,			// 0x0138B5AA
	PROXIMITY_OWN_TRACK_CLOSE_ABOVE,			// 0x0138B5AC
	PROXIMITY_FOREIGN_TRACK_ABOVE_OR_BELOW,		// 0x0138B5AE
	PROXIMITY_FOREIGN_TRACK_TOUCH_ABOVE,		// 0x0138B5B0
	PROXIMITY_FOREIGN_TRACK_CLOSE_ABOVE,		// 0x0138B5B2
	PROXIMITY_SCENERY_SIDE_BELOW,				// 0x0138B5B4
	PROXIMITY_SCENERY_SIDE_ABOVE,				// 0x0138B5B6
	PROXIMITY_OWN_STATION_TOUCH_ABOVE,			// 0x0138B5B8
	PROXIMITY_OWN_STATION_CLOSE_ABOVE,			// 0x0138B5BA
	PROXIMITY_TRACK_THROUGH_VERTICAL_LOOP,		// 0x0138B5BC
	PROXIMITY_PATH_TROUGH_VERTICAL_LOOP,		// 0x0138B5BE
	PROXIMITY_INTERSECTING_VERTICAL_LOOP,		// 0x0138B5C0
	PROXIMITY_THROUGH_VERTICAL_LOOP,			// 0x0138B5C2
	PROXIMITY_PATH_SIDE_CLOSE,					// 0x0138B5C4
	PROXIMITY_FOREIGN_TRACK_SIDE_CLOSE,			// 0x0138B5C6
	PROXIMITY_SURFACE_SIDE_CLOSE,				// 0x0138B5C8
	PROXIMITY_COUNT
};

typedef void (*ride_ratings_calculation)(rct_ride *ride);

rct_ride_rating_calc_data gRideRatingsCalcData;

static const ride_ratings_calculation ride_ratings_calculate_func_table[RIDE_TYPE_COUNT];

static void ride_ratings_update_state_0();
static void ride_ratings_update_state_1();
static void ride_ratings_update_state_2();
static void ride_ratings_update_state_3();
static void ride_ratings_update_state_4();
static void ride_ratings_update_state_5();
static void ride_ratings_begin_proximity_loop();
static void ride_ratings_calculate(rct_ride *ride);
static void ride_ratings_calculate_value(rct_ride *ride);
static void ride_ratings_score_close_proximity(rct_map_element *mapElement);

/**
 *
 *  rct2: 0x006B5A2A
 */
void ride_ratings_update_all()
{
	if (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR)
		return;

	switch (gRideRatingsCalcData.state) {
	case RIDE_RATINGS_STATE_FIND_NEXT_RIDE:
		ride_ratings_update_state_0();
		break;
	case RIDE_RATINGS_STATE_INITIALISE:
		ride_ratings_update_state_1();
		break;
	case RIDE_RATINGS_STATE_2:
		ride_ratings_update_state_2();
		break;
	case RIDE_RATINGS_STATE_CALCULATE:
		ride_ratings_update_state_3();
		break;
	case RIDE_RATINGS_STATE_4:
		ride_ratings_update_state_4();
		break;
	case RIDE_RATINGS_STATE_5:
		ride_ratings_update_state_5();
		break;
	}
}

/**
 *
 *  rct2: 0x006B5A5C
 */
static void ride_ratings_update_state_0()
{
	sint32 currentRide = gRideRatingsCalcData.current_ride;

	currentRide++;
	if (currentRide == 255) {
		currentRide = 0;
	}

	rct_ride *ride = get_ride(currentRide);
	if (ride->type != RIDE_TYPE_NULL && ride->status != RIDE_STATUS_CLOSED) {
		gRideRatingsCalcData.state = RIDE_RATINGS_STATE_INITIALISE;
	}
	gRideRatingsCalcData.current_ride = currentRide;
}

/**
 *
 *  rct2: 0x006B5A94
 */
static void ride_ratings_update_state_1()
{
	gRideRatingsCalcData.proximity_total = 0;
	for (sint32 i = 0; i < PROXIMITY_COUNT; i++) {
		gRideRatingsCalcData.proximity_scores[i] = 0;
	}
	gRideRatingsCalcData.num_brakes = 0;
	gRideRatingsCalcData.num_reversers = 0;
	gRideRatingsCalcData.state = RIDE_RATINGS_STATE_2;
	gRideRatingsCalcData.station_flags = 0;
	ride_ratings_begin_proximity_loop();
}

/**
 *
 *  rct2: 0x006B5C66
 */
static void ride_ratings_update_state_2()
{
	rct_ride *ride = get_ride(gRideRatingsCalcData.current_ride);
	if (ride->type == RIDE_TYPE_NULL || ride->status == RIDE_STATUS_CLOSED) {
		gRideRatingsCalcData.state = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
		return;
	}

	sint32 x = gRideRatingsCalcData.proximity_x / 32;
	sint32 y = gRideRatingsCalcData.proximity_y / 32;
	sint32 z = gRideRatingsCalcData.proximity_z / 8;
	sint32 trackType = gRideRatingsCalcData.proximity_track_type;

	rct_map_element *mapElement = map_get_first_element_at(x, y);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;
		if (mapElement->base_height != z)
			continue;

		if (trackType == 255 || ((mapElement->properties.track.sequence & 0x0F) == 0 && trackType == mapElement->properties.track.type)) {
			if (trackType == TRACK_ELEM_END_STATION) {
				sint32 entranceIndex = map_get_station(mapElement);
				gRideRatingsCalcData.station_flags &= ~RIDE_RATING_STATION_FLAG_NO_ENTRANCE;
				if (ride->entrances[entranceIndex] == 0xFFFF) {
					gRideRatingsCalcData.station_flags |= RIDE_RATING_STATION_FLAG_NO_ENTRANCE;
				}
			}

			ride_ratings_score_close_proximity(mapElement);

			rct_xy_element trackElement = {
				.x = gRideRatingsCalcData.proximity_x,
				.y = gRideRatingsCalcData.proximity_y,
				.element = mapElement
			};
			rct_xy_element nextTrackElement;
			if (!track_block_get_next(&trackElement, &nextTrackElement, NULL, NULL)) {
				gRideRatingsCalcData.state = RIDE_RATINGS_STATE_4;
				return;
			}

			x = nextTrackElement.x;
			y = nextTrackElement.y;
			z = nextTrackElement.element->base_height * 8;
			mapElement = nextTrackElement.element;
			if (x == gRideRatingsCalcData.proximity_start_x && y == gRideRatingsCalcData.proximity_start_y && z == gRideRatingsCalcData.proximity_start_z) {
				gRideRatingsCalcData.state = RIDE_RATINGS_STATE_CALCULATE;
				return;
			}
			gRideRatingsCalcData.proximity_x = x;
			gRideRatingsCalcData.proximity_y = y;
			gRideRatingsCalcData.proximity_z = z;
			gRideRatingsCalcData.proximity_track_type = mapElement->properties.track.type;
			return;
		}
	} while (!map_element_is_last_for_tile(mapElement++));

	gRideRatingsCalcData.state = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
}

/**
 *
 *  rct2: 0x006B5E4D
 */
static void ride_ratings_update_state_3()
{
	rct_ride *ride = get_ride(gRideRatingsCalcData.current_ride);
	if (ride->type == RIDE_TYPE_NULL || ride->status == RIDE_STATUS_CLOSED) {
		gRideRatingsCalcData.state = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
		return;
	}

	ride_ratings_calculate(ride);
	ride_ratings_calculate_value(ride);

	window_invalidate_by_number(WC_RIDE, gRideRatingsCalcData.current_ride);
	gRideRatingsCalcData.state = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
}

/**
 *
 *  rct2: 0x006B5BAB
 */
static void ride_ratings_update_state_4()
{
	gRideRatingsCalcData.state = RIDE_RATINGS_STATE_5;
	ride_ratings_begin_proximity_loop();
}

/**
 *
 *  rct2: 0x006B5D72
 */
static void ride_ratings_update_state_5()
{
	rct_ride *ride = get_ride(gRideRatingsCalcData.current_ride);
	if (ride->type == RIDE_TYPE_NULL || ride->status == RIDE_STATUS_CLOSED) {
		gRideRatingsCalcData.state = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
		return;
	}

	sint32 x = gRideRatingsCalcData.proximity_x / 32;
	sint32 y = gRideRatingsCalcData.proximity_y / 32;
	sint32 z = gRideRatingsCalcData.proximity_z / 8;
	sint32 trackType = gRideRatingsCalcData.proximity_track_type;

	rct_map_element *mapElement = map_get_first_element_at(x, y);
	do {
		if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK)
			continue;
		if (mapElement->base_height != z)
			continue;

		if (trackType == 255 || trackType == mapElement->properties.track.type) {
			ride_ratings_score_close_proximity(mapElement);

			x = gRideRatingsCalcData.proximity_x;
			y = gRideRatingsCalcData.proximity_y;
			track_begin_end trackBeginEnd;
			if (!track_block_get_previous(x, y, mapElement, &trackBeginEnd)) {
				gRideRatingsCalcData.state = RIDE_RATINGS_STATE_CALCULATE;
				return;
			}

			x = trackBeginEnd.begin_x;
			y = trackBeginEnd.begin_y;
			z = trackBeginEnd.begin_z;
			if (x == gRideRatingsCalcData.proximity_start_x && y == gRideRatingsCalcData.proximity_start_y && z == gRideRatingsCalcData.proximity_start_z) {
				gRideRatingsCalcData.state = RIDE_RATINGS_STATE_CALCULATE;
				return;
			}
			gRideRatingsCalcData.proximity_x = x;
			gRideRatingsCalcData.proximity_y = y;
			gRideRatingsCalcData.proximity_z = z;
			gRideRatingsCalcData.proximity_track_type = trackBeginEnd.begin_element->properties.track.type;
			return;
		}
	} while (!map_element_is_last_for_tile(mapElement++));

	gRideRatingsCalcData.state = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
}

/**
 *
 *  rct2: 0x006B5BB2
 */
static void ride_ratings_begin_proximity_loop()
{
	rct_ride *ride = get_ride(gRideRatingsCalcData.current_ride);
	if (ride->type == RIDE_TYPE_NULL || ride->status == RIDE_STATUS_CLOSED) {
		gRideRatingsCalcData.state = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
		return;
	}

	if (ride->type == RIDE_TYPE_MAZE) {
		gRideRatingsCalcData.state = RIDE_RATINGS_STATE_CALCULATE;
		return;
	}

	for (sint32 i = 0; i < 4; i++) {
		if (ride->station_starts[i] != 0xFFFF) {
			gRideRatingsCalcData.station_flags &= ~RIDE_RATING_STATION_FLAG_NO_ENTRANCE;
			if (ride->entrances[i] == 0xFFFF) {
				gRideRatingsCalcData.station_flags |= RIDE_RATING_STATION_FLAG_NO_ENTRANCE;
			}

			sint32 x = (ride->station_starts[i] & 0xFF) * 32;
			sint32 y = (ride->station_starts[i] >> 8) * 32;
			sint32 z = ride->station_heights[i] * 8;

			gRideRatingsCalcData.proximity_x = x;
			gRideRatingsCalcData.proximity_y = y;
			gRideRatingsCalcData.proximity_z = z;
			gRideRatingsCalcData.proximity_track_type = 255;
			gRideRatingsCalcData.proximity_start_x = x;
			gRideRatingsCalcData.proximity_start_y = y;
			gRideRatingsCalcData.proximity_start_z = z;
			return;
		}
	}

	gRideRatingsCalcData.state = RIDE_RATINGS_STATE_FIND_NEXT_RIDE;
}

static void proximity_score_increment(sint32 type)
{
	gRideRatingsCalcData.proximity_scores[type]++;
}

/**
 *
 *  rct2: 0x006B6207
 */
static void ride_ratings_score_close_proximity_in_direction(rct_map_element *inputMapElement, sint32 direction)
{
	sint32 x = gRideRatingsCalcData.proximity_x + TileDirectionDelta[direction].x;
	sint32 y = gRideRatingsCalcData.proximity_y + TileDirectionDelta[direction].y;
	if (x < 0 || y < 0 || x >= (32 * 256) || y >= (32 * 256))
		return;

	rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		switch (map_element_get_type(mapElement)) {
		case MAP_ELEMENT_TYPE_SURFACE:
			if (gRideRatingsCalcData.proximity_base_height <= inputMapElement->base_height) {
				if (inputMapElement->clearance_height <= mapElement->base_height) {
					proximity_score_increment(PROXIMITY_SURFACE_SIDE_CLOSE);
				}
			}
			break;
		case MAP_ELEMENT_TYPE_PATH:
			if (abs((sint32)inputMapElement->base_height - (sint32)mapElement->base_height) <= 2) {
				proximity_score_increment(PROXIMITY_PATH_SIDE_CLOSE);
			}
			break;
		case MAP_ELEMENT_TYPE_TRACK:
			if (inputMapElement->properties.track.ride_index != mapElement->properties.track.ride_index) {
				if (abs((sint32)inputMapElement->base_height - (sint32)mapElement->base_height) <= 2) {
					proximity_score_increment(PROXIMITY_FOREIGN_TRACK_SIDE_CLOSE);
				}
			}
			break;
		case MAP_ELEMENT_TYPE_SCENERY:
		case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
			if (mapElement->base_height < inputMapElement->clearance_height) {
				if (inputMapElement->base_height > mapElement->clearance_height) {
					proximity_score_increment(PROXIMITY_SCENERY_SIDE_ABOVE);
				} else {
					proximity_score_increment(PROXIMITY_SCENERY_SIDE_BELOW);
				}
			}
			break;
		}
	} while (!map_element_is_last_for_tile(mapElement++));

}

static void ride_ratings_score_close_proximity_loops_helper(rct_map_element *inputMapElement, sint32 x, sint32 y)
{
	rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		switch (map_element_get_type(mapElement)) {
		case MAP_ELEMENT_TYPE_PATH:
		{
			sint32 zDiff = (sint32)mapElement->base_height - (sint32)inputMapElement->base_height;
			if (zDiff >= 0 && zDiff <= 16)
			{
				proximity_score_increment(PROXIMITY_PATH_TROUGH_VERTICAL_LOOP);
			}
		} break;

		case MAP_ELEMENT_TYPE_TRACK:
		{
			sint32 unk = (mapElement->type ^ inputMapElement->type) & 1;
			if (unk != 0)
			{
				sint32 zDiff = (sint32)mapElement->base_height - (sint32)inputMapElement->base_height;
				if (zDiff >= 0 && zDiff <= 16)
				{
					proximity_score_increment(PROXIMITY_TRACK_THROUGH_VERTICAL_LOOP);
					if (
						mapElement->properties.track.type == TRACK_ELEM_LEFT_VERTICAL_LOOP ||
						mapElement->properties.track.type == TRACK_ELEM_RIGHT_VERTICAL_LOOP
						)
					{
						proximity_score_increment(PROXIMITY_INTERSECTING_VERTICAL_LOOP);
					}
				}
			}
		} break;
		}
	} while (!map_element_is_last_for_tile(mapElement++));
}

/**
 *
 *  rct2: 0x006B62DA
 */
static void ride_ratings_score_close_proximity_loops(rct_map_element *inputMapElement)
{
	sint32 trackType = inputMapElement->properties.track.type;
	if (trackType == TRACK_ELEM_LEFT_VERTICAL_LOOP || trackType == TRACK_ELEM_RIGHT_VERTICAL_LOOP) {
		sint32 x = gRideRatingsCalcData.proximity_x;
		sint32 y = gRideRatingsCalcData.proximity_y;
		ride_ratings_score_close_proximity_loops_helper(inputMapElement, x, y);

		sint32 direction = inputMapElement->type & MAP_ELEMENT_DIRECTION_MASK;
		x = gRideRatingsCalcData.proximity_x + TileDirectionDelta[direction].x;
		y = gRideRatingsCalcData.proximity_y + TileDirectionDelta[direction].y;
		ride_ratings_score_close_proximity_loops_helper(inputMapElement, x, y);
	}
}

/**
 *
 *  rct2: 0x006B5F9D
 */
static void ride_ratings_score_close_proximity(rct_map_element *inputMapElement)
{
	if (gRideRatingsCalcData.station_flags & RIDE_RATING_STATION_FLAG_NO_ENTRANCE) {
		return;
	}

	gRideRatingsCalcData.proximity_total++;
	sint32 x = gRideRatingsCalcData.proximity_x;
	sint32 y = gRideRatingsCalcData.proximity_y;
	rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
	do {
		switch (map_element_get_type(mapElement)) {
		case MAP_ELEMENT_TYPE_SURFACE:
			gRideRatingsCalcData.proximity_base_height = mapElement->base_height;
			if (mapElement->base_height * 8 == gRideRatingsCalcData.proximity_z) {
				proximity_score_increment(PROXIMITY_SURFACE_TOUCH);
			}
			sint32 waterHeight = (mapElement->properties.surface.terrain & 0x1F);
			if (waterHeight != 0) {
				sint32 z = waterHeight * 16;
				if (z <= gRideRatingsCalcData.proximity_z) {
					proximity_score_increment(PROXIMITY_WATER_OVER);
					if (z == gRideRatingsCalcData.proximity_z) {
						proximity_score_increment(PROXIMITY_WATER_TOUCH);
					}
					z += 16;
					if (z == gRideRatingsCalcData.proximity_z) {
						proximity_score_increment(PROXIMITY_WATER_LOW);
					}
					z += 112;
					if (z <= gRideRatingsCalcData.proximity_z) {
						proximity_score_increment(PROXIMITY_WATER_HIGH);
					}
				}
			}
			break;
		case MAP_ELEMENT_TYPE_PATH:
			if (mapElement->properties.path.type & 0xF0) {
				if (mapElement->clearance_height == inputMapElement->base_height) {
					proximity_score_increment(PROXIMITY_138B5A6);
				}
				if (mapElement->base_height == inputMapElement->clearance_height) {
					proximity_score_increment(PROXIMITY_138B5A8);
				}
			} else {
				if (mapElement->clearance_height <= inputMapElement->base_height) {
					proximity_score_increment(PROXIMITY_PATH_OVER);
				}
				if (mapElement->clearance_height == inputMapElement->base_height) {
					proximity_score_increment(PROXIMITY_PATH_TOUCH_ABOVE);
				}
				if (mapElement->base_height == inputMapElement->clearance_height) {
					proximity_score_increment(PROXIMITY_PATH_TOUCH_UNDER);
				}
			}
			break;
		case MAP_ELEMENT_TYPE_TRACK:
		{
			sint32 trackType = mapElement->properties.track.type;
			if (trackType == TRACK_ELEM_LEFT_VERTICAL_LOOP || trackType == TRACK_ELEM_RIGHT_VERTICAL_LOOP) {
				sint32 sequence = mapElement->properties.track.sequence & 0x0F;
				if (sequence == 3 || sequence == 6) {
					if (mapElement->base_height - inputMapElement->clearance_height <= 10) {
						proximity_score_increment(PROXIMITY_THROUGH_VERTICAL_LOOP);
					}
				}
			}
			if (inputMapElement->properties.track.ride_index != mapElement->properties.track.ride_index) {
				proximity_score_increment(PROXIMITY_FOREIGN_TRACK_ABOVE_OR_BELOW);
				if (mapElement->clearance_height == inputMapElement->base_height) {
					proximity_score_increment(PROXIMITY_FOREIGN_TRACK_TOUCH_ABOVE);
				}
				if (mapElement->clearance_height + 2 <= inputMapElement->base_height) {
					if (mapElement->clearance_height + 10 >= inputMapElement->base_height) {
						proximity_score_increment(PROXIMITY_FOREIGN_TRACK_CLOSE_ABOVE);
					}
				}
				if (inputMapElement->clearance_height == mapElement->base_height) {
					proximity_score_increment(PROXIMITY_FOREIGN_TRACK_TOUCH_ABOVE);
				}
				if (inputMapElement->clearance_height + 2 == mapElement->base_height) {
					if ((uint8)(inputMapElement->clearance_height + 10) >= mapElement->base_height) {
						proximity_score_increment(PROXIMITY_FOREIGN_TRACK_CLOSE_ABOVE);
					}
				}
			} else {
				trackType = mapElement->properties.track.type;
				bool isStation = (
					trackType == TRACK_ELEM_END_STATION ||
					trackType == TRACK_ELEM_MIDDLE_STATION ||
					trackType == TRACK_ELEM_BEGIN_STATION
				);
				if (mapElement->clearance_height == inputMapElement->base_height) {
					proximity_score_increment(PROXIMITY_OWN_TRACK_TOUCH_ABOVE);
					if (isStation) {
						proximity_score_increment(PROXIMITY_OWN_STATION_TOUCH_ABOVE);
					}
				}
				if (mapElement->clearance_height + 2 <= inputMapElement->base_height) {
					if (mapElement->clearance_height + 10 >= inputMapElement->base_height) {
						proximity_score_increment(PROXIMITY_OWN_TRACK_CLOSE_ABOVE);
						if (isStation) {
							proximity_score_increment(PROXIMITY_OWN_STATION_CLOSE_ABOVE);
						}
					}
				}

				if (inputMapElement->clearance_height == mapElement->base_height) {
					proximity_score_increment(PROXIMITY_OWN_TRACK_TOUCH_ABOVE);
					if (isStation) {
						proximity_score_increment(PROXIMITY_OWN_STATION_TOUCH_ABOVE);
					}
				}
				if (inputMapElement->clearance_height + 2 <= mapElement->base_height) {
					if (inputMapElement->clearance_height + 10 >= mapElement->base_height) {
						proximity_score_increment(PROXIMITY_OWN_TRACK_CLOSE_ABOVE);
						if (isStation) {
							proximity_score_increment(PROXIMITY_OWN_STATION_CLOSE_ABOVE);
						}
					}
				}
			}
		} break;
		} // switch map_element_get_type
	} while (!map_element_is_last_for_tile(mapElement++));

	uint8 direction = inputMapElement->type & MAP_ELEMENT_DIRECTION_MASK;
	ride_ratings_score_close_proximity_in_direction(inputMapElement, (direction + 1) & 3);
	ride_ratings_score_close_proximity_in_direction(inputMapElement, (direction - 1) & 3);
	ride_ratings_score_close_proximity_loops(inputMapElement);

	switch (gRideRatingsCalcData.proximity_track_type) {
	case TRACK_ELEM_BRAKES:
		gRideRatingsCalcData.num_brakes++;
		break;
	case TRACK_ELEM_LEFT_REVERSER:
	case TRACK_ELEM_RIGHT_REVERSER:
		gRideRatingsCalcData.num_reversers++;
		break;
	}
}

static void ride_ratings_calculate(rct_ride *ride)
{
	ride_ratings_calculation calcFunc = ride_ratings_calculate_func_table[ride->type];
	if (calcFunc != NULL) {
		calcFunc(ride);
	}

#ifdef ORIGINAL_RATINGS
	if (ride->ratings.excitement != -1) {
		// Address underflows allowed by original RCT2 code
		ride->ratings.excitement = max(0, ride->ratings.excitement);
		ride->ratings.intensity = max(0, ride->ratings.intensity);
		ride->ratings.nausea = max(0, ride->ratings.nausea);
	}
#endif
}

static void ride_ratings_calculate_value(rct_ride *ride)
{
	typedef struct row {
		sint32 months, multiplier, divisor, summand;
	} row;

	static const row age_table_new[] = {
		{5,         3,          2,          0},   // 1.5x
		{13,        6,          5,          0},   // 1.2x
		{40,        1,          1,          0},   // 1x
		{64,        3,          4,          0},   // 0.75x
		{88,        9,          16,         0},   // 0.56x
		{104,       27,         64,         0},   // 0.42x
		{120,       81,         256,        0},   // 0.32x
		{128,       81,         512,        0},   // 0.16x
		{200,       81,         1024,       0},   // 0.08x
		{200,       9,          16,         0}    // 0.56x "easter egg"
		};

#ifdef ORIGINAL_RATINGS
	static const row age_table_old[] = {
		{5,         1,          1,          30},  // +30
		{13,        1,          1,          10},  // +10
		{40,        1,          1,          0},   // 1x
		{64,        3,          4,          0},   // 0.75x
		{88,        9,          16,         0},   // 0.56x
		{104,       27,         64,         0},   // 0.42x
		{120,       81,         256,        0},   // 0.32x
		{128,       81,         512,        0},   // 0.16x
		{200,       81,         1024,       0},   // 0.08x
		{200,       9,          16,         0}    // 0.56x "easter egg"
		};
#endif


	if (!ride_has_ratings(ride)) {
		return;
	}

	// Start with the base ratings, multiplied by the ride type specific weights for excitement, intensity and nausea.
	sint32 value =
		(((ride->excitement * RideRatings[ride->type].excitement) * 32) >> 15) +
		(((ride->intensity  * RideRatings[ride->type].intensity) * 32) >> 15) +
		(((ride->nausea     * RideRatings[ride->type].nausea) * 32) >> 15);

	sint32 monthsOld = gDateMonthsElapsed - ride->build_date;

	const row *age_table = age_table_new;
	sint32 table_size = countof(age_table_new);

#ifdef ORIGINAL_RATINGS
	age_table = age_table_old;
	table_size = countof(age_table_old);
#endif

	row last_row = age_table[table_size-1];

	// Ride is older than oldest age in the table?
	if(monthsOld >= last_row.months) {
		value = (value * last_row.multiplier) / last_row.divisor + last_row.summand;
	}
	else {
		// Find the first hit in the table that matches this ride's age
		for(sint32 it = 0; it < table_size; it++) {
			row curr = age_table[it];

			if(monthsOld < curr.months) {
				value = (value * curr.multiplier) / curr.divisor + curr.summand;
				break;
			}
		}
	}

	// Other ride of same type penalty
	sint32 otherRidesOfSameType = 0;
	rct_ride *ride2;
	sint32 i;
	FOR_ALL_RIDES(i, ride2) {
		if (ride2->type == ride->type && ride2->status == RIDE_STATUS_OPEN)
			otherRidesOfSameType++;
	}
	if (otherRidesOfSameType > 1)
		value -= value / 4;

	ride->value = max(0, value);
}

/**
 * I think this function computes ride upkeep? Though it is weird that the
 *  rct2: sub_65E621
 * inputs
 * - edi: ride ptr
 */
static uint16 ride_compute_upkeep(rct_ride *ride)
{
	// data stored at 0x0057E3A8, incrementing 18 bytes at a time
	uint16 upkeep = initialUpkeepCosts[ride->type];

	uint16 trackCost = costPerTrackPiece[ride->type];
	uint8 dl = ride->drops;

	dl = dl >> 6;
	dl = dl & 3;
	upkeep += trackCost * dl;

	uint32 totalLength = ride_get_total_length(ride) >> 16;

	// The data originally here was 20's and 0's. The 20's all represented
	// rides that had tracks. The 0's were fixed rides like crooked house or
	// dodgems.
	// Data source is 0x0097E3AC
	if (hasRunningTrack[ride->type]) {
		totalLength *= 20;
	}
	upkeep += (uint16)(totalLength >> 10);

	if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) {
		// The original code read from a table starting at 0x0097E3AE and
		// incrementing by 0x12 bytes between values. However, all of these
		// values were 40. I have replaced the table lookup with the constant
		// 40 in this case.
		upkeep += 40;
	}

	// Add maintenance cost for reverser track pieces
	uint16 reverserMaintenanceCost = 80;
	if (ride->type == RIDE_TYPE_REVERSER_ROLLER_COASTER) {
		reverserMaintenanceCost = 10;
	}
	upkeep += reverserMaintenanceCost * gRideRatingsCalcData.num_reversers;

	// Add maintenance cost for brake track pieces
	upkeep += 20 * gRideRatingsCalcData.num_brakes;

	// these seem to be adhoc adjustments to a ride's upkeep/cost, times
	// various variables set on the ride itself.

	// https://gist.github.com/kevinburke/e19b803cd2769d96c540
	upkeep += rideUnknownData1[ride->type] * ride->num_vehicles;

	// either set to 3 or 0, extra boosts for some rides including mini golf
	if (rideUnknownData2[ride->type]) {
		upkeep += 3 * ride->num_cars_per_train;
	}

	// slight upkeep boosts for some rides - 5 for mini railroad, 10 for log
	// flume/rapids, 10 for roller coaster, 28 for giga coaster
	upkeep += rideUnknownData3[ride->type] * ride->num_stations;

	if (ride->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE) {
		upkeep += 30;
	} else if (ride->mode == RIDE_MODE_POWERED_LAUNCH_PASSTROUGH) {
		upkeep += 160;
	} else if (ride->mode == RIDE_MODE_LIM_POWERED_LAUNCH) {
		upkeep += 320;
	} else if (ride->mode == RIDE_MODE_POWERED_LAUNCH ||
			ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED) {
		upkeep += 220;
	}

	// multiply by 5/8
	upkeep = upkeep * 10;
	upkeep = upkeep >> 4;
	return upkeep;
}

/**
 *
 *  rct2: 0x0065E7FB
 *
 * inputs
 * - bx: excitement
 * - cx: intensity
 * - bp: nausea
 * - edi: ride ptr
 */
static void ride_ratings_apply_adjustments(rct_ride *ride, rating_tuple *ratings)
{
	rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);

	// Apply ride entry multipliers
	ratings->excitement += ((ratings->excitement * rideEntry->excitement_multipler) >> 7);
	ratings->intensity +=  ((ratings->intensity  * rideEntry->intensity_multipler ) >> 7);
	ratings->nausea +=     ((ratings->nausea     * rideEntry->nausea_multipler    ) >> 7);

	// Apply total air time
#ifdef ORIGINAL_RATINGS
	if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_AIR_TIME) {
		uint16 totalAirTime = ride->total_air_time;
		if (rideEntry->flags & RIDE_ENTRY_FLAG_LIMIT_AIRTIME_BONUS) {
			if (totalAirTime >= 96) {
				totalAirTime -= 96;
				ratings->excitement -= totalAirTime / 8;
				ratings->nausea += totalAirTime / 16;
			}
		} else {
			ratings->excitement += totalAirTime / 8;
			ratings->nausea += totalAirTime / 16;
		}
	}
#else
	if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_AIR_TIME) {
		if (rideEntry->flags & RIDE_ENTRY_FLAG_LIMIT_AIRTIME_BONUS) {
			// Limit airtime bonus for heartline twister coaster (see issues #2031 and #2064)
			ratings->excitement += min(ride->total_air_time, 96) / 8;
		} else {
			ratings->excitement += ride->total_air_time / 8;
		}
		ratings->nausea += ride->total_air_time / 16;
	}
#endif
}

/**
 * Lowers excitement, the higher the intensity.
 *  rct2: 0x0065E7A3
 */
static void ride_ratings_apply_intensity_penalty(rating_tuple *ratings)
{
	static const ride_rating intensityBounds[] = { 1000, 1100, 1200, 1320, 1450 };
	sint32 i;

	ride_rating excitement = ratings->excitement;
	for (i = 0; i < countof(intensityBounds); i++)
		if (ratings->intensity >= intensityBounds[i])
			excitement -= excitement / 4;
	ratings->excitement = excitement;
}

/**
 *
 *  rct2: 0x00655FD6
 */
static void set_unreliability_factor(rct_ride *ride)
{
	// The bigger the difference in lift speed and minimum the higher the unreliability
	uint8 lift_speed_adjustment = RideLiftData[ride->type].minimum_speed;
    ride->unreliability_factor += (ride->lift_hill_speed - lift_speed_adjustment) * 2;
}

static uint32 get_proximity_score_helper_1(uint16 x, uint16 max, uint32 multiplier)
{
	return (min(x, max) * multiplier) >> 16;
}

static uint32 get_proximity_score_helper_2(uint16 x, uint16 additionIfNotZero, uint16 max, uint32 multiplier)
{
	uint32 result = x;
	if (result != 0) result += additionIfNotZero;
	return (min(result, max) * multiplier) >> 16;
}

static uint32 get_proximity_score_helper_3(uint16 x, uint16 resultIfNotZero)
{
	return x == 0 ? 0 : resultIfNotZero;
}

/**
 *
 *  rct2: 0x0065E277
 */
static uint32 ride_ratings_get_proximity_score()
{
	const uint16 * scores = gRideRatingsCalcData.proximity_scores;

	uint32 result = 0;
	result += get_proximity_score_helper_1(scores[PROXIMITY_WATER_OVER                  ]    ,      60, 0x00AAAA);
	result += get_proximity_score_helper_1(scores[PROXIMITY_WATER_TOUCH                 ]    ,      22, 0x0245D1);
	result += get_proximity_score_helper_1(scores[PROXIMITY_WATER_LOW                   ]    ,      10, 0x020000);
	result += get_proximity_score_helper_1(scores[PROXIMITY_WATER_HIGH                  ]    ,      40, 0x00A000);
	result += get_proximity_score_helper_1(scores[PROXIMITY_SURFACE_TOUCH               ]    ,      70, 0x01B6DB);
	result += get_proximity_score_helper_1(scores[PROXIMITY_PATH_OVER                   ] + 8,      12, 0x064000);
	result += get_proximity_score_helper_3(scores[PROXIMITY_PATH_TOUCH_ABOVE            ]    ,  40              );
	result += get_proximity_score_helper_3(scores[PROXIMITY_PATH_TOUCH_UNDER            ]    ,  45              );
	result += get_proximity_score_helper_2(scores[PROXIMITY_138B5A6                     ]    ,  10, 20, 0x03C000);
	result += get_proximity_score_helper_2(scores[PROXIMITY_138B5A8                     ]    ,  10, 20, 0x044000);
	result += get_proximity_score_helper_2(scores[PROXIMITY_OWN_TRACK_TOUCH_ABOVE       ]    ,  10, 15, 0x035555);
	result += get_proximity_score_helper_1(scores[PROXIMITY_OWN_TRACK_CLOSE_ABOVE       ]    ,       5, 0x060000);
	result += get_proximity_score_helper_2(scores[PROXIMITY_FOREIGN_TRACK_ABOVE_OR_BELOW]    ,  10, 15, 0x02AAAA);
	result += get_proximity_score_helper_2(scores[PROXIMITY_FOREIGN_TRACK_TOUCH_ABOVE   ]    ,  10, 15, 0x04AAAA);
	result += get_proximity_score_helper_1(scores[PROXIMITY_FOREIGN_TRACK_CLOSE_ABOVE   ]    ,       5, 0x090000);
	result += get_proximity_score_helper_1(scores[PROXIMITY_SCENERY_SIDE_BELOW          ]    ,      35, 0x016DB6);
	result += get_proximity_score_helper_1(scores[PROXIMITY_SCENERY_SIDE_ABOVE          ]    ,      35, 0x00DB6D);
	result += get_proximity_score_helper_3(scores[PROXIMITY_OWN_STATION_TOUCH_ABOVE     ]    ,  55              );
	result += get_proximity_score_helper_3(scores[PROXIMITY_OWN_STATION_CLOSE_ABOVE     ]    ,  25              );
	result += get_proximity_score_helper_2(scores[PROXIMITY_TRACK_THROUGH_VERTICAL_LOOP ]    ,   4,  6, 0x140000);
	result += get_proximity_score_helper_2(scores[PROXIMITY_PATH_TROUGH_VERTICAL_LOOP   ]    ,   4,  6, 0x0F0000);
	result += get_proximity_score_helper_3(scores[PROXIMITY_INTERSECTING_VERTICAL_LOOP  ]    , 100              );
	result += get_proximity_score_helper_2(scores[PROXIMITY_THROUGH_VERTICAL_LOOP       ]    ,   4,  6, 0x0A0000);
	result += get_proximity_score_helper_2(scores[PROXIMITY_PATH_SIDE_CLOSE             ]    ,  10, 20, 0x01C000);
	result += get_proximity_score_helper_2(scores[PROXIMITY_FOREIGN_TRACK_SIDE_CLOSE    ]    ,  10, 20, 0x024000);
	result += get_proximity_score_helper_2(scores[PROXIMITY_SURFACE_SIDE_CLOSE          ]    ,  10, 20, 0x028000);
	return result;
}

/**
 * Seems to calculate how much of the track is sheltered in eighths.
 *  rct2: 0x0065E72D
 */
static sint32 sub_65E72D(rct_ride *ride)
{
	sint32 totalLength = ride_get_total_length(ride);
	sint32 shelteredLength = ride->sheltered_length;
	sint32 lengthEighth = totalLength / 8;
	sint32 lengthCounter = lengthEighth;
	sint32 numShelteredEighths = 0;
	for (sint32 i = 0; i < 7; i++) {
		if (shelteredLength >= lengthCounter) {
			lengthCounter += lengthEighth;
			numShelteredEighths++;
		}
	}

	sint32 dh = numShelteredEighths;
	rct_ride_entry *rideType = get_ride_entry(ride->subtype);
	if (rideType->flags & RIDE_ENTRY_FLAG_COVERED_RIDE)
		numShelteredEighths = 7;

	return (dh << 8) | numShelteredEighths;
}

static rating_tuple get_flat_turns_rating(rct_ride* ride)
{
	sint32 no_3_plus_turns = get_turn_count_3_elements(ride, 0);
	sint32 no_2_turns = get_turn_count_2_elements(ride, 0);
	sint32 no_1_turns = get_turn_count_1_element(ride, 0);

	rating_tuple rating;
	rating.excitement = (no_3_plus_turns * 0x28000) >> 16;
	rating.excitement += (no_2_turns * 0x30000) >> 16;
	rating.excitement += (no_1_turns * 63421) >> 16;

	rating.intensity = (no_3_plus_turns * 81920) >> 16;
	rating.intensity += (no_2_turns * 49152) >> 16;
	rating.intensity += (no_1_turns * 21140) >> 16;

	rating.nausea = (no_3_plus_turns * 0x50000) >> 16;
	rating.nausea += (no_2_turns * 0x32000) >> 16;
	rating.nausea += (no_1_turns * 42281) >> 16;

	return rating;
}

/**
 *
 *  rct2: 0x0065DF72
 */
static rating_tuple get_banked_turns_rating(rct_ride* ride)
{
	sint32 no_3_plus_turns = get_turn_count_3_elements(ride, 1);
	sint32 no_2_turns = get_turn_count_2_elements(ride, 1);
	sint32 no_1_turns = get_turn_count_1_element(ride, 1);

	rating_tuple rating;
	rating.excitement = (no_3_plus_turns * 0x3C000) >> 16;
	rating.excitement += (no_2_turns * 0x3C000) >> 16;
	rating.excitement += (no_1_turns * 73992) >> 16;

	rating.intensity = (no_3_plus_turns * 0x14000) >> 16;
	rating.intensity += (no_2_turns * 49152) >> 16;
	rating.intensity += (no_1_turns * 21140) >> 16;

	rating.nausea = (no_3_plus_turns * 0x50000) >> 16;
	rating.nausea += (no_2_turns * 0x32000) >> 16;
	rating.nausea += (no_1_turns * 48623) >> 16;

	return rating;
}

/**
 *
 *  rct2: 0x0065E047
 */
static rating_tuple get_sloped_turns_rating(rct_ride* ride) {
	rating_tuple rating;

	sint32 no_4_plus_turns = get_turn_count_4_plus_elements(ride, 2);
	sint32 no_3_turns = get_turn_count_3_elements(ride, 2);
	sint32 no_2_turns = get_turn_count_2_elements(ride, 2);
	sint32 no_1_turns = get_turn_count_1_element(ride, 2);

	rating.excitement = (min(no_4_plus_turns, 4) * 0x78000) >> 16;
	rating.excitement += (min(no_3_turns, 6) * 273066) >> 16;
	rating.excitement += (min(no_2_turns, 6) * 0x3AAAA) >> 16;
	rating.excitement += (min(no_1_turns, 7) * 187245) >> 16;
	rating.intensity = 0;
	rating.nausea = (min(no_4_plus_turns, 8) * 0x78000) >> 16;

	return rating;
}

/**
 *
 *  rct2: 0x0065E0F2
 */
static rating_tuple get_inversions_ratings(uint8 inversions) {
	rating_tuple rating;

	rating.excitement = (min(inversions, 6) * 0x1AAAAA) >> 16;
	rating.intensity = (inversions * 0x320000) >> 16;
	rating.nausea = (inversions * 0x15AAAA) >> 16;

	return rating;
}

static rating_tuple get_special_track_elements_rating(uint8 type, rct_ride *ride) {
	sint32 excitement = 0, intensity = 0, nausea = 0;
	if (type == RIDE_TYPE_GHOST_TRAIN) {
		if (ride_has_spinning_tunnel(ride)) {
			excitement += 40;
			intensity  += 25;
			nausea     += 55;
		}
	} else if (type == RIDE_TYPE_LOG_FLUME) {
		// Reverser for log flume
		if (ride_has_log_reverser(ride)) {
			excitement += 48;
			intensity  += 55;
			nausea     += 65;
		}
	} else {
		if (ride_has_water_splash(ride)) {
			excitement += 50;
			intensity  += 30;
			nausea     += 20;
		}
		if (ride_has_waterfall(ride)) {
			excitement += 55;
			intensity  += 30;
		}
		if (ride_has_whirlpool(ride)) {
			excitement += 35;
			intensity  += 20;
			nausea     += 23;
		}
	}
	uint8 helix_sections = ride_get_helix_sections(ride);
	sint32 al = min(helix_sections, 9);
	excitement += (al * 254862) >> 16;

	al = min(helix_sections, 11);
	intensity += (al * 148945) >> 16;

	al = max(helix_sections - 5, 0);
	al = min(al, 10);
	nausea += (al * 0x140000) >> 16;

	rating_tuple rating = { excitement, intensity, nausea };
	return rating;
}

/**
 *
 *  rct2: 0x0065DDD1
 */
static rating_tuple ride_ratings_get_turns_ratings(rct_ride *ride)
{
	sint32 excitement = 0, intensity = 0, nausea = 0;

	rating_tuple special_track_element_rating = get_special_track_elements_rating(ride->type, ride);
	excitement += special_track_element_rating.excitement;
	intensity  += special_track_element_rating.intensity;
	nausea     += special_track_element_rating.nausea;

	rating_tuple var_10E_rating = get_flat_turns_rating(ride);
	excitement += var_10E_rating.excitement;
	intensity  += var_10E_rating.intensity;
	nausea     += var_10E_rating.nausea;

	rating_tuple var_110_rating = get_banked_turns_rating(ride);
	excitement += var_110_rating.excitement;
	intensity  += var_110_rating.intensity;
	nausea     += var_110_rating.nausea;

	rating_tuple var_112_rating = get_sloped_turns_rating(ride);
	excitement += var_112_rating.excitement;
	intensity  += var_112_rating.intensity;
	nausea     += var_112_rating.nausea;

	rating_tuple inversions_rating = get_inversions_ratings(ride->inversions & 0x1F);
	excitement += inversions_rating.excitement;
	intensity  += inversions_rating.intensity;
	nausea     += inversions_rating.nausea;

	rating_tuple rating = { excitement, intensity, nausea };
	return rating;
}

/**
 *
 *  rct2: 0x0065E1C2
 */
static rating_tuple ride_ratings_get_sheltered_ratings(rct_ride *ride)
{
	sint32 sheltered_length_shifted = (ride->sheltered_length) >> 16;
	uint32 eax = min(sheltered_length_shifted, 1000);
	sint32 excitement = (eax * 9175) >> 16;

	eax = min(sheltered_length_shifted, 2000);
	sint32 intensity = (eax * 0x2666) >> 16;

	eax = min(sheltered_length_shifted, 1000);
	sint32 nausea = (eax * 0x4000) >> 16;

	/*eax = (ride->var_11C * 30340) >> 16;*/
	/*nausea += eax;*/

	if (ride->num_sheltered_sections & 0x40) {
		excitement += 20;
		nausea     += 15;
	}

	if (ride->num_sheltered_sections & 0x20) {
		excitement += 20;
		nausea     += 15;
	}

	uint8 lowerval = ride->num_sheltered_sections & 0x1F;
	lowerval = min(lowerval, 11);
	excitement += (lowerval * 774516) >> 16;

	rating_tuple rating = { excitement, intensity, nausea };
	return rating;
}

/**
 *
 *  rct2: 0x0065DCDC
 */
static rating_tuple ride_ratings_get_gforce_ratings(rct_ride *ride)
{
	rating_tuple result = {
		.excitement = 0,
		.intensity = 0,
		.nausea = 0
	};

	// Apply maximum positive G force factor
	result.excitement += (ride->max_positive_vertical_g * 5242) >> 16;
	result.intensity += (ride->max_positive_vertical_g * 52428) >> 16;
	result.nausea += (ride->max_positive_vertical_g * 17039) >> 16;

	// Apply maximum negative G force factor
	fixed16_2dp gforce = ride->max_negative_vertical_g;
	result.excitement += (clamp(-FIXED_2DP(2,50), gforce, FIXED_2DP(0,00)) * -15728) >> 16;
	result.intensity += ((gforce - FIXED_2DP(1,00)) * -52428) >> 16;
	result.nausea += ((gforce - FIXED_2DP(1,00)) * -14563) >> 16;

	// Apply lateral G force factor
	result.excitement += (min(FIXED_2DP(1,50), ride->max_lateral_g) * 26214) >> 16;
	result.intensity += ride->max_lateral_g;
	result.nausea += (ride->max_lateral_g * 21845) >> 16;

	// Very high lateral G force penalty
	#ifdef ORIGINAL_RATINGS
	if (ride->max_lateral_g > FIXED_2DP(2,80)) {
		result.intensity += FIXED_2DP(3,75);
		result.nausea += FIXED_2DP(2,00);
	}
	if (ride->max_lateral_g > FIXED_2DP(3,10)) {
		result.excitement /= 2;
		result.intensity += FIXED_2DP(8,50);
		result.nausea += FIXED_2DP(4,00);
	}
	#endif

	return result;
}

/**
 *
 *  rct2: 0x0065E139
 */
static rating_tuple ride_ratings_get_drop_ratings(rct_ride *ride)
{
	rating_tuple result = {
		.excitement = 0,
		.intensity = 0,
		.nausea = 0
	};

	// Apply number of drops factor
	sint32 drops = ride->drops & 0x3F;
	result.excitement += (min(9, drops) * 728177) >> 16;
	result.intensity += (drops * 928426) >> 16;
	result.nausea += (drops * 655360) >> 16;

	// Apply highest drop factor
	result.excitement += ((ride->highest_drop_height * 2) * 16000) >> 16;
	result.intensity += ((ride->highest_drop_height * 2) * 32000) >> 16;
	result.nausea += ((ride->highest_drop_height * 2) * 10240) >> 16;

	return result;
}

/**
 * Calculates a score based on the surrounding scenery.
 *  rct2: 0x0065E557
 */
static sint32 ride_ratings_get_scenery_score(rct_ride *ride)
{
	sint32 i;
	uint16 stationXY;
	for (i = 0; i < 4; i++) {
		stationXY = ride->station_starts[i];
		if (stationXY != 0xFFFF)
			break;
	}
	if (i == 4)
		return 0;

	if (ride->type == RIDE_TYPE_MAZE)
		stationXY = ride->entrances[0];

	sint32 x = stationXY & 0xFF;
	sint32 y = stationXY >> 8;
	sint32 z = map_element_height(x * 32, y * 32) & 0xFFFF;

	// Check if station is underground, returns a fixed mediocre score since you can't have scenery underground
	if (z > ride->station_heights[i] * 8)
		return 40;

	// Count surrounding scenery items
	sint32 numSceneryItems = 0;
	for (sint32 yy = max(y - 5, 0); yy <= min(y + 5, 255); yy++) {
		for (sint32 xx = max(x - 5, 0); xx <= min(x + 5, 255); xx++) {
			// Count scenery items on this tile
			rct_map_element *mapElement = map_get_first_element_at(xx, yy);
			do {
				if (mapElement->flags & (1 << 4))
					continue;

				sint32 type = map_element_get_type(mapElement);
				if (type == MAP_ELEMENT_TYPE_SCENERY || type == MAP_ELEMENT_TYPE_SCENERY_MULTIPLE)
					numSceneryItems++;
			} while (!map_element_is_last_for_tile(mapElement++));
		}
	}

	return min(numSceneryItems, 47) * 5;
}

#pragma region Ride rating calculation helpers

static void ride_ratings_set(rating_tuple *ratings, sint32 excitement, sint32 intensity, sint32 nausea)
{
	ratings->excitement = excitement;
	ratings->intensity = intensity;
	ratings->nausea = nausea;
}

static void ride_ratings_apply_length(rating_tuple *ratings, rct_ride *ride, sint32 maxLength, sint32 excitementMultiplier)
{
	ratings->excitement += (min(ride_get_total_length(ride) >> 16, maxLength) * excitementMultiplier) >> 16;
}

static void ride_ratings_apply_synchronisation(rating_tuple *ratings, rct_ride *ride, sint32 excitement, sint32 intensity)
{
	if ((ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS) &&
		ride_has_adjacent_station(ride)) {
		ratings->excitement += excitement;
		ratings->intensity += intensity;
	}
}

static void ride_ratings_apply_train_length(rating_tuple *ratings, rct_ride *ride, sint32 excitementMultiplier)
{
	ratings->excitement += ((ride->num_cars_per_train - 1) * excitementMultiplier) >> 16;
}

static void ride_ratings_apply_max_speed(rating_tuple *ratings, rct_ride *ride, sint32 excitementMultiplier, sint32 intensityMultiplier, sint32 nauseaMultiplier)
{
	ratings->excitement += ((ride->max_speed >> 16) * excitementMultiplier) >> 16;
	ratings->intensity += ((ride->max_speed >> 16) * intensityMultiplier) >> 16;
	ratings->nausea += ((ride->max_speed >> 16) * nauseaMultiplier) >> 16;
}

static void ride_ratings_apply_average_speed(rating_tuple *ratings, rct_ride *ride, sint32 excitementMultiplier, sint32 intensityMultiplier)
{
	ratings->excitement += ((ride->average_speed >> 16) * excitementMultiplier) >> 16;
	ratings->intensity += ((ride->average_speed >> 16) * intensityMultiplier) >> 16;
}

static void ride_ratings_apply_duration(rating_tuple *ratings, rct_ride *ride, sint32 maxDuration, sint32 excitementMultiplier)
{
	ratings->excitement += (min(ride_get_total_time(ride), maxDuration) * excitementMultiplier) >> 16;
}

static void ride_ratings_apply_gforces(rating_tuple *ratings, rct_ride *ride, sint32 excitementMultiplier, sint32 intensityMultiplier, sint32 nauseaMultiplier)
{
	rating_tuple subRating = ride_ratings_get_gforce_ratings(ride);
	ratings->excitement += (subRating.excitement * excitementMultiplier) >> 16;
	ratings->intensity += (subRating.intensity * intensityMultiplier) >> 16;
	ratings->nausea += (subRating.nausea * nauseaMultiplier) >> 16;
}

static void ride_ratings_apply_turns(rating_tuple *ratings, rct_ride *ride, sint32 excitementMultiplier, sint32 intensityMultiplier, sint32 nauseaMultiplier)
{
	rating_tuple subRating = ride_ratings_get_turns_ratings(ride);
	ratings->excitement += (subRating.excitement * excitementMultiplier) >> 16;
	ratings->intensity += (subRating.intensity * intensityMultiplier) >> 16;
	ratings->nausea += (subRating.nausea * nauseaMultiplier) >> 16;
}

static void ride_ratings_apply_drops(rating_tuple *ratings, rct_ride *ride, sint32 excitementMultiplier, sint32 intensityMultiplier, sint32 nauseaMultiplier)
{
	rating_tuple subRating = ride_ratings_get_drop_ratings(ride);
	ratings->excitement += (subRating.excitement * excitementMultiplier) >> 16;
	ratings->intensity += (subRating.intensity * intensityMultiplier) >> 16;
	ratings->nausea += (subRating.nausea * nauseaMultiplier) >> 16;
}

static void ride_ratings_apply_sheltered_ratings(rating_tuple *ratings, rct_ride *ride, sint32 excitementMultiplier, sint32 intensityMultiplier, sint32 nauseaMultiplier)
{
	rating_tuple subRating = ride_ratings_get_sheltered_ratings(ride);
	ratings->excitement += (subRating.excitement * excitementMultiplier) >> 16;
	ratings->intensity += (subRating.intensity * intensityMultiplier) >> 16;
	ratings->nausea += (subRating.nausea * nauseaMultiplier) >> 16;
}

static void ride_ratings_apply_operation_option(rating_tuple *ratings, rct_ride *ride, sint32 excitementMultiplier, sint32 intensityMultiplier, sint32 nauseaMultiplier)
{
	ratings->excitement += (ride->operation_option * excitementMultiplier) >> 16;
	ratings->intensity += (ride->operation_option * intensityMultiplier) >> 16;
	ratings->nausea += (ride->operation_option * nauseaMultiplier) >> 16;
}

static void ride_ratings_apply_proximity(rating_tuple *ratings, rct_ride *ride, sint32 excitementMultiplier)
{
	ratings->excitement += (ride_ratings_get_proximity_score() * excitementMultiplier) >> 16;
}

static void ride_ratings_apply_scenery(rating_tuple *ratings, rct_ride *ride, sint32 excitementMultiplier)
{
	ratings->excitement += (ride_ratings_get_scenery_score(ride) * excitementMultiplier) >> 16;
}

static void ride_ratings_apply_highest_drop_height_penalty(rating_tuple *ratings, rct_ride *ride, sint32 minHighestDropHeight, sint32 excitementPenalty, sint32 intensityPenalty, sint32 nauseaPenalty)
{
	if (ride->highest_drop_height < minHighestDropHeight) {
		ratings->excitement /= excitementPenalty;
		ratings->intensity /= intensityPenalty;
		ratings->nausea /= nauseaPenalty;
	}
}

static void ride_ratings_apply_max_speed_penalty(rating_tuple *ratings, rct_ride *ride, sint32 minMaxSpeed, sint32 excitementPenalty, sint32 intensityPenalty, sint32 nauseaPenalty)
{
	if (ride->max_speed < minMaxSpeed) {
		ratings->excitement /= excitementPenalty;
		ratings->intensity /= intensityPenalty;
		ratings->nausea /= nauseaPenalty;
	}
}

static void ride_ratings_apply_num_drops_penalty(rating_tuple *ratings, rct_ride *ride, sint32 minNumDrops, sint32 excitementPenalty, sint32 intensityPenalty, sint32 nauseaPenalty)
{
	if ((ride->drops & 0x3F) < minNumDrops) {
		ratings->excitement /= excitementPenalty;
		ratings->intensity /= intensityPenalty;
		ratings->nausea /= nauseaPenalty;
	}
}

static void ride_ratings_apply_max_negative_g_penalty(rating_tuple *ratings, rct_ride *ride, sint32 maxMaxNegativeVerticalG, sint32 excitementPenalty, sint32 intensityPenalty, sint32 nauseaPenalty)
{
	if (ride->max_negative_vertical_g >= maxMaxNegativeVerticalG) {
		ratings->excitement /= excitementPenalty;
		ratings->intensity /= intensityPenalty;
		ratings->nausea /= nauseaPenalty;
	}
}

static void ride_ratings_apply_max_lateral_g_penalty(rating_tuple *ratings, rct_ride *ride, sint32 minMaxLateralG, sint32 excitementPenalty, sint32 intensityPenalty, sint32 nauseaPenalty)
{
	if (ride->max_lateral_g < minMaxLateralG) {
		ratings->excitement /= excitementPenalty;
		ratings->intensity /= intensityPenalty;
		ratings->nausea /= nauseaPenalty;
	}
}

static void ride_ratings_apply_excessive_lateral_g_penalty(rating_tuple *ratings, rct_ride *ride)
{
	#ifndef ORIGINAL_RATINGS
	if (ride->max_lateral_g > FIXED_2DP(2,80)) {
		ratings->intensity += FIXED_2DP(3,75);
		ratings->nausea += FIXED_2DP(2,00);
	}
	if (ride->max_lateral_g > FIXED_2DP(3,10)) {
		ratings->excitement /= 2;
		ratings->intensity += FIXED_2DP(8,50);
		ratings->nausea += FIXED_2DP(4,00);
	}
	#endif
}

static void ride_ratings_apply_first_length_penalty(rating_tuple *ratings, rct_ride *ride, sint32 minFirstLength, sint32 excitementPenalty, sint32 intensityPenalty, sint32 nauseaPenalty)
{
	if (ride->length[0] < minFirstLength) {
		ratings->excitement /= excitementPenalty;
		ratings->intensity /= intensityPenalty;
		ratings->nausea /= nauseaPenalty;
	}
}

#pragma endregion

#pragma region Ride rating calculation functions

static void ride_ratings_calculate_spiral_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(3,30), RIDE_RATING(0,30), RIDE_RATING(0,30));
	ride_ratings_apply_length(&ratings, ride, 6000, 819);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 140434);
	ride_ratings_apply_max_speed(&ratings, ride, 51366, 85019, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 400497);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 36864, 30384, 49648);
	ride_ratings_apply_turns(&ratings, ride, 28235, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 43690, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
	}

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_stand_up_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 17;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,50), RIDE_RATING(3,00), RIDE_RATING(3,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,10));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 123987, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 59578);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 34952, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 12850, 28398, 30427);
	ride_ratings_apply_proximity(&ratings, ride, 17893);
	ride_ratings_apply_scenery(&ratings, ride, 5577);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,50), 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_suspended_swinging_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 18;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(3,30), RIDE_RATING(2,90), RIDE_RATING(3,50));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,10));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 32768, 23831, 79437);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 48036);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6971);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xC0000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 60), 2, 2, 2);
	ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1, 50), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_inverted_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 17;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(3,60), RIDE_RATING(2,80), RIDE_RATING(3,20));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,42), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 29789, 55606);
	ride_ratings_apply_turns(&ratings, ride, 26749, 29552, 57186);
	ride_ratings_apply_drops(&ratings, ride, 29127, 39009, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 15291, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 15657);
	ride_ratings_apply_scenery(&ratings, ride, 8366);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,30), 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_junior_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 13;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,40), RIDE_RATING(2,50), RIDE_RATING(1,80));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 20480, 23831, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 25700, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 9760);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 1, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_miniature_railway(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 11;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,50), RIDE_RATING(0,00), RIDE_RATING(0,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_train_length(&ratings, ride, 140434);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, -6425, 6553, 23405);
	ride_ratings_apply_proximity(&ratings, ride, 8946);
	ride_ratings_apply_scenery(&ratings, ride, 20915);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	sint32 edx = sub_65E72D(ride);
	if (((edx >> 8) & 0xFF) >= 4)
		ride->excitement /= 4;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_monorail(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,00), RIDE_RATING(0,00), RIDE_RATING(0,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_train_length(&ratings, ride, 93622);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 70849, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 218453);
	ride_ratings_apply_duration(&ratings, ride, 150, 21845);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 5140, 6553, 18724);
	ride_ratings_apply_proximity(&ratings, ride, 8946);
	ride_ratings_apply_scenery(&ratings, ride, 16732);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	sint32 edx = sub_65E72D(ride);
	if (((edx >> 8) & 0xFF) >= 4)
		ride->excitement /= 4;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_mini_suspended_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 15;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,80), RIDE_RATING(2,50), RIDE_RATING(2,70));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,45), RIDE_RATING(0,15));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 49648);
	ride_ratings_apply_turns(&ratings, ride, 34179, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 58254, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 19275, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 13943);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x80000, 2, 2, 2);
	ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1,30), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_boat_ride(rct_ride *ride)
{
	ride->unreliability_factor = 7;
	set_unreliability_factor(ride);

	// NOTE In the original game, the ratings were zeroed before calling set_unreliability_factor which is unusual as rest of
	// the calculation functions do this before hand. This is because set_unreliability_factor alters the value of
	// ebx (excitement). This is assumed to be a bug and therefore fixed.

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,90), RIDE_RATING(0,80), RIDE_RATING(0,90));

	// Most likely checking if the ride has does not have a circuit
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		ratings.excitement += RIDE_RATING(0,20);

	ride_ratings_apply_proximity(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 22310);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_wooden_wild_mouse(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,90), RIDE_RATING(2,90), RIDE_RATING(2,10));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0, 8));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 102400, 35746, 49648);
	ride_ratings_apply_turns(&ratings, ride, 29721, 43458, 45749);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 17893);
	ride_ratings_apply_scenery(&ratings, ride, 5577);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,10), 2, 2, 2);
	ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1,50), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 3, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_steeplechase(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,70), RIDE_RATING(2,40), RIDE_RATING(1,80));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,75), RIDE_RATING(0, 9));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 20480, 20852, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 25700, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 9760);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 4, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x80000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,50), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xF00000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_car_ride(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 12;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,00), RIDE_RATING(0,50), RIDE_RATING(0,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,15), RIDE_RATING(0,00));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_turns(&ratings, ride, 14860, 0, 11437);
	ride_ratings_apply_drops(&ratings, ride, 8738, 0, 0);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 12850, 6553, 4681);
	ride_ratings_apply_proximity(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 8366);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 8, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_launched_freefall(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,70), RIDE_RATING(3,00), RIDE_RATING(3,50));

	if (ride->mode == RIDE_MODE_DOWNWARD_LAUNCH) {
		ratings.excitement	+= RIDE_RATING(0,30);
		ratings.intensity	+= RIDE_RATING(0,65);
		ratings.nausea		+= RIDE_RATING(0,45);
	}

	ratings.excitement += ((ride_get_total_length(ride) >> 16) * 32768) >> 16;

#ifdef ORIGINAL_RATINGS
	ride_ratings_apply_operation_option(&ratings, ride, 0, 1355917, 451972);
#else
	// Only apply "launch speed" effects when the setting can be modified
	if (ride->mode == RIDE_MODE_UPWARD_LAUNCH) {
		ride_ratings_apply_operation_option(&ratings, ride, 0, 1355917, 451972);
	} else {
		// Fix #3282: When the ride mode is in downward launch mode, the intensity and
		//            nausea were fixed regardless of how high the ride is. The following
		//            calculation is based on roto-drop which is a similar mechanic.
		sint32 lengthFactor = ((ride_get_total_length(ride) >> 16) * 209715) >> 16;
		ratings.excitement += lengthFactor;
		ratings.intensity += lengthFactor * 2;
		ratings.nausea += lengthFactor * 2;
	}
#endif

	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 25098);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_bobsleigh_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,80), RIDE_RATING(3,20), RIDE_RATING(2,50));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,20), RIDE_RATING(0,00));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 65536, 23831, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 5577);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xC0000, 2, 2, 2);
	ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1,20), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_observation_tower(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 15;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,50), RIDE_RATING(0,00), RIDE_RATING(0,10));
	ratings.excitement += ((ride_get_total_length(ride) >> 16) * 45875) >> 16;
	ratings.nausea += ((ride_get_total_length(ride) >> 16) * 26214) >> 16;
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 83662);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;

	sint32 edx = sub_65E72D(ride);
	if (((edx >> 8) & 0xFF) >= 5)
		ride->excitement /= 4;
}

static void ride_ratings_calculate_looping_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = ride_is_powered_launched(ride) ? 20 : 15;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(3,00), RIDE_RATING(0,50), RIDE_RATING(0,20));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 14, 2, 2, 2);

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 10), 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
	}

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_dinghy_slide(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 13;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,70), RIDE_RATING(2,00), RIDE_RATING(1,50));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,50), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 65536, 29789, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 5577);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x8C0000, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_mine_train_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,90), RIDE_RATING(2,30), RIDE_RATING(2,10));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 40960, 35746, 49648);
	ride_ratings_apply_turns(&ratings, ride, 29721, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 19275, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 21472);
	ride_ratings_apply_scenery(&ratings, ride, 16732);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,10), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_chairlift(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14 + (ride->speed * 2);
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,60), RIDE_RATING(0,40), RIDE_RATING(0,50));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_turns(&ratings, ride, 7430, 3476, 4574);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, -19275, 21845, 23405);
	ride_ratings_apply_proximity(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 25098);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x960000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	if (ride->num_stations <= 1) {
		ratings.excitement = 0;
		ratings.intensity /= 2;
	}

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	sint32 edx = sub_65E72D(ride);
	if (((edx >> 8) & 0xFF) >= 4)
		ride->excitement /= 4;

	ride->inversions &= 0x1F;
	ride->inversions |= edx << 5;
}

static void ride_ratings_calculate_corkscrew_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(3,00), RIDE_RATING(0,50), RIDE_RATING(0,20));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
	}

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_maze(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 8;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,30), RIDE_RATING(0,50), RIDE_RATING(0,00));

	sint32 size = min(ride->maze_tiles, 100);
	ratings.excitement += size;
	ratings.intensity += size * 2;

	ride_ratings_apply_scenery(&ratings, ride, 22310);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_spiral_slide(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 8;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,50), RIDE_RATING(1,40), RIDE_RATING(0,90));

	// Unlimited slides boost
	if (ride->mode == RIDE_MODE_UNLIMITED_RIDES_PER_ADMISSION) {
		ratings.excitement += RIDE_RATING(0, 40);
		ratings.intensity  += RIDE_RATING(0, 20);
		ratings.nausea     += RIDE_RATING(0, 25);
	}

	ride_ratings_apply_scenery(&ratings, ride, 25098);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 2 << 5;
}

static void ride_ratings_calculate_go_karts(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,42), RIDE_RATING(1,73), RIDE_RATING(0,40));
	ride_ratings_apply_length(&ratings, ride, 700, 32768);

	if (ride->mode == RIDE_MODE_RACE && ride->num_vehicles >= 4) {
		ratings.excitement +=	RIDE_RATING(1,40);
		ratings.intensity +=	RIDE_RATING(0,50);

		sint32 lapsFactor = (ride->num_laps - 1) * 30;
		ratings.excitement += lapsFactor;
		ratings.intensity += lapsFactor / 2;
	}

	ride_ratings_apply_turns(&ratings, ride, 4458, 3476, 5718);
	ride_ratings_apply_drops(&ratings, ride, 8738, 5461, 6553);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 2570, 8738, 2340);
	ride_ratings_apply_proximity(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 16732);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	sint32 edx = sub_65E72D(ride);

	ride->inversions &= 0x1F;
	ride->inversions |= edx << 5;

	if (((edx >> 8) & 0xFF) >= 6)
		ride->excitement /= 2;
}

static void ride_ratings_calculate_log_flume(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 15;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,50), RIDE_RATING(0,55), RIDE_RATING(0,30));
	ride_ratings_apply_length(&ratings, ride, 2000, 7208);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_max_speed(&ratings, ride, 531372, 655360, 301111);
	ride_ratings_apply_duration(&ratings, ride, 300, 13107);
	ride_ratings_apply_turns(&ratings, ride, 22291, 20860, 4574);
	ride_ratings_apply_drops(&ratings, ride, 69905, 62415, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 22367);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_river_rapids(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,20), RIDE_RATING(0,70), RIDE_RATING(0,50));
	ride_ratings_apply_length(&ratings, ride, 2000, 6225);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,30), RIDE_RATING(0,05));
	ride_ratings_apply_max_speed(&ratings, ride, 115130, 159411, 106274);
	ride_ratings_apply_duration(&ratings, ride, 500, 13107);
	ride_ratings_apply_turns(&ratings, ride, 29721, 22598, 5718);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 31314);
	ride_ratings_apply_scenery(&ratings, ride, 13943);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 2, 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_dodgems(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,30), RIDE_RATING(0,50), RIDE_RATING(0,35));

	if (ride->num_vehicles >= 4)
		ratings.excitement += RIDE_RATING(0,40);

	ratings.excitement += ride->operation_option;
	ratings.intensity += ride->operation_option / 2;

	if (ride->num_vehicles >= 4)
		ratings.excitement += RIDE_RATING(0,40);

	ride_ratings_apply_scenery(&ratings, ride, 5577);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;
}

static void ride_ratings_calculate_pirate_ship(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 10;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,50), RIDE_RATING(1,90), RIDE_RATING(1,41));

	ratings.excitement += ride->operation_option * 5;
	ratings.intensity += ride->operation_option * 5;
	ratings.nausea += ride->operation_option * 10;

	ride_ratings_apply_scenery(&ratings, ride, 16732);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_inverter_ship(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,50), RIDE_RATING(2,70), RIDE_RATING(2,74));

	ratings.excitement += ride->operation_option * 11;
	ratings.intensity += ride->operation_option * 22;
	ratings.nausea += ride->operation_option * 22;

	ride_ratings_apply_scenery(&ratings, ride, 11155);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_food_stall(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_drink_stall(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_shop(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_merry_go_round(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(0,60), RIDE_RATING(0,15), RIDE_RATING(0,30));

	sint32 rotationsFactor = ride->rotations * 5;
	ratings.excitement += rotationsFactor;
	ratings.intensity += rotationsFactor;
	ratings.nausea += rotationsFactor;

	ride_ratings_apply_scenery(&ratings, ride, 19521);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;
}

static void ride_ratings_calculate_information_kiosk(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_toilets(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_ferris_wheel(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(0,60), RIDE_RATING(0,25), RIDE_RATING(0,30));

	sint32 rotationsFactor = ride->rotations * 25;
	ratings.excitement += rotationsFactor;
	ratings.intensity += rotationsFactor;
	ratings.nausea += rotationsFactor;

	ride_ratings_apply_scenery(&ratings, ride, 41831);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_motion_simulator(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 21;
	set_unreliability_factor(ride);

	// Base ratings
	rating_tuple ratings;
	if (ride->mode == RIDE_MODE_FILM_THRILL_RIDERS) {
		ratings.excitement	= RIDE_RATING(3,25);
		ratings.intensity	= RIDE_RATING(4,10);
		ratings.nausea		= RIDE_RATING(3,30);
	} else {
		ratings.excitement	= RIDE_RATING(2,90);
		ratings.intensity	= RIDE_RATING(3,50);
		ratings.nausea		= RIDE_RATING(3,00);
	}

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;
}

static void ride_ratings_calculate_3d_cinema(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 21;
	set_unreliability_factor(ride);

	// Base ratings
	rating_tuple ratings;
	switch (ride->mode) {
	default:
	case RIDE_MODE_3D_FILM_MOUSE_TAILS:
		ratings.excitement	= RIDE_RATING(3,50);
		ratings.intensity	= RIDE_RATING(2,40);
		ratings.nausea		= RIDE_RATING(1,40);
		break;
	case RIDE_MODE_3D_FILM_STORM_CHASERS:
		ratings.excitement	= RIDE_RATING(4,00);
		ratings.intensity	= RIDE_RATING(2,65);
		ratings.nausea		= RIDE_RATING(1,55);
		break;
	case RIDE_MODE_3D_FILM_SPACE_RAIDERS:
		ratings.excitement	= RIDE_RATING(4,20);
		ratings.intensity	= RIDE_RATING(2,60);
		ratings.nausea		= RIDE_RATING(1,48);
		break;
	}

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;
}

static void ride_ratings_calculate_top_spin(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 19;
	set_unreliability_factor(ride);

	// Base ratings
	rating_tuple ratings;
	switch (ride->mode) {
	default:
	case RIDE_MODE_BEGINNERS:
		ratings.excitement	= RIDE_RATING(2,00);
		ratings.intensity	= RIDE_RATING(4,80);
		ratings.nausea		= RIDE_RATING(5,74);
		break;
	case RIDE_MODE_INTENSE:
		ratings.excitement	= RIDE_RATING(3,00);
		ratings.intensity	= RIDE_RATING(5,75);
		ratings.nausea		= RIDE_RATING(6,64);
		break;
	case RIDE_MODE_BERSERK:
		ratings.excitement	= RIDE_RATING(3,20);
		ratings.intensity	= RIDE_RATING(6,80);
		ratings.nausea		= RIDE_RATING(7,94);
		break;
	}

	ride_ratings_apply_scenery(&ratings, ride, 11155);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_space_rings(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 7;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,50), RIDE_RATING(2,10), RIDE_RATING(6,50));
	ride_ratings_apply_scenery(&ratings, ride, 25098);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_reverse_freefall_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 25;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,00), RIDE_RATING(3,20), RIDE_RATING(2,80));
	ride_ratings_apply_length(&ratings, ride, 6000, 327);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,60), RIDE_RATING(0,15));
	ride_ratings_apply_max_speed(&ratings, ride, 436906, 436906, 320398);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 41704, 59578);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 12850, 28398, 11702);
	ride_ratings_apply_proximity(&ratings, ride, 17893);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 34, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_lift(rct_ride *ride)
{
	sint32 totalLength;

	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 15;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,11), RIDE_RATING(0,35), RIDE_RATING(0,30));

	totalLength = ride_get_total_length(ride) >> 16;
	ratings.excitement += (totalLength * 45875) >> 16;
	ratings.nausea += (totalLength * 26214) >> 16;

	ride_ratings_apply_proximity(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 83662);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;

	if ((sub_65E72D(ride) >> 8) >= 5)
		ride->excitement /= 4;
}

static void ride_ratings_calculate_vertical_drop_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(3,20), RIDE_RATING(0,80), RIDE_RATING(0,30));
	ride_ratings_apply_length(&ratings, ride, 4000, 1146);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_max_speed(&ratings, ride, 97418, 141699, 70849);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 40960, 35746, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 58254, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 20, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,10), 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 1, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_cash_machine(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_twist(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,13), RIDE_RATING(0,97), RIDE_RATING(1,90));

	ratings.excitement += ride->rotations * 20;
	ratings.intensity += ride->rotations * 20;
	ratings.nausea += ride->rotations * 20;

	ride_ratings_apply_scenery(&ratings, ride, 13943);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_haunted_house(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 8;
	set_unreliability_factor(ride);

	rating_tuple ratings = {
		.excitement = RIDE_RATING(3,41),
		.intensity = RIDE_RATING(1,53),
		.nausea = RIDE_RATING(0,10)
	};

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0xE0;
}

static void ride_ratings_calculate_flying_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 17;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(4,35), RIDE_RATING(1,85), RIDE_RATING(4,33));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 38130, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ratings.excitement /= 2;

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 1, 1);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,40), 2, 1, 1);

	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 1, 1);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_virginia_reel(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 19;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,10), RIDE_RATING(1,90), RIDE_RATING(3,70));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 110592, 29789, 59578);
	ride_ratings_apply_turns(&ratings, ride, 52012, 26075, 45749);
	ride_ratings_apply_drops(&ratings, ride, 43690, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 22367);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xD20000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_splash_boats(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 15;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,46), RIDE_RATING(0,35), RIDE_RATING(0,30));
	ride_ratings_apply_length(&ratings, ride, 2000, 7208);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_max_speed(&ratings, ride, 797059, 655360, 301111);
	ride_ratings_apply_duration(&ratings, ride, 500, 13107);
	ride_ratings_apply_turns(&ratings, ride, 22291, 20860, 4574);
	ride_ratings_apply_drops(&ratings, ride, 87381, 93622, 62259);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 22367);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_mini_helicopters(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 12;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,60), RIDE_RATING(0,40), RIDE_RATING(0,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,15), RIDE_RATING(0,00));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_turns(&ratings, ride, 14860, 0, 4574);
	ride_ratings_apply_drops(&ratings, ride, 8738, 0, 0);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 12850, 6553, 4681);
	ride_ratings_apply_proximity(&ratings, ride, 8946);
	ride_ratings_apply_scenery(&ratings, ride, 8366);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xA00000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 6 << 5;
}

static void ride_ratings_calculate_lay_down_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 18;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(3,85), RIDE_RATING(1,15), RIDE_RATING(2,75));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 38130, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0) {
		ratings.excitement /= 4;
		ratings.intensity /= 2;
		ratings.nausea /= 2;
	}

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
	}

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_suspended_monorail(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,15), RIDE_RATING(0,23), RIDE_RATING(0, 8));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_train_length(&ratings, ride, 93622);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 70849, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 218453);
	ride_ratings_apply_duration(&ratings, ride, 150, 21845);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 5140, 6553, 18724);
	ride_ratings_apply_proximity(&ratings, ride, 12525);
	ride_ratings_apply_scenery(&ratings, ride, 25098);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	sint32 edx = sub_65E72D(ride);
	if (((edx >> 8) & 0xFF) >= 4)
		ride->excitement /= 4;

	ride->inversions &= 0x1F;
	ride->inversions |= edx << 5;
}

static void ride_ratings_calculate_reverser_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 19;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,40), RIDE_RATING(1,80), RIDE_RATING(1,70));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);

	sint32 numReversers = min(gRideRatingsCalcData.num_reversers, 6);
	ride_rating reverserRating = numReversers * RIDE_RATING(0,20);
	ratings.excitement += reverserRating;
	ratings.intensity += reverserRating;
	ratings.nausea += reverserRating;

	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 28672, 23831, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 43458, 45749);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 22367);
	ride_ratings_apply_scenery(&ratings, ride, 11155);

	if (gRideRatingsCalcData.num_reversers < 1) {
		ratings.excitement /= 8;
	}

	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xC80000, 2, 1, 1);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 1, 1);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_heartline_twister_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 18;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,40), RIDE_RATING(1,70), RIDE_RATING(1,65));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,20), RIDE_RATING(0,04));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 97418, 123987, 70849);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 44683, 89367);
	ride_ratings_apply_turns(&ratings, ride, 26749, 52150, 57186);
	ride_ratings_apply_drops(&ratings, ride, 29127, 53052, 55705);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 34952, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 9841);
	ride_ratings_apply_scenery(&ratings, ride, 3904);

	if ((ride->inversions & 0x1F) == 0)
		ratings.excitement /= 4;

	ride_ratings_apply_num_drops_penalty(&ratings, ride, 1, 4, 1, 1);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_mini_golf(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 0;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,50), RIDE_RATING(0,90), RIDE_RATING(0,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_turns(&ratings, ride, 14860, 0, 0);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 5140, 6553, 4681);
	ride_ratings_apply_proximity(&ratings, ride, 15657);
	ride_ratings_apply_scenery(&ratings, ride, 27887);

	// Apply golf holes factor
	ratings.excitement += (ride->holes & 0x1F) * 5;

	// Apply no golf holes penalty
	if ((ride->inversions & 0x1F) == 0) {
		ratings.excitement /= 8;
		ratings.intensity /= 2;
		ratings.nausea /= 2;
	}

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_first_aid(rct_ride *ride)
{
	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;
}

static void ride_ratings_calculate_circus_show(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 9;
	set_unreliability_factor(ride);

	rating_tuple ratings = {
		.excitement	= RIDE_RATING(2,10),
		.intensity	= RIDE_RATING(0,30),
		.nausea		= RIDE_RATING(0,0)
	};

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 7 << 5;
}

static void ride_ratings_calculate_ghost_train(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 12;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,00), RIDE_RATING(0,20), RIDE_RATING(0,03));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,15), RIDE_RATING(0,00));
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_turns(&ratings, ride, 14860, 0, 11437);
	ride_ratings_apply_drops(&ratings, ride, 8738, 0, 0);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 25700, 6553, 4681);
	ride_ratings_apply_proximity(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 8366);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xB40000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_twister_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 15;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(3,50), RIDE_RATING(0,40), RIDE_RATING(0,30));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 32768, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
	}

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_wooden_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 19;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(3,20), RIDE_RATING(2,60), RIDE_RATING(2,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 40960, 34555, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 43458, 45749);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 22367);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,10), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x1720000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_side_friction_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 19;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,50), RIDE_RATING(2,00), RIDE_RATING(1,50));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 28672, 35746, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 43458, 45749);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 22367);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x50000, 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xFA0000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_wild_mouse(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,80), RIDE_RATING(2,50), RIDE_RATING(2,10));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0, 8));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 102400, 35746, 49648);
	ride_ratings_apply_turns(&ratings, ride, 29721, 43458, 45749);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 17893);
	ride_ratings_apply_scenery(&ratings, ride, 5577);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 6, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1,50), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_multi_dimension_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 18;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(3,75), RIDE_RATING(1,95), RIDE_RATING(4,79));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 38130, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ratings.excitement /= 4;

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 1, 1);
	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,40), 2, 1, 1);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 1, 1);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_giga_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(3,85), RIDE_RATING(0,40), RIDE_RATING(0,35));
	ride_ratings_apply_length(&ratings, ride, 6000, 819);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 140434);
	ride_ratings_apply_max_speed(&ratings, ride, 51366, 85019, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 400497);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 36864, 30384, 49648);
	ride_ratings_apply_turns(&ratings, ride, 28235, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 43690, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 16, 2, 2, 2);

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0, 40), 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
	}

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_roto_drop(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 24;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,80), RIDE_RATING(3,50), RIDE_RATING(3,50));

	sint32 lengthFactor = ((ride_get_total_length(ride) >> 16) * 209715) >> 16;
	ratings.excitement += lengthFactor;
	ratings.intensity += lengthFactor * 2;
	ratings.nausea += lengthFactor * 2;

	ride_ratings_apply_proximity(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 25098);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_flying_saucers(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 32;
	set_unreliability_factor(ride);

	rating_tuple ratings = {
		.excitement = RIDE_RATING(2,40),
		.intensity =  RIDE_RATING(0,55),
		.nausea =     RIDE_RATING(0,39)
	};

	if (ride->num_vehicles >= 4)
		ratings.excitement += RIDE_RATING(0,40);

	ratings.excitement += ride->time_limit;
	ratings.intensity += ride->time_limit / 2;

	if (ride->num_vehicles >= 4)
		ratings.excitement += RIDE_RATING(0,40);

	ride_ratings_apply_scenery(&ratings, ride, 5577);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_crooked_house(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 5;
	set_unreliability_factor(ride);

	rating_tuple ratings = {
		.excitement	= RIDE_RATING(2,15),
		.intensity	= RIDE_RATING(0,62),
		.nausea		= RIDE_RATING(0,34)
	};

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0xE0;
}

static void ride_ratings_calculate_monorail_cycles(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 4;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,40), RIDE_RATING(0,20), RIDE_RATING(0,00));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,15), RIDE_RATING(0,00));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_turns(&ratings, ride, 14860, 0, 4574);
	ride_ratings_apply_drops(&ratings, ride, 8738, 0, 0);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 5140, 6553, 2340);
	ride_ratings_apply_proximity(&ratings, ride, 8946);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x8C0000, 2, 2, 2);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_compact_inverted_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = ride->mode == RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE ? 31 : 21;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(3,15), RIDE_RATING(2,80), RIDE_RATING(3,20));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,42), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 30980, 55606);
	ride_ratings_apply_turns(&ratings, ride, 26749, 29552, 57186);
	ride_ratings_apply_drops(&ratings, ride, 29127, 39009, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 15291, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 15657);
	ride_ratings_apply_scenery(&ratings, ride, 8366);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,30), 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_water_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,70), RIDE_RATING(2,80), RIDE_RATING(2,10));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 20480, 23831, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 25700, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 9760);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 1, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	if (!(ride->special_track_elements & RIDE_ELEMENT_TUNNEL_SPLASH_OR_RAPIDS))
		ratings.excitement /= 8;

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_air_powered_vertical_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 28;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(4,13), RIDE_RATING(2,50), RIDE_RATING(2,80));
	ride_ratings_apply_length(&ratings, ride, 6000, 327);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,60), RIDE_RATING(0,05));
	ride_ratings_apply_max_speed(&ratings, ride, 509724, 364088, 320398);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 59578);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 21845, 11702);
	ride_ratings_apply_proximity(&ratings, ride, 17893);
	ride_ratings_apply_scenery(&ratings, ride, 11155);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 34, 2, 1, 1);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_inverted_hairpin_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 14;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(3,00), RIDE_RATING(2,65), RIDE_RATING(2,25));
	ride_ratings_apply_length(&ratings, ride, 6000, 873);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0, 8));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 364088, 655360);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 102400, 35746, 49648);
	ride_ratings_apply_turns(&ratings, ride, 29721, 43458, 45749);
	ride_ratings_apply_drops(&ratings, ride, 40777, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 16705, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 17893);
	ride_ratings_apply_scenery(&ratings, ride, 5577);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 8, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,10), 2, 2, 2);
	ride_ratings_apply_max_lateral_g_penalty(&ratings, ride, FIXED_2DP(1,50), 2, 2, 2);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0xAA0000, 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 3, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_magic_carpet(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,45), RIDE_RATING(1,60), RIDE_RATING(2,60));

	ratings.excitement += ride->operation_option * 10;
	ratings.intensity += ride->operation_option * 20;
	ratings.nausea += ride->operation_option * 20;

	ride_ratings_apply_scenery(&ratings, ride, 11155);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_submarine_ride(rct_ride *ride)
{
	ride->unreliability_factor = 7;
	set_unreliability_factor(ride);

	// NOTE Fixed bug from original game, see boat ride.

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,20), RIDE_RATING(1,80), RIDE_RATING(1,40));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_proximity(&ratings, ride, 11183);
	ride_ratings_apply_scenery(&ratings, ride, 22310);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 0 << 5;
}

static void ride_ratings_calculate_river_rafts(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 12;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(1,45), RIDE_RATING(0,25), RIDE_RATING(0,34));
	ride_ratings_apply_length(&ratings, ride, 2000, 7208);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_max_speed(&ratings, ride, 531372, 655360, 301111);
	ride_ratings_apply_duration(&ratings, ride, 500, 13107);
	ride_ratings_apply_turns(&ratings, ride, 22291, 20860, 4574);
	ride_ratings_apply_drops(&ratings, ride, 78643, 93622, 62259);
	ride_ratings_apply_proximity(&ratings, ride, 13420);
	ride_ratings_apply_scenery(&ratings, ride, 11155);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_enterprise(rct_ride *ride)
{
	ride->lifecycle_flags |= RIDE_LIFECYCLE_TESTED;
	ride->lifecycle_flags |= RIDE_LIFECYCLE_NO_RAW_STATS;
	ride->unreliability_factor = 22;
	set_unreliability_factor(ride);

	// Base ratings
	rating_tuple ratings = {
		.excitement	= RIDE_RATING(3,60),
		.intensity	= RIDE_RATING(4,55),
		.nausea		= RIDE_RATING(5,72)
	};

	ratings.excitement += ride->operation_option;
	ratings.intensity += ride->operation_option * 16;
	ratings.nausea += ride->operation_option * 16;

	ride_ratings_apply_scenery(&ratings, ride, 19521);

	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= 3 << 5;
}

static void ride_ratings_calculate_inverted_impulse_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 20;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(4,00), RIDE_RATING(3,00), RIDE_RATING(3,20));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,42), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 29789, 55606);
	ride_ratings_apply_turns(&ratings, ride, 26749, 29552, 57186);
	ride_ratings_apply_drops(&ratings, ride, 29127, 39009, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 15291, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 15657);
	ride_ratings_apply_scenery(&ratings, ride, 9760);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 20, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_mini_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 13;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,55), RIDE_RATING(2,40), RIDE_RATING(1,85));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 20480, 23831, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 25700, 30583, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 9760);
	ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 12, 2, 2, 2);
	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0x70000, 2, 2, 2);
	ride_ratings_apply_max_negative_g_penalty(&ratings, ride, FIXED_2DP(0,50), 2, 2, 2);
	ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_mine_ride(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 16;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,75), RIDE_RATING(1,00), RIDE_RATING(1,80));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 40960, 29789, 49648);
	ride_ratings_apply_turns(&ratings, ride, 29721, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 19275, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 21472);
	ride_ratings_apply_scenery(&ratings, ride, 16732);
	ride_ratings_apply_first_length_penalty(&ratings, ride, 0x10E0000, 2, 2, 2);

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

static void ride_ratings_calculate_lim_launched_roller_coaster(rct_ride *ride)
{
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED))
		return;

	ride->unreliability_factor = 25;
	set_unreliability_factor(ride);

	rating_tuple ratings;
	ride_ratings_set(&ratings, RIDE_RATING(2,90), RIDE_RATING(1,50), RIDE_RATING(2,20));
	ride_ratings_apply_length(&ratings, ride, 6000, 764);
	ride_ratings_apply_synchronisation(&ratings, ride, RIDE_RATING(0,40), RIDE_RATING(0,05));
	ride_ratings_apply_train_length(&ratings, ride, 187245);
	ride_ratings_apply_max_speed(&ratings, ride, 44281, 88562, 35424);
	ride_ratings_apply_average_speed(&ratings, ride, 291271, 436906);
	ride_ratings_apply_duration(&ratings, ride, 150, 26214);
	ride_ratings_apply_gforces(&ratings, ride, 24576, 35746, 49648);
	ride_ratings_apply_turns(&ratings, ride, 26749, 34767, 45749);
	ride_ratings_apply_drops(&ratings, ride, 29127, 46811, 49152);
	ride_ratings_apply_sheltered_ratings(&ratings, ride, 15420, 32768, 35108);
	ride_ratings_apply_proximity(&ratings, ride, 20130);
	ride_ratings_apply_scenery(&ratings, ride, 6693);

	if ((ride->inversions & 0x1F) == 0)
		ride_ratings_apply_highest_drop_height_penalty(&ratings, ride, 10, 2, 2, 2);

	ride_ratings_apply_max_speed_penalty(&ratings, ride, 0xA0000, 2, 2, 2);

	if ((ride->inversions & 0x1F) == 0) {
		ride_ratings_apply_max_negative_g_penalty(&ratings, ride, 10, 2, 2, 2);
		ride_ratings_apply_num_drops_penalty(&ratings, ride, 2, 2, 2, 2);
	}

	ride_ratings_apply_excessive_lateral_g_penalty(&ratings, ride);
	ride_ratings_apply_intensity_penalty(&ratings);
	ride_ratings_apply_adjustments(ride, &ratings);

	ride->ratings = ratings;

	ride->upkeep_cost = ride_compute_upkeep(ride);
	ride->window_invalidate_flags |= RIDE_INVALIDATE_RIDE_INCOME;

	ride->inversions &= 0x1F;
	ride->inversions |= sub_65E72D(ride) << 5;
}

#pragma endregion

#pragma region Ride rating calculation function table

// rct2: 0x0097E050
static const ride_ratings_calculation ride_ratings_calculate_func_table[RIDE_TYPE_COUNT] = {
	ride_ratings_calculate_spiral_roller_coaster,				// SPIRAL_ROLLER_COASTER
	ride_ratings_calculate_stand_up_roller_coaster,				// STAND_UP_ROLLER_COASTER
	ride_ratings_calculate_suspended_swinging_coaster,			// SUSPENDED_SWINGING_COASTER
	ride_ratings_calculate_inverted_roller_coaster,				// INVERTED_ROLLER_COASTER
	ride_ratings_calculate_junior_roller_coaster,				// JUNIOR_ROLLER_COASTER
	ride_ratings_calculate_miniature_railway,					// MINIATURE_RAILWAY
	ride_ratings_calculate_monorail,							// MONORAIL
	ride_ratings_calculate_mini_suspended_coaster,				// MINI_SUSPENDED_COASTER
	ride_ratings_calculate_boat_ride,							// BOAT_RIDE
	ride_ratings_calculate_wooden_wild_mouse,					// WOODEN_WILD_MOUSE
	ride_ratings_calculate_steeplechase,						// STEEPLECHASE
	ride_ratings_calculate_car_ride,							// CAR_RIDE
	ride_ratings_calculate_launched_freefall,					// LAUNCHED_FREEFALL
	ride_ratings_calculate_bobsleigh_coaster,					// BOBSLEIGH_COASTER
	ride_ratings_calculate_observation_tower,					// OBSERVATION_TOWER
	ride_ratings_calculate_looping_roller_coaster,				// LOOPING_ROLLER_COASTER
	ride_ratings_calculate_dinghy_slide,						// DINGHY_SLIDE
	ride_ratings_calculate_mine_train_coaster,					// MINE_TRAIN_COASTER
	ride_ratings_calculate_chairlift,							// CHAIRLIFT
	ride_ratings_calculate_corkscrew_roller_coaster,			// CORKSCREW_ROLLER_COASTER
	ride_ratings_calculate_maze,								// MAZE
	ride_ratings_calculate_spiral_slide,						// SPIRAL_SLIDE
	ride_ratings_calculate_go_karts,							// GO_KARTS
	ride_ratings_calculate_log_flume,							// LOG_FLUME
	ride_ratings_calculate_river_rapids,						// RIVER_RAPIDS
	ride_ratings_calculate_dodgems,								// DODGEMS
	ride_ratings_calculate_pirate_ship,							// PIRATE_SHIP
	ride_ratings_calculate_inverter_ship,						// SWINGING_INVERTER_SHIP
	ride_ratings_calculate_food_stall,							// FOOD_STALL
	ride_ratings_calculate_food_stall,							// 1D
	ride_ratings_calculate_drink_stall,							// DRINK_STALL
	ride_ratings_calculate_drink_stall,							// 1F
	ride_ratings_calculate_shop,								// SHOP
	ride_ratings_calculate_merry_go_round,						// MERRY_GO_ROUND
	ride_ratings_calculate_shop,								// 22
	ride_ratings_calculate_information_kiosk,					// INFORMATION_KIOSK
	ride_ratings_calculate_toilets,								// TOILETS
	ride_ratings_calculate_ferris_wheel,						// FERRIS_WHEEL
	ride_ratings_calculate_motion_simulator,					// MOTION_SIMULATOR
	ride_ratings_calculate_3d_cinema,							// 3D_CINEMA
	ride_ratings_calculate_top_spin,							// TOP_SPIN
	ride_ratings_calculate_space_rings,							// SPACE_RINGS
	ride_ratings_calculate_reverse_freefall_coaster,			// REVERSE_FREEFALL_COASTER
	ride_ratings_calculate_lift,								// LIFT
	ride_ratings_calculate_vertical_drop_roller_coaster,		// VERTICAL_DROP_ROLLER_COASTER
	ride_ratings_calculate_cash_machine,						// CASH_MACHINE
	ride_ratings_calculate_twist,								// TWIST
	ride_ratings_calculate_haunted_house,						// HAUNTED_HOUSE
	ride_ratings_calculate_first_aid,							// FIRST_AID
	ride_ratings_calculate_circus_show,							// CIRCUS_SHOW
	ride_ratings_calculate_ghost_train,							// GHOST_TRAIN
	ride_ratings_calculate_twister_roller_coaster,				// TWISTER_ROLLER_COASTER
	ride_ratings_calculate_wooden_roller_coaster,				// WOODEN_ROLLER_COASTER
	ride_ratings_calculate_side_friction_roller_coaster,		// SIDE_FRICTION_ROLLER_COASTER
	ride_ratings_calculate_wild_mouse,							// WILD_MOUSE
	ride_ratings_calculate_multi_dimension_roller_coaster,		// MULTI_DIMENSION_ROLLER_COASTER
	ride_ratings_calculate_multi_dimension_roller_coaster,		// 38
	ride_ratings_calculate_flying_roller_coaster,				// FLYING_ROLLER_COASTER
	ride_ratings_calculate_flying_roller_coaster,				// 3A
	ride_ratings_calculate_virginia_reel,						// VIRGINIA_REEL
	ride_ratings_calculate_splash_boats,						// SPLASH_BOATS
	ride_ratings_calculate_mini_helicopters,					// MINI_HELICOPTERS
	ride_ratings_calculate_lay_down_roller_coaster,				// LAY_DOWN_ROLLER_COASTER
	ride_ratings_calculate_suspended_monorail,					// SUSPENDED_MONORAIL
	ride_ratings_calculate_lay_down_roller_coaster,				// 40
	ride_ratings_calculate_reverser_roller_coaster,				// REVERSER_ROLLER_COASTER
	ride_ratings_calculate_heartline_twister_coaster,			// HEARTLINE_TWISTER_COASTER
	ride_ratings_calculate_mini_golf,							// MINI_GOLF
	ride_ratings_calculate_giga_coaster,						// GIGA_COASTER
	ride_ratings_calculate_roto_drop,							// ROTO_DROP
	ride_ratings_calculate_flying_saucers,						// FLYING_SAUCERS
	ride_ratings_calculate_crooked_house,						// CROOKED_HOUSE
	ride_ratings_calculate_monorail_cycles,						// MONORAIL_CYCLES
	ride_ratings_calculate_compact_inverted_coaster,			// COMPACT_INVERTED_COASTER
	ride_ratings_calculate_water_coaster,						// WATER_COASTER
	ride_ratings_calculate_air_powered_vertical_coaster,		// AIR_POWERED_VERTICAL_COASTER
	ride_ratings_calculate_inverted_hairpin_coaster,			// INVERTED_HAIRPIN_COASTER
	ride_ratings_calculate_magic_carpet,						// MAGIC_CARPET
	ride_ratings_calculate_submarine_ride,						// SUBMARINE_RIDE
	ride_ratings_calculate_river_rafts,							// RIVER_RAFTS
	NULL,														// 50
	ride_ratings_calculate_enterprise,							// ENTERPRISE
	NULL,														// 52
	NULL,														// 53
	NULL,														// 54
	NULL,														// 55
	ride_ratings_calculate_inverted_impulse_coaster,			// INVERTED_IMPULSE_COASTER
	ride_ratings_calculate_mini_roller_coaster,					// MINI_ROLLER_COASTER
	ride_ratings_calculate_mine_ride,							// MINE_RIDE
	NULL,														// 59
	ride_ratings_calculate_lim_launched_roller_coaster,			// LIM_LAUNCHED_ROLLER_COASTER
};

#pragma endregion
