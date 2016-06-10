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

#include "map_element.h"
#include "../../addresses.h"
#include "../../interface/viewport.h"
#include "../../ride/track.h"
#include "../../world/footpath.h"
#include "../../config.h"
#include "../paint.h"
#include "../../world/scenery.h"
#include "surface.h"
#include "../../ride/track_paint.h"
#include "../../localisation/localisation.h"
#include "../../game.h"
#include "../supports.h"

// #3628: Until path_paint is implemented, this variable is used by scrolling_text_setup
//        to use the old string arguments array. Remove when scrolling_text_setup is no
//        longer hooked.
bool TempForScrollText = false;

const uint8 byte_98D800[] = {
	12, 9, 3, 6
};

const uint8 byte_98D6E0[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	0, 1, 2, 20, 4, 5, 6, 22, 8, 9, 10, 26, 12, 13, 14, 36,
	0, 1, 2, 3, 4, 5, 21, 23, 8, 9, 10, 11, 12, 13, 33, 37,
	0, 1, 2, 3, 4, 5, 6, 24, 8, 9, 10, 11, 12, 13, 14, 38,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 29, 30, 34, 39,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 40,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 35, 41,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 42,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 25, 10, 27, 12, 31, 14, 43,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 28, 12, 13, 14, 44,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 45,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 46,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 32, 14, 47,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 48,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 49,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 50
};

const sint16 stru_98D804[][4] = {
	{3, 3, 26, 26},
	{0, 3, 29, 26},
	{3, 3, 26, 29},
	{0, 3, 29, 29},
	{3, 3, 29, 26},
	{0, 3, 32, 26},
	{3, 3, 29, 29},
	{0, 3, 32, 29},
	{3, 0, 26, 29},
	{0, 0, 29, 29},
	{3, 0, 26, 32},
	{0, 0, 29, 32},
	{3, 0, 29, 29},
	{0, 0, 32, 29},
	{3, 0, 29, 32},
	{0, 0, 32, 32},
};

const uint8 byte_98D8A4[] = {
	0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0
};

typedef struct
{
	uint8 var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7;
} unk_supports_desc_bound_box;

const unk_supports_desc_bound_box stru_98D8D4[] = {
	{0, 0, 0, 1, 1, 4, 0, 1},
	{0, 0, 0, 1, 1, 4, 0, 1},
	{0, 0, 0, 1, 1, 4, 0, 1},
	{0, 0, 0, 1, 1, 4, 0, 1}
};


void loc_6A37C9(rct_map_element * map_element, int height, rct_footpath_entry * dword_F3EF6C, bool word_F3F038, uint32 dword_F3EF70, uint32 dword_F3EF74);

void loc_6A3B57(rct_map_element* mapElement, sint16 height, rct_footpath_entry* footpathEntry, bool hasFences, uint32 imageFlags, uint32 sceneryImageFlags);

/* rct2: 0x006A5AE5 */
void path_bit_lights_paint(rct_scenery_entry* pathBitEntry, rct_map_element* mapElement, int height, uint8 edges, uint32 pathBitImageFlags) {
	if (footpath_element_is_sloped(mapElement))
		height += 8;

	uint32 imageId;

	if (!(edges & (1 << 0))) {
		imageId = pathBitEntry->image + 1;

		if (mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)
			imageId += 4;

		imageId |= pathBitImageFlags;

		sub_98197C(imageId, 2, 16, 1, 1, 23, height, 3, 16, height + 2, get_current_rotation());
	}
	if (!(edges & (1 << 1))) {
		imageId = pathBitEntry->image + 2;

		if (mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)
			imageId += 4;

		imageId |= pathBitImageFlags;

		sub_98197C(imageId, 16, 30, 1, 0, 23, height, 16, 29, height + 2, get_current_rotation());
	}

	if (!(edges & (1 << 2))) {
		imageId = pathBitEntry->image + 3;

		if (mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)
			imageId += 4;

		imageId |= pathBitImageFlags;

		sub_98197C(imageId, 30, 16, 0, 1, 23, height, 29, 16, height + 2, get_current_rotation());
	}

	if (!(edges & (1 << 3))) {
		imageId = pathBitEntry->image + 4;

		if (mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)
			imageId += 4;

		imageId |= pathBitImageFlags;

		sub_98197C(imageId, 16, 2, 1, 1, 23, height, 16, 3, height + 2, get_current_rotation());
	}
}

