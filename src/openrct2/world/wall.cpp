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

#include "../common.h"
#include "../network/network.h"

extern "C"
{
    #include "map.h"
	#include "park.h"
	#include "scenery.h"
	#include "../cheats.h"
	#include "../game.h"
	#include "../ride/track.h"
	#include "../ride/track_data.h"
	#include "../localisation/string_ids.h"
}

static bool gWallAcrossTrack;

extern "C"
{
	uint8 fence_get_animation_frame(rct_map_element *fenceElement) {
		return (fenceElement->properties.fence.animation >> 3) & 0xF;
	}
	
	rct_map_element *map_get_fence_element_at(sint32 x, sint32 y, sint32 z, sint32 direction)
	{
		rct_map_element *mapElement = map_get_first_element_at(x >> 5, y >> 5);
		do {
			if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_FENCE)
				continue;
			if (mapElement->base_height != z)
				continue;
			if (map_element_get_direction(mapElement) != direction)
				continue;

			return mapElement;
		} while (!map_element_is_last_for_tile(mapElement++));
		return NULL;
	}
	
	/**
	 *
	 *  rct2: 0x006E588E
	 */
	void map_remove_walls_at(sint32 x, sint32 y, sint32 z0, sint32 z1)
	{
		rct_map_element *mapElement;

		z0 /= 8;
		z1 /= 8;
	repeat:
		mapElement = map_get_first_element_at(x >> 5, y >> 5);
		do {
			if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_FENCE)
				continue;
			if (z0 >= mapElement->clearance_height)
				continue;
			if (z1 <= mapElement->base_height)
				continue;

			map_element_remove_banner_entry(mapElement);
			map_invalidate_tile_zoom1(x, y, mapElement->base_height * 8, mapElement->base_height * 8 + 72);
			map_element_remove(mapElement);
			goto repeat;
		} while (!map_element_is_last_for_tile(mapElement++));
	}
		
	/**
	 *
	 *  rct2: 0x006E57E6
	 */
	void map_remove_walls_at_z(sint32 x, sint32 y, sint32 z)
	{
		map_remove_walls_at(x, y, z, z + 48);
	}

	/**
	 *
	 *  rct2: 0x006E5935
	 */
	void map_remove_intersecting_walls(sint32 x, sint32 y, sint32 z0, sint32 z1, sint32 direction)
	{
		rct_map_element *mapElement;

		mapElement = map_get_first_element_at(x >> 5, y >> 5);
		do {
			if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_FENCE)
				continue;

			if (mapElement->clearance_height <= z0 || mapElement->base_height >= z1)
				continue;

			if (direction != (mapElement->type & 3))
				continue;

			map_element_remove_banner_entry(mapElement);
			map_invalidate_tile_zoom1(x, y, mapElement->base_height * 8, mapElement->base_height * 8 + 72);
			map_element_remove(mapElement);
			mapElement--;
		} while (!map_element_is_last_for_tile(mapElement++));
	}
	
	/**
	 *
	 *  rct2: 0x006E5CBA
	 */
	static bool map_place_fence_check_obstruction_with_track(rct_scenery_entry *wall, sint32 x, sint32 y, sint32 z0, sint32 z1, sint32 edge, rct_map_element *trackElement)
	{
		const rct_preview_track *trackBlock;
		sint32 z, direction;

		sint32 trackType = trackElement->properties.track.type;
		sint32 sequence = trackElement->properties.track.sequence & 0x0F;
		direction = (edge - trackElement->type) & 3;
		rct_ride *ride = get_ride(trackElement->properties.track.ride_index);

		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
			if (FlatRideTrackSequenceElementAllowedWallEdges[trackType][sequence] & (1 << direction)) {
				if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_TRACK_NO_WALLS)) {
					return true;
				}
			}
		} else {
			if (TrackSequenceElementAllowedWallEdges[trackType][sequence] & (1 << direction)) {
				if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_TRACK_NO_WALLS)) {
					return true;
				}
			}
		}

		if (!(wall->wall.flags & WALL_SCENERY_IS_DOOR)) {
			return false;
		}

		if (!(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_0)) {
			return false;
		}

		rct_ride_entry *rideEntry = get_ride_entry(ride->subtype);
		if (rideEntry->flags & RIDE_ENTRY_FLAG_16) {
			return false;
		}

		gWallAcrossTrack = true;
		if (z0 & 1) {
			return false;
		}

		if (sequence == 0) {
			if (TrackSequenceProperties[trackType][0] & TRACK_SEQUENCE_FLAG_DISALLOW_DOORS) {
				return false;
			}

			if (TrackDefinitions[trackType].bank_start == 0) {
				if (!(TrackCoordinates[trackType].rotation_begin & 4)) {
					direction = (trackElement->type & 3) ^ 2;
					if (direction == edge) {
						trackBlock = &TrackBlocks[trackType][sequence];
						z = TrackCoordinates[trackType].z_begin;
						z = trackElement->base_height + ((z - trackBlock->z) * 8);
						if (z == z0) {
							return true;
						}
					}
				}
			}
		}

		trackBlock = &TrackBlocks[trackType][sequence + 1];
		if (trackBlock->index != 0xFF) {
			return false;
		}

		if (TrackDefinitions[trackType].bank_end != 0) {
			return false;
		}

		direction = TrackCoordinates[trackType].rotation_end;
		if (direction & 4) {
			return false;
		}

		direction = (trackElement->type + direction) & 3;
		if (direction != edge) {
			return false;
		}

		trackBlock = &TrackBlocks[trackType][sequence];
		z = TrackCoordinates[trackType].z_end;
		z = trackElement->base_height + ((z - trackBlock->z) * 8);
		if (z != z0) {
			return false;
		}

		return true;
	}

	/**
	 *
	 *  rct2: 0x006E5C1A
	 */
	static bool map_place_fence_check_obstruction(rct_scenery_entry *wall, sint32 x, sint32 y, sint32 z0, sint32 z1, sint32 edge)
	{
		sint32 entryType, sequence;
		rct_scenery_entry *entry;
		rct_large_scenery_tile *tile;

		gWallAcrossTrack = false;
		gMapGroundFlags = ELEMENT_IS_ABOVE_GROUND;
		if (map_is_location_at_edge(x, y)) {
			gGameCommandErrorText = STR_OFF_EDGE_OF_MAP;
			return false;
		}

		rct_map_element *mapElement = map_get_first_element_at(x / 32, y / 32);
		do {
			sint32 elementType = map_element_get_type(mapElement);
			if (elementType == MAP_ELEMENT_TYPE_SURFACE) continue;
			if (z0 >= mapElement->clearance_height) continue;
			if (z1 <= mapElement->base_height) continue;
			if (elementType == MAP_ELEMENT_TYPE_FENCE) {
				sint32 direction = mapElement->type & 3;
				if (edge == direction) {
					map_obstruction_set_error_text(mapElement);
					return false;
				}
				continue;
			}
			if ((mapElement->flags & 0x0F) == 0) continue;

			switch (elementType) {
			case MAP_ELEMENT_TYPE_ENTRANCE:
				map_obstruction_set_error_text(mapElement);
				return false;
			case MAP_ELEMENT_TYPE_PATH:
				if (mapElement->properties.path.edges & (1 << edge)) {
					map_obstruction_set_error_text(mapElement);
					return false;
				}
				break;
			case MAP_ELEMENT_TYPE_SCENERY_MULTIPLE:
				entryType = mapElement->properties.scenerymultiple.type & 0x3FF;
				sequence = mapElement->properties.scenerymultiple.type >> 10;
				entry = get_large_scenery_entry(entryType);
				tile = &entry->large_scenery.tiles[sequence];
				{
					sint32 direction = ((edge - mapElement->type) & 3) + 8;
					if (!(tile->var_7 & (1 << direction))) {
						map_obstruction_set_error_text(mapElement);
						return false;
					}
				}
				break;
			case MAP_ELEMENT_TYPE_SCENERY:
				entryType = mapElement->properties.scenery.type;
				entry = get_small_scenery_entry(entryType);
				if (entry->small_scenery.flags & SMALL_SCENERY_FLAG_ALLOW_WALLS) {
					map_obstruction_set_error_text(mapElement);
					return false;
				}
				break;
			case MAP_ELEMENT_TYPE_TRACK:
				if (!map_place_fence_check_obstruction_with_track(wall, x, y, z0, z1, edge, mapElement)) {
					return false;
				}
				break;
			}
		} while (!map_element_is_last_for_tile(mapElement++));
		return true;
	}
	
	enum
	{
		EDGE_SLOPE_ELEVATED = (1 << 0), // 0x01
		EDGE_SLOPE_UPWARDS = (1 << 6), // 0x40
		EDGE_SLOPE_DOWNWARDS = (1 << 7), // 0x80

		EDGE_SLOPE_UPWARDS_ELEVATED = EDGE_SLOPE_UPWARDS | EDGE_SLOPE_ELEVATED,
		EDGE_SLOPE_DOWNWARDS_ELEVATED = EDGE_SLOPE_DOWNWARDS | EDGE_SLOPE_ELEVATED,
	};

	/** rct2: 0x009A3FEC */
	static const uint8 EdgeSlopes[][4] = {
	//	  Top right							Bottom right					Bottom left						Top left
		{ 0,								0,								0,								0								},
		{ 0,								EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_DOWNWARDS,			0								},
		{ 0,								0,								EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_DOWNWARDS			},
		{ 0,								EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_ELEVATED,			EDGE_SLOPE_DOWNWARDS			},
		{ EDGE_SLOPE_DOWNWARDS,				0,								0,								EDGE_SLOPE_UPWARDS				},
		{ EDGE_SLOPE_DOWNWARDS,				EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_DOWNWARDS,			EDGE_SLOPE_UPWARDS				},
		{ EDGE_SLOPE_DOWNWARDS,				0,								EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_ELEVATED				},
		{ EDGE_SLOPE_DOWNWARDS,				EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_ELEVATED,			EDGE_SLOPE_ELEVATED				},
		{ EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_DOWNWARDS,			0,								0								},
		{ EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_ELEVATED,			EDGE_SLOPE_DOWNWARDS,			0								},
		{ EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_DOWNWARDS,			EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_DOWNWARDS			},
		{ EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_ELEVATED,			EDGE_SLOPE_ELEVATED,			EDGE_SLOPE_DOWNWARDS			},
		{ EDGE_SLOPE_ELEVATED,				EDGE_SLOPE_DOWNWARDS,			0,								EDGE_SLOPE_UPWARDS				},
		{ EDGE_SLOPE_ELEVATED,				EDGE_SLOPE_ELEVATED,			EDGE_SLOPE_DOWNWARDS,			EDGE_SLOPE_UPWARDS				},
		{ EDGE_SLOPE_ELEVATED,				EDGE_SLOPE_DOWNWARDS,			EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_ELEVATED				},
		{ EDGE_SLOPE_ELEVATED,				EDGE_SLOPE_ELEVATED,			EDGE_SLOPE_ELEVATED,			EDGE_SLOPE_ELEVATED				},
		{ 0,								0,								0,								0								},
		{ 0,								0,								0,								0								},
		{ 0,								0,								0,								0								},
		{ 0,								0,								0,								0								},
		{ 0,								0,								0,								0								},
		{ 0,								0,								0,								0								},
		{ 0,								0,								0,								0								},
		{ EDGE_SLOPE_DOWNWARDS,				EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_UPWARDS_ELEVATED,	EDGE_SLOPE_DOWNWARDS_ELEVATED	},
		{ 0,								0,								0,								0								},
		{ 0,								0,								0,								0								},
		{ 0,								0,								0,								0								},
		{ EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_UPWARDS_ELEVATED,	EDGE_SLOPE_DOWNWARDS_ELEVATED,	EDGE_SLOPE_DOWNWARDS			},
		{ 0,								0,								0,								0								},
		{ EDGE_SLOPE_UPWARDS_ELEVATED,		EDGE_SLOPE_DOWNWARDS_ELEVATED,	EDGE_SLOPE_DOWNWARDS,			EDGE_SLOPE_UPWARDS				},
		{ EDGE_SLOPE_DOWNWARDS_ELEVATED,	EDGE_SLOPE_DOWNWARDS,			EDGE_SLOPE_UPWARDS,				EDGE_SLOPE_UPWARDS_ELEVATED		},
		{ 0,								0,								0,								0								},
	};

	/**
	 *
	 *  rct2: 0x006E519A
	 */
	void game_command_place_fence(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp){
		rct_xyz16 position = {
			*eax & 0xFFFF,
			*ecx & 0xFFFF,
			*edi & 0xFFFF
		};

		uint8 flags = *ebx & 0xFF;
		uint8 fence_type = (*ebx >> 8) & 0xFF;
		uint8 primary_colour = (*edx >> 8) & 0xFF;
		uint8 secondary_colour = *ebp & 0xFF;
		uint8 tertiary_colour = (*ebp >> 8) & 0xFF;
		uint8 edge = *edx & 0xFF;

		gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
		gCommandPosition.x = position.x + 16;
		gCommandPosition.y = position.y + 16;
		gCommandPosition.z = position.z;

		if (position.z == 0){
			gCommandPosition.z = map_element_height(position.x, position.y) & 0xFFFF;
		}

		if (game_is_paused() && !gCheatsBuildInPauseMode){
			gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
			*ebx = MONEY32_UNDEFINED;
			return;
		}

		if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) &&
			!(flags & GAME_COMMAND_FLAG_7) && !gCheatsSandboxMode){

			if (position.z == 0){
				if (!map_is_location_in_park(position.x, position.y)){
					*ebx = MONEY32_UNDEFINED;
					return;
				}
			}
			else if (!map_is_location_owned(position.x, position.y, position.z)){
				*ebx = MONEY32_UNDEFINED;
				return;
			}
		}

		uint8 bp = 0;
		if (position.z == 0){
			rct_map_element* map_element = map_get_surface_element_at(position.x / 32, position.y / 32);
			if (map_element == NULL){
				*ebx = MONEY32_UNDEFINED;
				return;
			}
			position.z = map_element->base_height * 8;

			uint8 slope = map_element->properties.surface.slope & MAP_ELEMENT_SLOPE_MASK;
			bp = EdgeSlopes[slope][edge & 3];
			if (bp & EDGE_SLOPE_ELEVATED) {
				position.z += 16;
				bp &= ~(1 << 0);
			}
		}

		rct_map_element* map_element = map_get_surface_element_at(position.x / 32, position.y / 32);
		if (map_element == NULL){
			*ebx = MONEY32_UNDEFINED;
			return;
		}

		if (map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK){
			uint16 water_height = map_element->properties.surface.terrain & MAP_ELEMENT_WATER_HEIGHT_MASK;
			water_height *= 16;

			if (position.z < water_height){
				gGameCommandErrorText = STR_CANT_BUILD_THIS_UNDERWATER;
				*ebx = MONEY32_UNDEFINED;
				return;
			}
		}

		if (position.z / 8 < map_element->base_height){
			gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
			*ebx = MONEY32_UNDEFINED;
			return;
		}

		if (!(bp & 0xC0)){
			uint8 new_edge = (edge + 2) & 3;
			uint8 new_base_height = map_element->base_height;
			new_base_height += 2;
			if (map_element->properties.surface.slope & (1 << new_edge)){
				if (position.z / 8 < new_base_height){
					gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
					*ebx = MONEY32_UNDEFINED;
					return;
				}

				if (map_element->properties.surface.slope & (1 << 4)){
					new_edge = (new_edge - 1) & 3;

					if (map_element->properties.surface.slope & (1 << new_edge)){
						new_edge = (new_edge + 2) & 3;
						if (map_element->properties.surface.slope & (1 << new_edge)){
							new_base_height += 2;
							if (position.z / 8 < new_base_height){
								gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
								*ebx = MONEY32_UNDEFINED;
								return;
							}
							new_base_height -= 2;
						}
					}
				}
			}

			new_edge = (edge + 3) & 3;
			if (map_element->properties.surface.slope & (1 << new_edge)){
				if (position.z / 8 < new_base_height){
					gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
					*ebx = MONEY32_UNDEFINED;
					return;
				}

				if (map_element->properties.surface.slope & (1 << 4)){
					new_edge = (new_edge - 1) & 3;

					if (map_element->properties.surface.slope & (1 << new_edge)){
						new_edge = (new_edge + 2) & 3;
						if (map_element->properties.surface.slope & (1 << new_edge)){
							new_base_height += 2;
							if (position.z / 8 < new_base_height){
								gGameCommandErrorText = STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND;
								*ebx = MONEY32_UNDEFINED;
								return;
							}
						}
					}
				}
			}
		}
		sint32 banner_index = 0xFF;
		rct_scenery_entry* fence = get_wall_entry(fence_type);
		// Have to check both -1 and NULL, as one can be a invalid object,
		// while the other can be invalid index
		if ((uintptr_t)fence == (uintptr_t)-1 || fence == NULL) {
			*ebx = MONEY32_UNDEFINED;
			return;
		}
		if (fence->wall.scrolling_mode != 0xFF){
			banner_index = create_new_banner(flags);

			if (banner_index == 0xFF){
				*ebx = MONEY32_UNDEFINED;
				return;
			}

			rct_banner* banner = &gBanners[banner_index];
			if (flags & GAME_COMMAND_FLAG_APPLY){
				banner->flags |= (1 << 3);
				banner->type = 0;
				banner->x = position.x / 32;
				banner->y = position.y / 32;

				sint32 rideIndex = banner_get_closest_ride_index(position.x, position.y, position.z);
				if (rideIndex != -1) {
					banner->colour = rideIndex & 0xFF;
					banner->flags |= BANNER_FLAG_LINKED_TO_RIDE;
				}
			}
		}

		uint8 clearanceHeight = position.z / 8;
		if (bp & (EDGE_SLOPE_UPWARDS | EDGE_SLOPE_DOWNWARDS)) {
			if (fence->wall.flags & WALL_SCENERY_CANT_BUILD_ON_SLOPE){
				gGameCommandErrorText = STR_ERR_UNABLE_TO_BUILD_THIS_ON_SLOPE;
				*ebx = MONEY32_UNDEFINED;
				return;
			}
			clearanceHeight += 2;
		}
		clearanceHeight += fence->wall.height;

		if (!(flags & GAME_COMMAND_FLAG_7) && !gCheatsDisableClearanceChecks){
			if (!map_place_fence_check_obstruction(fence, position.x, position.y, position.z / 8, clearanceHeight, edge)) {
				*ebx = MONEY32_UNDEFINED;
				return;
			}
		}

		if (!map_check_free_elements_and_reorganise(1)){
			*ebx = MONEY32_UNDEFINED;
			return;
		}

		if (flags & GAME_COMMAND_FLAG_APPLY){
			if (gGameCommandNestLevel == 1 && !(*ebx & GAME_COMMAND_FLAG_GHOST)) {
				rct_xyz16 coord;
				coord.x = position.x + 16;
				coord.y = position.y + 16;
				coord.z = map_element_height(coord.x, coord.y);
				network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
			}

			map_element = map_element_insert(position.x / 32, position.y / 32, position.z / 8, 0);
			assert(map_element != NULL);

			map_animation_create(MAP_ANIMATION_TYPE_WALL, position.x, position.y, position.z / 8);

			map_element->clearance_height = clearanceHeight;

			map_element->type = bp | edge | MAP_ELEMENT_TYPE_FENCE;

			map_element->properties.fence.colour_1 = primary_colour;
			map_element->properties.fence.colour_1 |= (secondary_colour & 7) << 5;
			map_element->flags |= (secondary_colour & 0x18) << 2;

			if (gWallAcrossTrack) {
				map_element->properties.fence.animation |= (1 << 2);
			}

			map_element->properties.fence.type = fence_type;
			if (banner_index != 0xFF){
				map_element->properties.fence.banner_index = banner_index;
			}

			if (fence->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR){
				map_element->properties.fence.colour_3 = tertiary_colour;
			}

			if (flags & (1 << 6)){
				map_element->flags |= MAP_ELEMENT_FLAG_GHOST;
			}

			gSceneryMapElement = map_element;
			map_invalidate_tile_zoom1(position.x, position.y, map_element->base_height * 8, map_element->base_height * 8 + 72);
		}

		if (gParkFlags & PARK_FLAGS_NO_MONEY){
			*ebx = 0;
		}
		else{
			*ebx = fence->wall.price;
		}
	}

	money32 map_place_fence(
		sint32 type, sint32 x, sint32 y, sint32 z, sint32 edge, sint32 primaryColour, sint32 secondaryColour, sint32 tertiaryColour, sint32 flags
	) {
		sint32 eax, ebx, ecx, edx, esi, edi, ebp;

		eax = x;
		ebx = flags | (type << 8);
		ecx = y;
		edx = edge | (primaryColour << 8);
		edi = z;
		ebp = secondaryColour | (tertiaryColour << 8);
		game_command_place_fence(&eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
		return ebx;
	}

	
	/**
	 *
	 *  rct2: 0x006E5597
	 */
	void game_command_remove_fence(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
	{
		sint32 x = *eax;
		sint32 y = *ecx;
		if (!map_is_location_valid(x, y)) {
			*ebx = MONEY32_UNDEFINED;
			return;
		}
		uint8 base_height = (*edx >> 8);
		uint8 direction = *edx;
		uint8 flags = *ebx & 0xFF;

		gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
		if(!(flags & GAME_COMMAND_FLAG_GHOST) && game_is_paused() && !gCheatsBuildInPauseMode){
			gGameCommandErrorText = STR_CONSTRUCTION_NOT_POSSIBLE_WHILE_GAME_IS_PAUSED;
			*ebx = MONEY32_UNDEFINED;
			return;
		}
		if(!(flags & GAME_COMMAND_FLAG_GHOST) && !(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode && !map_is_location_owned(x, y, base_height * 8)){
			*ebx = MONEY32_UNDEFINED;
			return;
		}

		bool sceneryFound = false;
		rct_map_element* map_element = map_get_first_element_at(x / 32, y / 32);
		do {
			if (map_element_get_type(map_element) != MAP_ELEMENT_TYPE_FENCE)
				continue;
			if (map_element->base_height != base_height)
				continue;
			if ((map_element->type & MAP_ELEMENT_DIRECTION_MASK) != direction)
				continue;
			if ((flags & GAME_COMMAND_FLAG_GHOST) && !(map_element->flags & MAP_ELEMENT_FLAG_GHOST))
				continue;

			sceneryFound = true;
			break;
		} while (!map_element_is_last_for_tile(map_element++));

		if (!(*ebx & GAME_COMMAND_FLAG_APPLY) || (sceneryFound == false)) {
			*ebx = 0;
			return;
		}

		if (gGameCommandNestLevel == 1 && !(flags & GAME_COMMAND_FLAG_GHOST)) {
			rct_xyz16 coord;
			coord.x = x + 16;
			coord.y = y + 16;
			coord.z = map_element_height(coord.x, coord.y);
			network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
		}

		map_element_remove_banner_entry(map_element);
		map_invalidate_tile_zoom1(x, y, map_element->base_height * 8, (map_element->base_height * 8) + 72);
		map_element_remove(map_element);
		*ebx = 0;
	}
	
	/**
	 *
	 *  rct2: 0x006E56B5
	 */
	void game_command_set_fence_colour(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
	{
		gCommandExpenditureType = RCT_EXPENDITURE_TYPE_LANDSCAPING;
		sint32 x = *eax;
		sint32 y = *ecx;
		uint8 map_element_direction = *edx;
		uint8 base_height = *edx >> 8;
		uint8 colour1 = *ebx >> 8;
		uint8 colour2 = *ebp;
		uint8 colour3 = *ebp >> 8;
		uint8 flags = *ebx & 0xFF;
		sint32 z = base_height * 8;

		gCommandPosition.x = x + 16;
		gCommandPosition.y = y + 16;
		gCommandPosition.z = z;

		if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) &&
			!map_is_location_in_park(x, y) &&
			!gCheatsSandboxMode) {

			*ebx = MONEY32_UNDEFINED;
			return;
		}

		rct_map_element* map_element = map_get_fence_element_at(x, y, base_height, map_element_direction);

		if (map_element == NULL) {
			*ebx = 0;
			return;
		}

		if ((flags & GAME_COMMAND_FLAG_GHOST) && !(map_element->flags & MAP_ELEMENT_FLAG_GHOST)) {
			*ebx = 0;
			return;
		}

		if(flags & GAME_COMMAND_FLAG_APPLY){
			rct_scenery_entry* scenery_entry = get_wall_entry(map_element->properties.fence.type);
			map_element->properties.fence.colour_1 &= 0xE0;
			map_element->properties.fence.colour_1 |= colour1;
			map_element->properties.fence.colour_1 &= 0x1F;
			map_element->flags &= 0x9F;
			map_element->properties.fence.colour_1 |= (colour2 & 0x7) * 32;
			map_element->flags |= (colour2 & 0x18) * 4;

			if(scenery_entry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR){
				map_element->properties.fence.colour_3 = colour3;
			}
			map_invalidate_tile_zoom1(x, y, z, z + 72);
		}

		*ebx = 0;
	}
}
