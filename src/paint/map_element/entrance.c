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

#include "../paint.h"
#include "../../addresses.h"
#include "../../config.h"
#include "../../game.h"
#include "../../interface/viewport.h"
#include "../../localisation/localisation.h"
#include "../supports.h"
#include "../../ride/ride_data.h"
#include "../../world/entrance.h"
#include "../../world/footpath.h"

/**
 *
 *  rct2: 0x0066508C, 0x00665540
 */
void ride_entrance_exit_paint(uint8 direction, int height, rct_map_element* map_element)
{
	rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);
	uint8 is_exit = map_element->properties.entrance.type == ENTRANCE_TYPE_RIDE_EXIT;

	if (RCT2_GLOBAL(0x9DEA6F, uint8_t) & 1){
		if (map_element->properties.entrance.ride_index != RCT2_GLOBAL(0x00F64DE8, uint8))
			return;
	}

	rct_ride* ride = get_ride(map_element->properties.entrance.ride_index);
	if (ride->entrance_style == RIDE_ENTRANCE_STYLE_NONE) return;

	const rct_ride_entrance_definition *style = &RideEntranceDefinitions[ride->entrance_style];

	uint8 colour_1, colour_2;
	uint32 transparant_image_id = 0, image_id = 0;
	if (style->flags & (1 << 30)) {
		colour_1 = ride->track_colour_main[0] + 0x70;
		transparant_image_id = (colour_1 << 19) | 0x40000000;
	}

	colour_1 = ride->track_colour_main[0];
	colour_2 = ride->track_colour_additional[0];
	image_id = (colour_1 << 19) | (colour_2 << 24) | 0xA0000000;

	RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_RIDE;
	RCT2_GLOBAL(0x009E32BC, uint32) = 0;

	if (map_element->flags & MAP_ELEMENT_FLAG_GHOST){
		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_NONE;
		image_id = RCT2_ADDRESS(0x993CC4, uint32_t)[gConfigGeneral.construction_marker_colour];
		RCT2_GLOBAL(0x009E32BC, uint32) = image_id;
		if (transparant_image_id)
			transparant_image_id = image_id;
	}

	if (is_exit){
		image_id |= style->sprite_index + direction + 8;
	}
	else{
		image_id |= style->sprite_index + direction;
	}
	// Format modifed to stop repeated code

	// Each entrance is split into 2 images for drawing
	// Certain entrance styles have another 2 images to draw for coloured windows

	sint8 ah = is_exit ? 0x23 : 0x33;

	sint16 lengthY = (direction & 1) ? 28 : 2;
	sint16 lengthX = (direction & 1) ? 2 : 28;

	sub_98197C(image_id, 0, 0, lengthX, lengthY, ah, height, 2, 2, height, get_current_rotation());

	if (transparant_image_id){
		if (is_exit){
			transparant_image_id |= style->sprite_index + direction + 24;
		}
		else{
			transparant_image_id |= style->sprite_index + direction + 16;
		}

		sub_98199C(transparant_image_id, 0, 0, lengthX, lengthY, ah, height, 2, 2, height, get_current_rotation());
	}

	image_id += 4;

	sub_98197C(image_id, 0, 0, lengthX, lengthY, ah, height, (direction & 1) ? 28 : 2, (direction & 1) ? 2 : 28, height, get_current_rotation());

	if (transparant_image_id){
		transparant_image_id += 4;
		sub_98199C(transparant_image_id, 0, 0, lengthX, lengthY, ah, height, (direction & 1) ? 28 : 2, (direction & 1) ? 2 : 28, height, get_current_rotation());
	}

	uint32 eax = 0xFFFF0600 | ((height / 16) & 0xFF);
	if (direction & 1){
		RCT2_ADDRESS(0x009E30B6, uint32)[RCT2_GLOBAL(0x141F56B, uint8) / 2] = eax;
		RCT2_GLOBAL(0x141F56B, uint8)++;
	}
	else{
		RCT2_ADDRESS(0x009E3138, uint32)[RCT2_GLOBAL(0x141F56A, uint8) / 2] = eax;
		RCT2_GLOBAL(0x141F56A, uint8)++;
	}

	if (!is_exit &&
		!(map_element->flags & MAP_ELEMENT_FLAG_GHOST) &&
		map_element->properties.entrance.ride_index != 0xFF){

		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint32) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 4, uint32) = 0;

		rct_string_id string_id = STR_RIDE_ENTRANCE_CLOSED;

		if (ride->status == RIDE_STATUS_OPEN &&
			!(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)){

			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, rct_string_id) = ride->name;
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, uint32) = ride->name_arguments;

			string_id = STR_RIDE_ENTRANCE_NAME;
		}

		utf8 entrance_string[MAX_PATH];
		if (gConfigGeneral.upper_case_banners) {
			format_string_to_upper(entrance_string, string_id, RCT2_ADDRESS(RCT2_ADDRESS_COMMON_FORMAT_ARGS, void));
		} else {
			format_string(entrance_string, string_id, RCT2_ADDRESS(RCT2_ADDRESS_COMMON_FORMAT_ARGS, void));
		}

		gCurrentFontSpriteBase = FONT_SPRITE_BASE_TINY;

		uint16 string_width = gfx_get_string_width(entrance_string);
		uint16 scroll = (gCurrentTicks / 2) % string_width;

		sub_98199C(scrolling_text_setup(string_id, scroll, style->scrolling_mode), 0, 0, 0x1C, 0x1C, 0x33, height + style->height, 2, 2, height + style->height, get_current_rotation());
	}

	image_id = RCT2_GLOBAL(0x009E32BC, uint32);
	if (image_id == 0) {
		image_id = SPRITE_ID_PALETTE_COLOUR_1(COLOUR_SATURATED_BROWN);
	}
	wooden_a_supports_paint_setup(direction & 1, 0, height, image_id, NULL);

	RCT2_GLOBAL(0x141E9B4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9B8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9BC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D4, uint16) = 0xFFFF;

	height += is_exit ? 40 : 56;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height){
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x141E9DA, uint8) = 32;
	}
}