/* rct2: 0x006A5C94 */
void path_bit_bins_paint(rct_scenery_entry* pathBitEntry, rct_map_element* mapElement, int height, uint8 edges, uint32 pathBitImageFlags) {
	if (footpath_element_is_sloped(mapElement))
		height += 8;

	uint32 imageId;

	if (!(edges & (1 << 0))) {
		imageId = pathBitEntry->image + 5;

		imageId |= pathBitImageFlags;

		if (!(mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)) {
			imageId -= 4;
			
			if (!(mapElement->properties.path.addition_status & (0x3 << get_current_rotation())))
				imageId += 8;
		}
		

		sub_98197C(imageId, 7, 16, 1, 1, 7, height, 7, 16, height + 2, get_current_rotation());
	}
	if (!(edges & (1 << 1))) {
		imageId = pathBitEntry->image + 6;

		imageId |= pathBitImageFlags;

		if (!(mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)) {
			imageId -= 4;

			if (!(mapElement->properties.path.addition_status & rol8(0xC, get_current_rotation())))
				imageId += 8;
		}
		

		sub_98197C(imageId, 16, 25, 1, 1, 7, height, 16, 25, height + 2, get_current_rotation());
	}

	if (!(edges & (1 << 2))) {
		imageId = pathBitEntry->image + 7;

		imageId |= pathBitImageFlags;

		if (!(mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)) {
			imageId -= 4;

			if (!(mapElement->properties.path.addition_status & rol8(0x30, get_current_rotation())))
				imageId += 8;
		}
		

		sub_98197C(imageId, 25, 16, 1, 1, 7, height, 25, 16, height + 2, get_current_rotation());
	}

	if (!(edges & (1 << 3))) {
		imageId = pathBitEntry->image + 8;

		imageId |= pathBitImageFlags;

		if (!(mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)) {
			imageId -= 4;

			if (!(mapElement->properties.path.addition_status & rol8(0xC0, get_current_rotation())))
				imageId += 8;
		}
		

		sub_98197C(imageId, 16, 7, 1, 1, 7, height, 16, 7, height + 2, get_current_rotation());
	}
}

/* rct2: 0x006A5E81 */
void path_bit_benches_paint(rct_scenery_entry* pathBitEntry, rct_map_element* mapElement, int height, uint8 edges, uint32 pathBitImageFlags) {
	uint32 imageId;

	if (!(edges & (1 << 0))) {
		imageId = pathBitEntry->image + 1;

		if (mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)
			imageId += 4;

		imageId |= pathBitImageFlags;

		sub_98197C(imageId, 7, 16, 0, 16, 7, height, 6, 8, height + 2, get_current_rotation());
	}
	if (!(edges & (1 << 1))) {
		imageId = pathBitEntry->image + 2;

		if (mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)
			imageId += 4;

		imageId |= pathBitImageFlags;

		sub_98197C(imageId, 16, 25, 16, 0, 7, height, 8, 23, height + 2, get_current_rotation());
	}

	if (!(edges & (1 << 2))) {
		imageId = pathBitEntry->image + 3;

		if (mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)
			imageId += 4;

		imageId |= pathBitImageFlags;

		sub_98197C(imageId, 25, 16, 0, 16, 7, height, 23, 8, height + 2, get_current_rotation());
	}

	if (!(edges & (1 << 3))) {
		imageId = pathBitEntry->image + 4;

		if (mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)
			imageId += 4;

		imageId |= pathBitImageFlags;

		sub_98197C(imageId, 16, 7, 16, 0, 7, height, 8, 6, height + 2, get_current_rotation());
	}
}

/* rct2: 0x006A6008 */
void path_bit_jumping_fountains_paint(rct_scenery_entry* pathBitEntry, rct_map_element* mapElement, int height, uint8 edges, uint32 pathBitImageFlags, rct_drawpixelinfo* dpi) {
	if (dpi->zoom_level != 0)
		return;

	uint32 imageId = pathBitEntry->image;
	imageId |= pathBitImageFlags;

	sub_98197C(imageId + 1, 0, 0, 1, 1, 2, height, 3, 3, height + 2, get_current_rotation());
	sub_98197C(imageId + 2, 0, 0, 1, 1, 2, height, 3, 29, height + 2, get_current_rotation());
	sub_98197C(imageId + 3, 0, 0, 1, 1, 2, height, 29, 29, height + 2, get_current_rotation());
	sub_98197C(imageId + 4, 0, 0, 1, 1, 2, height, 29, 3, height + 2, get_current_rotation());
}

bool do_sub_6A2ECC(int supportType, int special, int height, uint32 imageColourFlags, rct_footpath_entry * dword_F3EF6C, bool * underground)
{
	if (underground != NULL) {
		*underground = false;
	}

	int eax = special, edx = height, _edi = supportType, ebp = imageColourFlags;

	if (gCurrentViewportFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS) {
		return false;
	}

	if (!(RCT2_GLOBAL(0x0141E9DB, uint8) & 1)) {
		return false;
	}

	int z = floor2(RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) + 15, 16);
	height -= z;
	if (height < 0) {
		if (underground != NULL) {
			*underground = true;
		}
		return false;
	}
	height /= 16;

	bool hasSupports = false;

	// Draw base support (usually shaped to the slope)
	int slope = RCT2_GLOBAL(0x0141E9DA, uint8);
	if (slope & 0x20) {
		uint32 image_id = (dword_F3EF6C->bridge_image + 48) | imageColourFlags;
		sub_98196C(image_id, 0, 0, 32, 32, 0, z - 2, get_current_rotation());
		hasSupports = true;
	} else {

	}

	while (height != 0) {
		if ((z & 16) == 0 && height >= 2 && z + 16 != RCT2_GLOBAL(0x00141E9DC, uint16)) {
			// Full support
			int imageId = (dword_F3EF6C->bridge_image + 22 + supportType * 24) | imageColourFlags;
			uint8 ah = height == 2 ? 23 : 28; // weird
			sub_98196C(imageId, 0, 0, 32, 32, ah, z, get_current_rotation());
			hasSupports = true;
			z += 32;
			height -= 2;
		} else {
			// Half support
			int imageId = (dword_F3EF6C->bridge_image + 23 + supportType * 24) | imageColourFlags;
			uint8 ah = special == 1 ? 7 : 12;
			sub_98196C(imageId, 0, 0, 32, 32, ah, z, get_current_rotation());
			hasSupports = true;
			z += 16;
			height -= 1;
		}
	}

	if (special == 0) {
		return hasSupports;
	}

	special = (special - 1) & 0xFFFF;

	uint32 imageId = (dword_F3EF6C->bridge_image + 55 + special) | imageColourFlags;


	unk_supports_desc_bound_box stru = stru_98D8D4[special];
	// var_6 is always zero.

	sub_98197C(imageId, 0, 0, stru.var_3, stru.var_4, stru.var_5, z, stru.var_0, stru.var_1, z + stru.var_3, get_current_rotation());
	hasSupports = true;

	return hasSupports;
}

