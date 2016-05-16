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
#include "../../game.h"
#include "../../interface/viewport.h"
#include "../../paint/paint.h"
#include "../../paint/supports.h"
#include "../../world/map.h"
#include "../../world/scenery.h"

void scenery_paint(uint8 direction, int height, rct_map_element* mapElement) {
	//RCT2_CALLPROC_X(0x6DFF47, 0, 0, direction, height, (int)mapElement, 0, 0); return;
	RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8_t) = VIEWPORT_INTERACTION_ITEM_SCENERY;
	RCT2_GLOBAL(0x009DEA52, sint16) = 0;
	RCT2_GLOBAL(0x009DEA54, sint16) = 0;
	int ebx = 0;
	if (RCT2_GLOBAL(0x009DEA6F, uint8) & 1) {
		rct_map_element *mapElement2 = RCT2_GLOBAL(0x00F63674, rct_map_element*);
		while (mapElement2 != mapElement) {
			mapElement2++;
			if ((mapElement2 - 1) != (rct_map_element*)0xFFFFFFFF) {
				continue;
			}
			ebx = 0x21700000;
			break;
		}
	}
	if (mapElement->flags & MAP_ELEMENT_FLAG_GHOST) {
		RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8_t) = VIEWPORT_INTERACTION_ITEM_NONE;
		ebx = RCT2_ADDRESS(0x993CC4, uint32_t)[RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_CONSTRUCTION_MARKER, uint8)];
	}
	uint32 dword_F64EB0 = ebx;
	
	rct_scenery_entry *entry = (rct_scenery_entry*)gSmallSceneryEntries[mapElement->properties.scenery.type];;
	ebx = entry->image + direction;
	RCT2_GLOBAL(0x00F64ED6, uint16) = 2;
	RCT2_GLOBAL(0x00F64ED8, uint16) = 2;
	int ecx;
	sint8 x_offset = 0;
	sint8 y_offset = 0;
	if (entry->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE) {
		if (entry->small_scenery.flags & SMALL_SCENERY_FLAG24) {
			// 6DFFE3:
			RCT2_GLOBAL(0x009DEA52, sint16) = RCT2_ADDRESS(0x009A3E7C, uint16)[direction * 4];
			RCT2_GLOBAL(0x009DEA54, sint16) = RCT2_ADDRESS(0x009A3E7E, uint16)[direction * 4];
			RCT2_GLOBAL(0x00F64ED6, uint16) = RCT2_ADDRESS(0x009A3E8C, uint16)[direction * 4];
			RCT2_GLOBAL(0x00F64ED8, uint16) = RCT2_ADDRESS(0x009A3E8E, uint16)[direction * 4];
			x_offset = 3;
			y_offset = 3;
		} else {
			x_offset = 15;
			y_offset = 15;
			if (entry->small_scenery.flags & SMALL_SCENERY_FLAG_VOFFSET_CENTRE) {
				x_offset = 3;
				y_offset = 3;
				RCT2_GLOBAL(0x00F64ED6, uint16) = 26;
				RCT2_GLOBAL(0x00F64ED8, uint16) = 26;
				if (entry->small_scenery.flags & SMALL_SCENERY_FLAG_ALLOW_WALLS) {
					x_offset = 1;
					y_offset = 1;
					RCT2_GLOBAL(0x00F64ED6, uint16) = 30;
					RCT2_GLOBAL(0x00F64ED8, uint16) = 30;
				}
			}
		}
	} else {
		// 6DFFC2:
		ecx = ((mapElement->type >> 6) + get_current_rotation()) & 3;
		x_offset = RCT2_ADDRESS(0x009A3E74, sint8)[ecx * 2];
		y_offset = RCT2_ADDRESS(0x009A3E75, sint8)[ecx * 2];
	}
	// 6E0074:
	RCT2_GLOBAL(0x009DEA52, sint16) = x_offset;
	RCT2_GLOBAL(0x009DEA54, sint16) = y_offset;
	RCT2_GLOBAL(0x009DEA56, sint16) = height;
	uint8 bblz = entry->small_scenery.height - 4;
	if (bblz > 0x80) {
		bblz = 0x80;
	}
	if (entry->small_scenery.flags & SMALL_SCENERY_FLAG6) {
		if (mapElement->properties.scenery.age >= 40) {
			ebx += 4;
		}
		if (mapElement->properties.scenery.age >= 55) {
			ebx += 4;
		}
	}
	if (entry->small_scenery.flags & SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR) {
		ebx |= ((mapElement->properties.scenery.colour_1 & 0x1F) << 19) | 0x20000000;
		if (entry->small_scenery.flags & SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR) {
			ebx |= ((mapElement->properties.scenery.colour_2 & 0x1F) << 24) | 0x80000000;
		}
	}
	if (dword_F64EB0 != 0) {
		ebx = (ebx & 0x7FFFF) | dword_F64EB0;
	}
	if (!(entry->small_scenery.flags & SMALL_SCENERY_FLAG21)) {
		sub_98197C(ebx, x_offset, y_offset, RCT2_GLOBAL(0x00F64ED6, uint16), RCT2_GLOBAL(0x00F64ED8, uint16), bblz - 1, height, RCT2_GLOBAL(0x009DEA52, sint16), RCT2_GLOBAL(0x009DEA54, sint16), height, get_current_rotation());
	}

	if (entry->small_scenery.flags & SMALL_SCENERY_FLAG10) {
		if (dword_F64EB0 == 0) {
			// Draw transparent overlay:
			int image_id = (ebx & 0x7FFFF) + (((mapElement->properties.scenery.colour_1 & 0x1F) + 112) << 19) + 0x40000004;
			sub_98199C(image_id, x_offset, y_offset, RCT2_GLOBAL(0x00F64ED6, uint16), RCT2_GLOBAL(0x00F64ED8, uint16), bblz - 1, height, RCT2_GLOBAL(0x009DEA52, sint16), RCT2_GLOBAL(0x009DEA54, sint16), height, get_current_rotation());
		}
	}

	if (entry->small_scenery.flags & SMALL_SCENERY_FLAG_ANIMATED) {
		rct_drawpixelinfo* dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);
		if ( !(entry->small_scenery.flags & SMALL_SCENERY_FLAG21) || (dpi->zoom_level <= 1) ) {
			// 6E01A9:
			if (entry->small_scenery.flags & SMALL_SCENERY_FLAG12) {
				// 6E0512:
				int image_id = ((gCurrentTicks / 2) & 0xF) + entry->image + 4;
				if (dword_F64EB0 != 0) {
					image_id = (image_id & 0x7FFFF) | dword_F64EB0;
				}
				sub_98199C(image_id, x_offset, y_offset, RCT2_GLOBAL(0x00F64ED6, uint16), RCT2_GLOBAL(0x00F64ED8, uint16), bblz - 1, height, RCT2_GLOBAL(0x009DEA52, sint16), RCT2_GLOBAL(0x009DEA54, sint16), height, get_current_rotation());
			} else
			if (entry->small_scenery.flags & SMALL_SCENERY_FLAG13) {
				// 6E043B:
				int image_id = ((gCurrentTicks / 2) & 0xF) + entry->image + 8;
				if (dword_F64EB0 != 0) {
					image_id = (image_id & 0x7FFFF) | dword_F64EB0;
				}
				sub_98199C(image_id, x_offset, y_offset, RCT2_GLOBAL(0x00F64ED6, uint16), RCT2_GLOBAL(0x00F64ED8, uint16), bblz - 1, height, RCT2_GLOBAL(0x009DEA52, sint16), RCT2_GLOBAL(0x009DEA54, sint16), height, get_current_rotation());

				image_id = direction + entry->image + 4;
				if (dword_F64EB0 != 0) {
					image_id = (image_id & 0x7FFFF) | dword_F64EB0;
				}
				sub_98199C(image_id, x_offset, y_offset, RCT2_GLOBAL(0x00F64ED6, uint16), RCT2_GLOBAL(0x00F64ED8, uint16), bblz - 1, height, RCT2_GLOBAL(0x009DEA52, sint16), RCT2_GLOBAL(0x009DEA54, sint16), height, get_current_rotation());

				image_id = ((gCurrentTicks / 2) & 0xF) + entry->image + 24;
				if (dword_F64EB0 != 0) {
					image_id = (image_id & 0x7FFFF) | dword_F64EB0;
				}
				sub_98199C(image_id, x_offset, y_offset, RCT2_GLOBAL(0x00F64ED6, uint16), RCT2_GLOBAL(0x00F64ED8, uint16), bblz - 1, height, RCT2_GLOBAL(0x009DEA52, sint16), RCT2_GLOBAL(0x009DEA54, sint16), height, get_current_rotation());
			} else
			if (entry->small_scenery.flags & SMALL_SCENERY_FLAG_IS_CLOCK) {
				// 6E035C:
				int si = ((RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_MINUTE, uint16) + 6) * 17) / 256;
				int bx = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_HOUR, uint16);
				while (bx >= 12) {
					bx -= 12;
				}
				bx = (bx * 4) + si;
				if (bx >= 48) {
					bx -= 48;
				}
				int image_id = bx + (direction * 12);
				if (image_id >= 48) {
					image_id -= 48;
				}
				
				image_id = image_id + entry->image + 68;
				if (dword_F64EB0 != 0) {
					image_id = (image_id & 0x7FFFF) | dword_F64EB0;
				}
				sub_98199C(image_id, x_offset, y_offset, RCT2_GLOBAL(0x00F64ED6, uint16), RCT2_GLOBAL(0x00F64ED8, uint16), bblz - 1, height, RCT2_GLOBAL(0x009DEA52, sint16), RCT2_GLOBAL(0x009DEA54, sint16), height, get_current_rotation());

				image_id = RCT2_GLOBAL(RCT2_ADDRESS_OS_TIME_MINUTE, uint16) + (direction * 15);
				if (image_id >= 60) {
					image_id -= 60;
				}
				image_id = image_id + entry->image + 8;
				if (dword_F64EB0 != 0) {
					image_id = (image_id & 0x7FFFF) | dword_F64EB0;
				}
				sub_98199C(image_id, x_offset, y_offset, RCT2_GLOBAL(0x00F64ED6, uint16), RCT2_GLOBAL(0x00F64ED8, uint16), bblz - 1, height, RCT2_GLOBAL(0x009DEA52, sint16), RCT2_GLOBAL(0x009DEA54, sint16), height, get_current_rotation());
			} else
			if (entry->small_scenery.flags & SMALL_SCENERY_FLAG15) {
				// 6E02F6:
				int image_id = ((gCurrentTicks + (RCT2_GLOBAL(0x9DE568, sint16) / 4) + (RCT2_GLOBAL(0x9DE56C, sint16) / 4) / 4) & 0xF) + entry->image;
				if (dword_F64EB0 != 0) {
					image_id = (image_id & 0x7FFFF) | dword_F64EB0;
				}
				sub_98199C(image_id, x_offset, y_offset, RCT2_GLOBAL(0x00F64ED6, uint16), RCT2_GLOBAL(0x00F64ED8, uint16), bblz - 1, height, RCT2_GLOBAL(0x009DEA52, sint16), RCT2_GLOBAL(0x009DEA54, sint16), height, get_current_rotation());
			} else {
				if (entry->small_scenery.flags & SMALL_SCENERY_FLAG16) {
					// nothing
				}
				int esi = gCurrentTicks;
				if (!(entry->small_scenery.flags & SMALL_SCENERY_FLAG22)) {
					// 6E01F8:
					esi += ((RCT2_GLOBAL(0x9DE568, sint16) / 4) + (RCT2_GLOBAL(0x9DE56C, sint16) / 4));
					esi += (mapElement->type & 0xC0) / 16;
				}
				// 6E0222:
				uint16 cx = entry->small_scenery.var_14;
				uint8 cl = cx & 0xFF;
				esi >>= cl;
				esi &= entry->small_scenery.var_16;
				int image_id = 0;
				if (esi < entry->small_scenery.var_18) {
					image_id = ((uint8*)entry->small_scenery.var_10)[esi];
				}
				image_id = (image_id * 4) + direction + entry->image;
				if (entry->small_scenery.flags & (SMALL_SCENERY_FLAG21 | SMALL_SCENERY_FLAG17)) {
					image_id += 4;
				}
				if (entry->small_scenery.flags & SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR) {
					image_id |= ((mapElement->properties.scenery.colour_1 & 0x1F) << 19) | 0x20000000;
					if (entry->small_scenery.flags & SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR) {
						image_id |= ((mapElement->properties.scenery.colour_1 & 0x1F) << 24) | 0x80000000;
					}
				}
				if (dword_F64EB0 != 0) {
					image_id = (image_id & 0x7FFFF) | dword_F64EB0;
				}
				if (entry->small_scenery.flags & SMALL_SCENERY_FLAG21) {
					sub_98197C(image_id, x_offset, y_offset, RCT2_GLOBAL(0x00F64ED6, uint16), RCT2_GLOBAL(0x00F64ED8, uint16), bblz - 1, height, RCT2_GLOBAL(0x009DEA52, sint16), RCT2_GLOBAL(0x009DEA54, sint16), height, get_current_rotation());
				} else {
					sub_98199C(image_id, x_offset, y_offset, RCT2_GLOBAL(0x00F64ED6, uint16), RCT2_GLOBAL(0x00F64ED8, uint16), bblz - 1, height, RCT2_GLOBAL(0x009DEA52, sint16), RCT2_GLOBAL(0x009DEA54, sint16), height, get_current_rotation());
				}
			}
		}
	}
	// 6E0556: Draw supports:
	if (mapElement->properties.scenery.colour_1 & 0x20) {
		if (!(entry->small_scenery.flags & SMALL_SCENERY_FLAG20)) {
			int ax = 0;
			int edx = height;
			if (edx & 0xF) {
				edx &= 0xFFFFFFF0;
				ax = 49;
			}
			int ebp = 0x20000000;
			if (entry->small_scenery.flags & SMALL_SCENERY_FLAG26) {
				ebp = ((mapElement->properties.scenery.colour_1 & 0x1F) << 19) | 0x20000000;
			}
			if (dword_F64EB0 != 0) {
				ebp = dword_F64EB0;
			}
			if (direction & 1) {
				wooden_b_supports_paint_setup(1, ax, edx, ebp);
			} else {
				wooden_b_supports_paint_setup(0, ax, edx, ebp);
			}
		}
	}
	// 6E05D1:
	height += entry->small_scenery.height;
	uint16 word_F64F2A = height;
	height += 7;
	height &= 0xFFF8;
	if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
		RCT2_GLOBAL(0x141E9DA, uint8) = 0x20;
	}
	// 6E05FF:
	if (entry->small_scenery.flags & SMALL_SCENERY_FLAG23) {
		height = word_F64F2A;
		if (entry->small_scenery.flags & SMALL_SCENERY_FLAG_FULL_TILE) {
			// 6E0825:
			RCT2_GLOBAL(0x141E9C4, uint16) = height;
			RCT2_GLOBAL(0x141E9C6, uint8) = 0x20;
			if (entry->small_scenery.flags & SMALL_SCENERY_FLAG_VOFFSET_CENTRE) {
				RCT2_GLOBAL(0x141E9D0, uint16) = height;
				RCT2_GLOBAL(0x141E9D2, uint8) = 0x20;
				RCT2_GLOBAL(0x141E9D4, uint16) = height;
				RCT2_GLOBAL(0x141E9D6, uint8) = 0x20;
				RCT2_GLOBAL(0x141E9C8, uint16) = height;
				RCT2_GLOBAL(0x141E9CA, uint8) = 0x20;
				RCT2_GLOBAL(0x141E9CC, uint16) = height;
				RCT2_GLOBAL(0x141E9CE, uint8) = 0x20;
				RCT2_GLOBAL(0x141E9B8, uint16) = height;
				RCT2_GLOBAL(0x141E9BA, uint8) = 0x20;
				RCT2_GLOBAL(0x141E9BC, uint16) = height;
				RCT2_GLOBAL(0x141E9BE, uint8) = 0x20;
				RCT2_GLOBAL(0x141E9B4, uint16) = height;
				RCT2_GLOBAL(0x141E9B6, uint8) = 0x20;
				RCT2_GLOBAL(0x141E9C0, uint16) = height;
				RCT2_GLOBAL(0x141E9C2, uint8) = 0x20;
			}
			return;
		}
		if (!(entry->small_scenery.flags & SMALL_SCENERY_FLAG_VOFFSET_CENTRE)) {
			return;
		}
		int cl = ((mapElement->type >> 6) + get_current_rotation()) & 3;
		// 6E075C:
		switch (cl) {
		case 0:
			RCT2_GLOBAL(0x141E9B4, uint16) = height;
			RCT2_GLOBAL(0x141E9B6, uint8) = 0x20;
			RCT2_GLOBAL(0x141E9C8, uint16) = height;
			RCT2_GLOBAL(0x141E9CA, uint8) = 0x20;
			RCT2_GLOBAL(0x141E9CC, uint16) = height;
			RCT2_GLOBAL(0x141E9CE, uint8) = 0x20;
			break;
		case 1:
			RCT2_GLOBAL(0x141E9BC, uint16) = height;
			RCT2_GLOBAL(0x141E9BE, uint8) = 0x20;
			RCT2_GLOBAL(0x141E9CC, uint16) = height;
			RCT2_GLOBAL(0x141E9CE, uint8) = 0x20;
			RCT2_GLOBAL(0x141E9D4, uint16) = height;
			RCT2_GLOBAL(0x141E9D6, uint8) = 0x20;
			break;
		case 2:
			RCT2_GLOBAL(0x141E9C0, uint16) = height;
			RCT2_GLOBAL(0x141E9C2, uint8) = 0x20;
			RCT2_GLOBAL(0x141E9D0, uint16) = height;
			RCT2_GLOBAL(0x141E9D2, uint8) = 0x20;
			RCT2_GLOBAL(0x141E9D4, uint16) = height;
			RCT2_GLOBAL(0x141E9D6, uint8) = 0x20;
			break;
		case 3:
			RCT2_GLOBAL(0x141E9B8, uint16) = height;
			RCT2_GLOBAL(0x141E9BA, uint8) = 0x20;
			RCT2_GLOBAL(0x141E9D0, uint16) = height;
			RCT2_GLOBAL(0x141E9D2, uint8) = 0x20;
			RCT2_GLOBAL(0x141E9C8, uint16) = height;
			RCT2_GLOBAL(0x141E9CA, uint8) = 0x20;
			break;
		}
		return;
	}
	if (entry->small_scenery.flags & (SMALL_SCENERY_FLAG27 | SMALL_SCENERY_FLAG_FULL_TILE)) {
		RCT2_GLOBAL(0x141E9C4, uint16) = 0xFFFF;
		if (entry->small_scenery.flags & SMALL_SCENERY_FLAG_VOFFSET_CENTRE) {
			RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
			RCT2_GLOBAL(0x141E9D4, uint16) = 0xFFFF;
			RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
			RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;
			RCT2_GLOBAL(0x141E9B8, uint16) = 0xFFFF;
			RCT2_GLOBAL(0x141E9BC, uint16) = 0xFFFF;
			RCT2_GLOBAL(0x141E9B4, uint16) = 0xFFFF;
			RCT2_GLOBAL(0x141E9C0, uint16) = 0xFFFF;
		}
		return;
	}
	if (!(entry->small_scenery.flags & SMALL_SCENERY_FLAG_VOFFSET_CENTRE)) {
		return;
	}
	int cl = ((mapElement->type >> 6) + get_current_rotation()) & 3;
	switch (cl) {
	case 0:
		RCT2_GLOBAL(0x141E9B4, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;
		break;
	case 1:
		RCT2_GLOBAL(0x141E9BC, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9CC, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9D4, uint16) = 0xFFFF;
		break;
	case 2:
		RCT2_GLOBAL(0x141E9C0, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9D4, uint16) = 0xFFFF;
		break;
	case 3:
		RCT2_GLOBAL(0x141E9B8, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9D0, uint16) = 0xFFFF;
		RCT2_GLOBAL(0x141E9C8, uint16) = 0xFFFF;
		break;
	}
	return;
}