/**
 *
 *  rct2: 0x006658ED
 */
void park_entrance_paint(uint8 direction, int height, rct_map_element* map_element){
	if (RCT2_GLOBAL(0x9DEA6F, uint8_t) & 1)
		return;

	RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_PARK;
	RCT2_GLOBAL(0x009E32BC, uint32) = 0;
	uint32 image_id, ghost_id = 0;
	if (map_element->flags & MAP_ELEMENT_FLAG_GHOST){
		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_NONE;
		ghost_id = RCT2_ADDRESS(0x993CC4, uint32)[gConfigGeneral.construction_marker_colour];
		RCT2_GLOBAL(0x009E32BC, uint32) = ghost_id;
	}

	rct_path_type* path_entry = g_pathTypeEntries[map_element->properties.entrance.path_type];

	// Index to which part of the entrance
	// Middle, left, right
	uint8 part_index = map_element->properties.entrance.index & 0xF;
	rct_entrance_type* entrance;
	uint8 di = (direction / 2 + part_index / 2) & 1 ? 0x1A : 0x20;

	switch (part_index){
	case 0:
		image_id = (path_entry->image + 5 * (1 + (direction & 1))) | ghost_id;
			sub_98197C(image_id, 0, 0, 32, 0x1C, 0, height, 0, 2, height, get_current_rotation());

		entrance = (rct_entrance_type*)object_entry_groups[OBJECT_TYPE_PARK_ENTRANCE].chunks[0];
		image_id = (entrance->image_id + direction * 3) | ghost_id;
			sub_98197C(image_id, 0, 0, 0x1C, 0x1C, 0x2F, height, 2, 2, height + 32, get_current_rotation());

		if ((direction + 1) & (1 << 1))
			break;
		if (ghost_id != 0)
			break;

		rct_string_id park_text_id = 1730;
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint32) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 4, uint32) = 0;

		if (gParkFlags & PARK_FLAGS_PARK_OPEN){
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, rct_string_id) = gParkName;
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, uint32) = gParkNameArgs;

			park_text_id = 1731;
		}

		utf8 park_name[MAX_PATH];
		if (gConfigGeneral.upper_case_banners) {
			format_string_to_upper(park_name, park_text_id, RCT2_ADDRESS(RCT2_ADDRESS_COMMON_FORMAT_ARGS, void));
		} else {
			format_string(park_name, park_text_id, RCT2_ADDRESS(RCT2_ADDRESS_COMMON_FORMAT_ARGS, void));
		}

		gCurrentFontSpriteBase = FONT_SPRITE_BASE_TINY;
		uint16 string_width = gfx_get_string_width(park_name);
		uint16 scroll = (gCurrentTicks / 2) % string_width;

		if (entrance->scrolling_mode == 0xFF)
			break;

			sub_98199C(scrolling_text_setup(park_text_id, scroll, entrance->scrolling_mode + direction / 2), 0, 0, 0x1C, 0x1C, 0x2F, height + entrance->text_height, 2, 2, height + entrance->text_height, get_current_rotation());
		break;
	case 1:
	case 2:
		entrance = (rct_entrance_type*)object_entry_groups[OBJECT_TYPE_PARK_ENTRANCE].chunks[0];
		image_id = (entrance->image_id + part_index + direction * 3) | ghost_id;
			sub_98197C(image_id, 0, 0, 0x1A, di, 0x4F, height, 3, 3, height, get_current_rotation());
		break;
	}

	image_id = ghost_id;
	if (image_id == 0) {
		image_id = SPRITE_ID_PALETTE_COLOUR_1(COLOUR_SATURATED_BROWN);
	}
	wooden_a_supports_paint_setup(direction & 1, 0, height, image_id, NULL);

	RCT2_GLOBAL(0x141E9B4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9B8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9BC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C4, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
	RCT2_GLOBAL(0x141E9D4, uint16) = 0xFFFF;

	height += 80;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height){
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x141E9DA, uint8) = 32;
	}
}

/**
 *
 *  rct2: 0x00664FD4
 */
void entrance_paint(uint8 direction, int height, rct_map_element* map_element){
	RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8_t) = VIEWPORT_INTERACTION_ITEM_LABEL;

	rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);

	if (gCurrentViewportFlags & VIEWPORT_FLAG_PATH_HEIGHTS &&
		dpi->zoom_level == 0){
		uint32 ebx =
			(map_element->properties.entrance.type << 4) |
			(map_element->properties.entrance.index & 0xF);

		if (RCT2_ADDRESS(0x0097B974, uint8)[ebx] & 0xF){

			int z = map_element->base_height * 8 + 3;
			uint32 image_id =
				z / 16 +
				RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS,sint16) +
				0x20101689;

			image_id -= RCT2_GLOBAL(0x01359208, sint16);

			sub_98197C(image_id, 16, 16, 1, 1, 0, height, 31, 31, z + 64, get_current_rotation());
		}
	}

	switch (map_element->properties.entrance.type){
	case ENTRANCE_TYPE_RIDE_ENTRANCE:
	case ENTRANCE_TYPE_RIDE_EXIT:
		ride_entrance_exit_paint(direction, height, map_element);
		break;
	case ENTRANCE_TYPE_PARK_ENTRANCE:
		park_entrance_paint(direction, height, map_element);
		break;
	}
}