/**
 * rct2: 0x006A4101
 * @param map_element (esi)
 * @param (ecx)
 * @param ebp (ebp)
 * @param base_image_id (0x00F3EF78)
 */
void sub_6A4101(rct_map_element * map_element, uint16 height, uint32 ebp, bool word_F3F038, rct_footpath_entry * dword_F3EF6C, uint32 base_image_id, uint32 dword_F3EF70)
{
	if (map_element->type & 1) {
		uint8 local_ebp = ebp & 0x0F;
		if (map_element->properties.path.type & 0x04) {
			switch ((map_element->properties.path.type + get_current_rotation()) & 0x03) {
				case 0:
					sub_98197C(95 + base_image_id, 0, 4, 32, 1, 23, height, 0, 4, height + 2, get_current_rotation());
					sub_98197C(95 + base_image_id, 0, 28, 32, 1, 23, height, 0, 28, height + 2, get_current_rotation());
					break;
				case 1:
					sub_98197C(94 + base_image_id, 4, 0, 1, 32, 23, height, 4, 0, height + 2, get_current_rotation());
					sub_98197C(94 + base_image_id, 28, 0, 1, 32, 23, height, 28, 0, height + 2, get_current_rotation());
					break;
				case 2:
					sub_98197C(96 + base_image_id, 0, 4, 32, 1, 23, height, 0, 4, height + 2, get_current_rotation());
					sub_98197C(96 + base_image_id, 0, 28, 32, 1, 23, height, 0, 28, height + 2, get_current_rotation());
					break;
				case 3:
					sub_98197C(93 + base_image_id, 4, 0, 1, 32, 23, height, 4, 0, height + 2, get_current_rotation());
					sub_98197C(93 + base_image_id, 28, 0, 1, 32, 23, height, 28, 0, height + 2, get_current_rotation());
					break;
			}
		} else {
			switch (local_ebp) {
				case 1:
					sub_98197C(90 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2, get_current_rotation());
					sub_98197C(90 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2, get_current_rotation());
					break;
				case 2:
					sub_98197C(91 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2, get_current_rotation());
					sub_98197C(91 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2, get_current_rotation());
					break;
				case 3:
					sub_98197C(90 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2, get_current_rotation());
					sub_98197C(91 + base_image_id, 28, 0, 1, 28, 7, height, 28, 4, height + 2, get_current_rotation()); // bound_box_offset_y seems to be a bug
					sub_98197C(98 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2, get_current_rotation());
					break;
				case 4:
					sub_98197C(92 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2, get_current_rotation());
					sub_98197C(92 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2, get_current_rotation());
					break;
				case 5:
					sub_98197C(88 + base_image_id, 0, 4, 32, 1, 7, height, 0, 4, height + 2, get_current_rotation());
					sub_98197C(88 + base_image_id, 0, 28, 32, 1, 7, height, 0, 28, height + 2, get_current_rotation());
					break;
				case 6:
					sub_98197C(91 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2, get_current_rotation());
					sub_98197C(92 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2, get_current_rotation());
					sub_98197C(99 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2, get_current_rotation());
					break;
				case 8:
					sub_98197C(89 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2, get_current_rotation());
					sub_98197C(89 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2, get_current_rotation());
					break;
				case 9:
					sub_98197C(89 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2, get_current_rotation());
					sub_98197C(90 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2, get_current_rotation());
					sub_98197C(97 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2, get_current_rotation());
					break;
				case 10:
					sub_98197C(87 + base_image_id, 4, 0, 1, 32, 7, height, 4, 0, height + 2, get_current_rotation());
					sub_98197C(87 + base_image_id, 28, 0, 1, 32, 7, height, 28, 0, height + 2, get_current_rotation());
					break;
				case 12:
					sub_98197C(89 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2, get_current_rotation());
					sub_98197C(92 + base_image_id, 0, 28, 28, 1, 7, height, 4, 28, height + 2, get_current_rotation()); // bound_box_offset_x seems to be a bug
					sub_98197C(100 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2, get_current_rotation());
					break;
				default:
					// purposely left empty
					break;
			}
		}

		if (!(map_element->properties.path.type & 0x08)) {
			return;
		}

		uint8 direction = ((map_element->type & 0xC0) >> 6);
		// Draw ride sign
		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_RIDE;
		if (footpath_element_is_sloped(map_element)) {
			if (footpath_element_get_slope_direction(map_element) == direction)
				height += 16;
		}
		direction += get_current_rotation();
		direction &= 3;

		rct_xyz16 boundBoxOffsets = {
			.x = RCT2_ADDRESS(0x0098D884, sint16)[direction * 4],
			.y = RCT2_ADDRESS(0x0098D884 + 2, sint16)[direction * 4],
			.z = height + 2
		};

		uint32 imageId = (direction << 1) + base_image_id + 101;
		
		sub_98197C(imageId, 0, 0, 1, 1, 21, height, boundBoxOffsets.x, boundBoxOffsets.y, boundBoxOffsets.z, get_current_rotation());

		boundBoxOffsets.x = RCT2_ADDRESS(0x98D888, sint16)[direction * 4];
		boundBoxOffsets.y = RCT2_ADDRESS(0x98D888 + 2, sint16)[direction * 4];
		imageId++;
		sub_98197C(imageId, 0, 0, 1, 1, 21, height, boundBoxOffsets.x, boundBoxOffsets.y, boundBoxOffsets.z, get_current_rotation());

		direction--;
		// If text shown
		if (direction < 2 && map_element->properties.path.ride_index != 255 && dword_F3EF70 == 0) {
			uint16 scrollingMode = dword_F3EF6C->scrolling_mode;
			scrollingMode += direction;

			set_format_arg(0, uint32, 0);
			set_format_arg(4, uint32, 0);

			rct_ride* ride = get_ride(map_element->properties.path.ride_index);
			rct_string_id string_id = STR_RIDE_ENTRANCE_CLOSED;
			if (ride->status == RIDE_STATUS_OPEN && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)){
				set_format_arg(0, uint16, ride->name);
				set_format_arg(2, uint32, ride->name_arguments);
				string_id = STR_RIDE_ENTRANCE_NAME;
			}
			if (gConfigGeneral.upper_case_banners) {
				format_string_to_upper(RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char), string_id, gCommonFormatArgs);
			} else {
				format_string(RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char), string_id, gCommonFormatArgs);
			}

			gCurrentFontSpriteBase = FONT_SPRITE_BASE_TINY;

			uint16 string_width = gfx_get_string_width(RCT2_ADDRESS(RCT2_ADDRESS_COMMON_STRING_FORMAT_BUFFER, char));
			uint16 scroll = (gCurrentTicks / 2) % string_width;

			sub_98199C(scrolling_text_setup(string_id, scroll, scrollingMode), 0, 0, 1, 1, 21, height + 7,  boundBoxOffsets.x,  boundBoxOffsets.y,  boundBoxOffsets.z, get_current_rotation());
		}

		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_FOOTPATH;
		if (dword_F3EF70 != 0) {
			RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_NONE;
		}
		return;
	}


	// save ecx, ebp, esi
	rct_footpath_entry * ebx = dword_F3EF6C;
	uint32 dword_F3EF80 = ebp;
	if (!(ebx->flags & 2)) {
		dword_F3EF80 &= 0x0F;
	}

	if (map_element->properties.path.type & 0x04) {
		switch ((map_element->properties.path.type + get_current_rotation()) & 0x03) {
			case 0:
				sub_98197C(81 + base_image_id, 0, 4, 32, 1, 23, height, 0, 4, height + 2, get_current_rotation());
				sub_98197C(81 + base_image_id, 0, 28, 32, 1, 23, height, 0, 28, height + 2, get_current_rotation());
				break;
			case 1:
				sub_98197C(80 + base_image_id, 4, 0, 1, 32, 23, height, 4, 0, height + 2, get_current_rotation());
				sub_98197C(80 + base_image_id, 28, 0, 1, 32, 23, height, 28, 0, height + 2, get_current_rotation());
				break;
			case 2:
				sub_98197C(82 + base_image_id, 0, 4, 32, 1, 23, height, 0, 4, height + 2, get_current_rotation());
				sub_98197C(82 + base_image_id, 0, 28, 32, 1, 23, height, 0, 28, height + 2, get_current_rotation());
				break;
			case 3:
				sub_98197C(79 + base_image_id, 4, 0, 1, 32, 23, height, 4, 0, height + 2, get_current_rotation());
				sub_98197C(79 + base_image_id, 28, 0, 1, 32, 23, height, 28, 0, height + 2, get_current_rotation());
				break;
		}
	} else {
		if (!word_F3F038) {
			return;
		}

		uint8 local_ebp = ebp & 0x0F;
		switch (local_ebp) {
			case 0:
				// purposely left empty
				break;
			case 1:
				sub_98197C(76 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2, get_current_rotation());
				sub_98197C(76 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2, get_current_rotation());
				break;
			case 2:
				sub_98197C(77 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2, get_current_rotation());
				sub_98197C(77 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2, get_current_rotation());
				break;
			case 4:
				sub_98197C(78 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2, get_current_rotation());
				sub_98197C(78 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2, get_current_rotation());
				break;
			case 5:
				sub_98197C(74 + base_image_id, 0, 4, 32, 1, 7, height, 0, 4, height + 2, get_current_rotation());
				sub_98197C(74 + base_image_id, 0, 28, 32, 1, 7, height, 0, 28, height + 2, get_current_rotation());
				break;
			case 8:
				sub_98197C(75 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2, get_current_rotation());
				sub_98197C(75 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2, get_current_rotation());
				break;
			case 10:
				sub_98197C(73 + base_image_id, 4, 0, 1, 32, 7, height, 4, 0, height + 2, get_current_rotation());
				sub_98197C(73 + base_image_id, 28, 0, 1, 32, 7, height, 28, 0, height + 2, get_current_rotation());
				break;

			case 3:
				sub_98197C(76 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2, get_current_rotation());
				sub_98197C(77 + base_image_id, 28, 0, 1, 28, 7, height, 28, 4, height + 2, get_current_rotation()); // bound_box_offset_y seems to be a bug
				if (!(dword_F3EF80 & 0x10)) {
					sub_98197C(84 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2, get_current_rotation());
				}
				break;
			case 6:
				sub_98197C(77 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2, get_current_rotation());
				sub_98197C(78 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2, get_current_rotation());
				if (!(dword_F3EF80 & 0x20)) {
					sub_98197C(85 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2, get_current_rotation());
				}
				break;
			case 9:
				sub_98197C(75 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2, get_current_rotation());
				sub_98197C(76 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2, get_current_rotation());
				if (!(dword_F3EF80 & 0x80)) {
					sub_98197C(83 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2, get_current_rotation());
				}
				break;
			case 12:
				sub_98197C(75 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2, get_current_rotation());
				sub_98197C(78 + base_image_id, 0, 28, 28, 1, 7, height, 4, 28, height + 2, get_current_rotation()); // bound_box_offset_x seems to be a bug
				if (!(dword_F3EF80 & 0x40)) {
					sub_98197C(86 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2, get_current_rotation());
				}
				break;

			case 7:
				sub_98197C(74 + base_image_id, 0, 4, 32, 1, 7, height, 0, 4, height + 2, get_current_rotation());
				if (!(dword_F3EF80 & 0x10)) {
					sub_98197C(84 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2, get_current_rotation());
				}
				if (!(dword_F3EF80 & 0x20)) {
					sub_98197C(85 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2, get_current_rotation());
				}
				break;
			case 13:
				sub_98197C(74 + base_image_id, 0, 28, 32, 1, 7, height, 0, 28, height + 2, get_current_rotation());
				if (!(dword_F3EF80 & 0x40)) {
					sub_98197C(86 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2, get_current_rotation());
				}
				if (!(dword_F3EF80 & 0x80)) {
					sub_98197C(83 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2, get_current_rotation());
				}
				break;
			case 14:
				sub_98197C(73 + base_image_id, 4, 0, 1, 32, 7, height, 4, 0, height + 2, get_current_rotation());
				if (!(dword_F3EF80 & 0x20)) {
					sub_98197C(85 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2, get_current_rotation());
				}
				if (!(dword_F3EF80 & 0x40)) {
					sub_98197C(86 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2, get_current_rotation());
				}
				break;
			case 11:
				sub_98197C(73 + base_image_id, 28, 0, 1, 32, 7, height, 28, 0, height + 2, get_current_rotation());
				if (!(dword_F3EF80 & 0x10)) {
					sub_98197C(84 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2, get_current_rotation());
				}
				if (!(dword_F3EF80 & 0x80)) {
					sub_98197C(83 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2, get_current_rotation());
				}
				break;

			case 15:
				if (!(dword_F3EF80 & 0x10)) {
					sub_98197C(84 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2, get_current_rotation());
				}
				if (!(dword_F3EF80 & 0x20)) {
					sub_98197C(85 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2, get_current_rotation());
				}
				if (!(dword_F3EF80 & 0x40)) {
					sub_98197C(86 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2, get_current_rotation());
				}
				if (!(dword_F3EF80 & 0x80)) {
					sub_98197C(83 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2, get_current_rotation());
				}
				break;


		}
	}

}

/**
 * rct2: 0x006A3F61
 * @param map_element (esp[0])
 * @param bp (bp)
 * @param height (dx)
 * @param dword_F3EF6C (0x00F3EF6C)
 * @param dword_F3EF70 (0x00F3EF70)
 * @param dword_F3EF74 (0x00F3EF74)
 */
void sub_6A3F61(rct_map_element * map_element, uint16 bp, uint16 height, rct_footpath_entry * dword_F3EF6C, uint32 dword_F3EF70, uint32 dword_F3EF74, bool word_F3F038)
{
	// eax --
	// ebx --
	// ecx
	// edx
	// esi --
	// edi --
	// ebp
	// esp: [ esi, ???, 000]

	// Probably drawing benches etc.

	rct_drawpixelinfo * dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);

	if (dpi->zoom_level <= 1) {
		if (!(RCT2_GLOBAL(0x9DEA6F, uint8) & 1)) {
			uint8 additions = map_element->properties.path.additions & 0xF;
			if (additions != 0) {
				RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM;
				if (dword_F3EF74 != 0) {
					RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_NONE;
				}

				// Draw additional path bits (bins, benchs, lamps, queue screens)
				rct_scenery_entry* sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(map_element));
				switch (sceneryEntry->path_bit.draw_type) {
				case PATH_BIT_DRAW_TYPE_LIGHTS:
					path_bit_lights_paint(sceneryEntry, map_element, height, (uint8)bp, dword_F3EF74);
					break;
				case PATH_BIT_DRAW_TYPE_BINS:
					path_bit_bins_paint(sceneryEntry, map_element, height, (uint8)bp, dword_F3EF74);
					break;
				case PATH_BIT_DRAW_TYPE_BENCHES:
					path_bit_benches_paint(sceneryEntry, map_element, height, (uint8)bp, dword_F3EF74);
					break;
				case PATH_BIT_DRAW_TYPE_JUMPING_FOUNTAINS:
					path_bit_jumping_fountains_paint(sceneryEntry, map_element, height, (uint8)bp, dword_F3EF74, dpi);
					break;
				}
				
				RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_FOOTPATH;

				if (dword_F3EF74 != 0) {
					RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_NONE;
				}
			}
		}

		// Redundant zoom-level check removed
		RCT2_GLOBAL(0xF3EF78, uint32) = dword_F3EF6C->image | dword_F3EF70;
		//RCT2_CALLPROC_X(0x6A4101, 0, 0, 0, 0, (int) map_element, 0, 0);
		sub_6A4101(map_element, height, bp, word_F3F038, dword_F3EF6C, dword_F3EF6C->image | dword_F3EF70, dword_F3EF70);
	}

	// This is about tunnel drawing
	uint8 direction = (map_element->properties.path.type + get_current_rotation()) & 0x03;
	uint8 diagonal = map_element->properties.path.type & 0x04;
	uint8 bl = direction | diagonal;

	if (bp & 2) {
		// Bottom right of tile is a tunnel
		registers regs = {0};
		regs.eax = 0xFFFFFFFF;
		if (bl == 5) {
			regs.ax = (height + 16) / 16;
			regs.ah = 0x0A;
		} else if (bp & 1) {
			regs.ax = height / 16;
			regs.ah = 0x0B;
		} else {
			regs.ax = height / 16;
			regs.ah = 0x0A;

		}
		RCT2_ADDRESS(0x009E30B6, uint32)[RCT2_GLOBAL(0x141F56B, uint8) / 2] = regs.eax;
		RCT2_GLOBAL(0x141F56B, uint8)++;
	}

	if (!(bp & 4)) {
		return;
	}

	// Bottom left of the tile is a tunnel
	registers regs = {0};
	regs.eax = 0xFFFFFFFF;
	if (bl == 6) {
		regs.ax = (height + 16) / 16;
		regs.ah = 0x0A;
	} else if (bp & 8) {
		regs.ax = height / 16;
		regs.ah = 0x0B;
	} else {
		regs.ax = height / 16;
		regs.ah = 0x0A;

	}
	RCT2_ADDRESS(0x009E3138, uint32)[RCT2_GLOBAL(0x141F56A, uint8) / 2] = regs.eax;
	RCT2_GLOBAL(0x141F56A, uint8)++;
}

/**
 * rct2: 0x0006A3590
 */
void path_paint(uint8 direction, uint16 height, rct_map_element * map_element)
{
	if (gUseOriginalRidePaint) {
		TempForScrollText = true;
		RCT2_CALLPROC_X(0x6A3590, 0, 0, direction, height, (int) map_element, 0, 0);
		TempForScrollText = false;
		return;
	}


	RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_FOOTPATH;

	uint32 ecx = get_current_rotation();
	bool word_F3F038 = false;

	uint32 dword_F3EF74 = 0;
	uint32 dword_F3EF70 = 0;

	if (RCT2_GLOBAL(0x9DEA6F, uint8) & 1) {
		if (map_element->type & 1) {
			if (map_element->properties.path.ride_index != RCT2_GLOBAL(0x00F64DE8, uint8)) {
				return;
			}
		}

		if (!track_design_save_contains_map_element(map_element)) {
			dword_F3EF70 = 0x21700000;
		}
	}

	if (footpath_element_path_scenery_is_ghost(map_element)) {
		dword_F3EF74 = RCT2_ADDRESS(0x993CC4, uint32_t)[gConfigGeneral.construction_marker_colour];
	}

	if (map_element->flags & MAP_ELEMENT_FLAG_GHOST) {
		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_NONE;
		dword_F3EF70 = RCT2_ADDRESS(0x993CC4, uint32_t)[gConfigGeneral.construction_marker_colour];
	}

	sint16 x = RCT2_GLOBAL(0x009DE56A, sint16), y = RCT2_GLOBAL(0x009DE56E, sint16);

	rct_map_element * surface = map_get_surface_element_at(x / 32, y / 32);

	uint16 bl = height / 8;
	if (surface == NULL) {
		word_F3F038 = true;
	} else if (surface->base_height != bl) {
		word_F3F038 = true;
	} else {
		if (map_element->properties.path.type & 0x04) {
			// Diagonal path

			if ((surface->properties.surface.slope & 0x1F) != byte_98D800[map_element->properties.path.type & 0x03]) {
				word_F3F038 = true;
			}
		} else {
			if (surface->properties.surface.slope & 0x1F) {
				word_F3F038 = true;
			}
		}
	}

	if (RCT2_GLOBAL(0x009DEA50, sint16) != -1) {
		sint32 staffIndex = RCT2_GLOBAL(0x009DEA50, sint16);
		uint8 staffType = staffIndex & 0x7FFF;
		sint16 x = RCT2_GLOBAL(0x009DE56A, sint16), y = RCT2_GLOBAL(0x009DE56E, sint16);

		uint8 patrolColour = COLOUR_LIGHT_BLUE;

		x = (x & 0x1F80) >> 7;
		y = (y & 0x1F80) >> 1;
		int offset = (x | y) >> 5;
		int bitIndex = (x | y) & 0x1F;

		bool do_it = false;

		if (staffIndex >= 0) {
			rct_peep * staff = GET_PEEP(staffIndex);
			if (RCT2_ADDRESS(RCT2_ADDRESS_STAFF_PATROL_AREAS + (staff->staff_id * 512), uint32)[offset] & (1 << bitIndex)) {
				do_it = true;
			} else {
				patrolColour = COLOUR_GREY;
				staffType = (staff->staff_type + 200) * 512;
				if (RCT2_ADDRESS(RCT2_ADDRESS_STAFF_PATROL_AREAS + staffType, uint32)[offset] & (1 << bitIndex)) {
					do_it = true;
				}
			}
		} else {
			staffType = (staffType + 200) * 512;
			if (RCT2_ADDRESS(RCT2_ADDRESS_STAFF_PATROL_AREAS + staffType, uint32)[offset] & (1 << bitIndex)) {
				do_it = true;
			}
		}

		if (do_it) {
			uint32 imageId = 2618;
			int height = map_element->base_height * 8;
			if (map_element->properties.path.type & 0x04) {
				imageId = 2619 + ((map_element->properties.path.type + get_current_rotation()) & 3);
				height += 16;
			}

			sub_98196C(imageId | patrolColour << 19 | 0x20000000, 16, 16, 1, 1, 0, height + 2, get_current_rotation());
		}
	}


	if (gCurrentViewportFlags & VIEWPORT_FLAG_PATH_HEIGHTS) {
		uint16 height = 3 + map_element->base_height * 8;
		if (map_element->properties.path.type & 0x04) {
			height += 8;
		}
		uint32 imageId = (SPR_HEIGHT_MARKER_BASE + height / 16) | COLOUR_GREY << 19 | 0x20000000;
		imageId += RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_HEIGHT_MARKERS, uint16);
		imageId -= RCT2_GLOBAL(0x01359208, uint16);
		sub_98196C(imageId, 16, 16, 1, 1, 0, height, get_current_rotation());
	}

	uint8 pathType = (map_element->properties.path.type & 0xF0) >> 4;
	rct_footpath_entry * dword_F3EF6C = gFootpathEntries[pathType];

	if (dword_F3EF6C->var_0A == 0) {
		loc_6A37C9(map_element, height, dword_F3EF6C, word_F3F038, dword_F3EF70, dword_F3EF74);
	} else {
		loc_6A3B57(map_element, height, dword_F3EF6C, word_F3F038, dword_F3EF70, dword_F3EF74);
	}
}

void loc_6A37C9(rct_map_element * map_element, int height, rct_footpath_entry * dword_F3EF6C, bool word_F3F038, uint32 dword_F3EF70, uint32 dword_F3EF74)
{
	// esi: mapElement
	// ecx: get_current_rotation();
	registers regs = {0};
	regs.al = map_element->properties.path.edges;
	regs.ah = regs.al << 4;

	uint8 edi_index = rol16(regs.ax, get_current_rotation()) & 0xF;
	// stru_98D804[edi_index];
//    mov     eax, dword ptr ds:stru_98D804.anonymous_0[edi*8]
//    mov     ebx, dword ptr ds:stru_98D804.anonymous_1[edi*8]
//    mov     dword ptr _boundbox_offset_x, eax
//    mov     dword ptr word_F3EFC8, ebx
	rct_xy16 boundBoxOffset = {.x =stru_98D804[edi_index][0], .y = stru_98D804[edi_index][1]};
	rct_xy16 boundBoxSize = {.x =stru_98D804[edi_index][2], .y = stru_98D804[edi_index][3]};

	regs.al = map_element->properties.path.edges;
	regs.ah = regs.al >> 4;
	regs.ax = (rol16(regs.ax, get_current_rotation()) >> 4) & 0xF0;

	uint16 edi = edi_index | regs.ax;

	uint32 ebx;
	if (map_element->properties.path.type & 0x04) {
		ebx = ((map_element->properties.path.type + get_current_rotation()) & 3) + 16;
	} else {
		ebx = byte_98D6E0[edi];
	}

	// save edi

	ebx += dword_F3EF6C->image;
	if (map_element_get_direction(map_element) & 1) {
		ebx += 51;
	}

	if (!RCT2_GLOBAL(0x9DE57C, bool)) {
		boundBoxOffset.x = 3;
		boundBoxOffset.y = 3;
		boundBoxSize.x = 26;
		boundBoxSize.y = 26;
	}

	if (!word_F3F038 || !RCT2_GLOBAL(0x9DE57C, bool)) {
		sub_98197C(ebx | dword_F3EF70, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y, height + 1, get_current_rotation());
	} else {
		uint32 image_id;
		if (map_element->properties.path.type & 0x04) {
			image_id = ((map_element->properties.path.type + get_current_rotation()) & 3) + dword_F3EF6C->bridge_image + 51;
		} else {
			image_id = byte_98D8A4[edi_index] + dword_F3EF6C->bridge_image + 49;
		}

		sub_98197C(image_id | dword_F3EF70, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y, height + 1, get_current_rotation());

		if (!(map_element->type & 1) && !(dword_F3EF6C->flags & 2)) {
			// don't draw
		} else {
			sub_98199C(ebx | dword_F3EF70, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y, height + 1, get_current_rotation());
		}
	}


	sub_6A3F61(map_element, edi, height, dword_F3EF6C, dword_F3EF70, dword_F3EF74, word_F3F038); // TODO: arguments

	uint16 ax = 0;
	if (map_element->properties.path.type & 0x04) {
		ax = ((map_element->properties.path.type + get_current_rotation()) & 0x3) + 1;
	}

	uint32 ebp = edi;
	ebx = ebp & 0xF;

	if (byte_98D8A4[ebx] == 0) {
		do_sub_6A2ECC(0, ax, height, dword_F3EF70, dword_F3EF6C, NULL);
	} else {
		do_sub_6A2ECC(1, ax, height, dword_F3EF70, dword_F3EF6C, NULL);
	}


	sint16 x = RCT2_GLOBAL(0x009DE56A, sint16), y = RCT2_GLOBAL(0x009DE56E, sint16);

	height += 32;
	if (map_element->properties.path.type & 0x04) {
		height += 16;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x141E9DA, uint8) = 0x20;
	}

	if (map_element->type & 1
	    || (map_element->properties.path.edges != 0xFF && word_F3F038)
		) {
		RCT2_GLOBAL(0x141E9C4, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9D4, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9B4, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9C0, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9B8, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9BC, uint16) = 0xFFFF;
		return;
	}

	if (map_element->properties.path.edges == 0xFF) {
		RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9D4, uint16) = 0xFFFF;
		return;
	}

	RCT2_GLOBAL(0x141E9C4, uint16) = 0xFFFF;

	// no idea whre bp comes from
	uint16 bp = edi;

	if (bp & 1) {
		RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;
	}

	if (bp & 2) {
		RCT2_GLOBAL(0x141E9D4, uint16) = 0xFFFF;
	}

	if (bp & 4) {
		RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
	}

	if (bp & 8) {
		RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
	}

}

void loc_6A3B57(rct_map_element* mapElement, sint16 height, rct_footpath_entry* footpathEntry, bool hasFences, uint32 imageFlags, uint32 sceneryImageFlags)
{
	uint8 type = footpath_element_get_type(mapElement);
	// Rol edges around rotation
	uint8 edges = ((mapElement->properties.path.edges << get_current_rotation()) & 0xF) |
		(((mapElement->properties.path.edges & 0xF) << get_current_rotation()) >> 4);

	rct_xy16 boundBoxOffset = {
		.x = stru_98D804[edges][0],
		.y = stru_98D804[edges][1]
	};

	rct_xy16 boundBoxSize = {
		.x = stru_98D804[edges][2],
		.y = stru_98D804[edges][3]
	};

	uint8 corners = (((mapElement->properties.path.edges >> 4) << get_current_rotation()) & 0xF) |
		(((mapElement->properties.path.edges >> 4) << get_current_rotation()) >> 4);

	uint16 edi = edges | (corners << 4);

	uint32 imageId;
	if (footpath_element_is_sloped(mapElement)) {
		imageId = ((mapElement->properties.path.type + get_current_rotation()) & 3) + 16;
	}
	else {
		imageId = byte_98D6E0[edi];
	}


	imageId += footpathEntry->image;
	if (mapElement->type & 1) {
		imageId += 51;
	}

	// Below Surface
	if (!RCT2_GLOBAL(0x9DE57C, bool)) {
		boundBoxOffset.x = 3;
		boundBoxOffset.y = 3;
		boundBoxSize.x = 26;
		boundBoxSize.y = 26;
	}

	if (!hasFences || !RCT2_GLOBAL(0x9DE57C, bool)) {
		sub_98197C(imageId | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y, height + 1, get_current_rotation());
	}
	else {
		uint32 bridgeImage;
		if (footpath_element_is_sloped(mapElement)) {
			bridgeImage = ((mapElement->properties.path.type + get_current_rotation()) & 3) + footpathEntry->bridge_image + 16;
		}
		else {
			bridgeImage = edges + footpathEntry->bridge_image;
			bridgeImage |= imageFlags;
		}

		sub_98197C(bridgeImage | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y, height + 1, get_current_rotation());

		if ((mapElement->type & 1) || (footpathEntry->flags & 2)) {
			sub_98199C(imageId | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y, height + 1, get_current_rotation());
		}
	}

	sub_6A3F61(mapElement, edi, height, footpathEntry, imageFlags, sceneryImageFlags, hasFences); // TODO: arguments

	uint16 ax = 0;
	if (footpath_element_is_sloped(mapElement)) {
		ax = 8;
	}

	uint8 supports[] = {
		6,
		8,
		7,
		5
	};

	RCT2_GLOBAL(0x00F3EF6C, rct_footpath_entry*) = footpathEntry;
	for (sint8 i = 3; i > -1; --i) {
		if (!(edges & (1 << i))) {
			path_wooden_a_supports_paint_setup(supports[i], ax, height, imageFlags);
		}
	}
	
	sint16 x = RCT2_GLOBAL(0x009DE56A, sint16), y = RCT2_GLOBAL(0x009DE56E, sint16);

	height += 32;
	if (footpath_element_is_sloped(mapElement)) {
		height += 16;
	}

	paint_util_set_general_support_height(height, 0x20);
	
	if ((mapElement->type & 1)
	    || (mapElement->properties.path.edges != 0xFF && hasFences)
		) {

		paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
		return;
	}

	if (mapElement->properties.path.edges == 0xFF) {
		paint_util_set_segment_support_height(SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, 0xFFFF, 0);
		return;
	}

	paint_util_set_segment_support_height(SEGMENT_C4, 0xFFFF, 0);

	if (edges & 1) {
		paint_util_set_segment_support_height(SEGMENT_CC, 0xFFFF, 0);
	}

	if (edges & 2) {
		paint_util_set_segment_support_height(SEGMENT_D4, 0xFFFF, 0);
	}

	if (edges & 4) {
		paint_util_set_segment_support_height(SEGMENT_D0, 0xFFFF, 0);
	}

	if (edges & 8) {
		paint_util_set_segment_support_height(SEGMENT_C8, 0xFFFF, 0);
	}

}