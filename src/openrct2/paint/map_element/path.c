#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../../config/Config.h"
#include "../../game.h"
#include "../../interface/viewport.h"
#include "../../localisation/localisation.h"
#include "../../object_list.h"
#include "../../peep/staff.h"
#include "../../ride/track.h"
#include "../../ride/track_paint.h"
#include "../../world/footpath.h"
#include "../../world/scenery.h"
#include "../paint.h"
#include "../supports.h"
#include "map_element.h"
#include "surface.h"
#include "../../world/map.h"
#include "../../drawing/lightfx.h"

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

void path_paint_pole_support(rct_map_element * mapElement, sint32 height, rct_footpath_entry * footpathEntry, bool hasFences, uint32 imageFlags, uint32 sceneryImageFlags);

void path_paint_box_support(rct_map_element* mapElement, sint16 height, rct_footpath_entry* footpathEntry, bool hasFences, uint32 imageFlags, uint32 sceneryImageFlags);

/* rct2: 0x006A5AE5 */
static void path_bit_lights_paint(rct_scenery_entry* pathBitEntry, rct_map_element* mapElement, sint32 height, uint8 edges, uint32 pathBitImageFlags) {
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
static void path_bit_bins_paint(rct_scenery_entry* pathBitEntry, rct_map_element* mapElement, sint32 height, uint8 edges, uint32 pathBitImageFlags) {
    if (footpath_element_is_sloped(mapElement))
        height += 8;

    uint32 imageId;

    if (!(edges & (1 << 0))) {
        imageId = pathBitEntry->image + 5;

        imageId |= pathBitImageFlags;

        if (!(mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)) {
            imageId -= 4;

            // Edges have been rotated around the rotation to check addition status
            // this will also need to be rotated.
            if (!(mapElement->properties.path.addition_status & ror8(0x3,(2 * get_current_rotation()))))
                imageId += 8;
        }


        sub_98197C(imageId, 7, 16, 1, 1, 7, height, 7, 16, height + 2, get_current_rotation());
    }
    if (!(edges & (1 << 1))) {
        imageId = pathBitEntry->image + 6;

        imageId |= pathBitImageFlags;

        if (!(mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)) {
            imageId -= 4;

            // Edges have been rotated around the rotation to check addition status
            // this will also need to be rotated.
            if (!(mapElement->properties.path.addition_status & ror8(0xC, (2 * get_current_rotation()))))
                imageId += 8;
        }


        sub_98197C(imageId, 16, 25, 1, 1, 7, height, 16, 25, height + 2, get_current_rotation());
    }

    if (!(edges & (1 << 2))) {
        imageId = pathBitEntry->image + 7;

        imageId |= pathBitImageFlags;

        if (!(mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)) {
            imageId -= 4;

            // Edges have been rotated around the rotation to check addition status
            // this will also need to be rotated.
            if (!(mapElement->properties.path.addition_status & ror8(0x30, (2 * get_current_rotation()))))
                imageId += 8;
        }


        sub_98197C(imageId, 25, 16, 1, 1, 7, height, 25, 16, height + 2, get_current_rotation());
    }

    if (!(edges & (1 << 3))) {
        imageId = pathBitEntry->image + 8;

        imageId |= pathBitImageFlags;

        if (!(mapElement->flags & MAP_ELEMENT_FLAG_BROKEN)) {
            imageId -= 4;

            // Edges have been rotated around the rotation to check addition status
            // this will also need to be rotated.
            if (!(mapElement->properties.path.addition_status & ror8(0xC0, (2 * get_current_rotation()))))
                imageId += 8;
        }


        sub_98197C(imageId, 16, 7, 1, 1, 7, height, 16, 7, height + 2, get_current_rotation());
    }
}

/* rct2: 0x006A5E81 */
static void path_bit_benches_paint(rct_scenery_entry* pathBitEntry, rct_map_element* mapElement, sint32 height, uint8 edges, uint32 pathBitImageFlags) {
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
static void path_bit_jumping_fountains_paint(rct_scenery_entry* pathBitEntry, rct_map_element* mapElement, sint32 height, uint8 edges, uint32 pathBitImageFlags, rct_drawpixelinfo* dpi) {
    if (dpi->zoom_level != 0)
        return;

    uint32 imageId = pathBitEntry->image;
    imageId |= pathBitImageFlags;

    sub_98197C(imageId + 1, 0, 0, 1, 1, 2, height, 3, 3, height + 2, get_current_rotation());
    sub_98197C(imageId + 2, 0, 0, 1, 1, 2, height, 3, 29, height + 2, get_current_rotation());
    sub_98197C(imageId + 3, 0, 0, 1, 1, 2, height, 29, 29, height + 2, get_current_rotation());
    sub_98197C(imageId + 4, 0, 0, 1, 1, 2, height, 29, 3, height + 2, get_current_rotation());
}

/**
 * rct2: 0x006A4101
 * @param map_element (esi)
 * @param (ecx)
 * @param ebp (ebp)
 * @param base_image_id (0x00F3EF78)
 */
static void sub_6A4101(rct_map_element * map_element, uint16 height, uint32 ebp, bool word_F3F038, rct_footpath_entry * footpathEntry, uint32 base_image_id, uint32 imageFlags)
{
    if (footpath_element_is_queue(map_element)) {
        uint8 local_ebp = ebp & 0x0F;
        if (footpath_element_is_sloped(map_element)) {
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
        gPaintSession.InteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
        if (footpath_element_is_sloped(map_element)) {
            if (footpath_element_get_slope_direction(map_element) == direction)
                height += 16;
        }
        direction += get_current_rotation();
        direction &= 3;

        rct_xyz16 boundBoxOffsets = {
            .x = BannerBoundBoxes[direction][0].x,
            .y = BannerBoundBoxes[direction][0].y,
            .z = height + 2
        };

        uint32 imageId = (direction << 1) + base_image_id + 101;

        sub_98197C(imageId, 0, 0, 1, 1, 21, height, boundBoxOffsets.x, boundBoxOffsets.y, boundBoxOffsets.z, get_current_rotation());

        boundBoxOffsets.x = BannerBoundBoxes[direction][1].x;
        boundBoxOffsets.y = BannerBoundBoxes[direction][1].y;
        imageId++;
        sub_98197C(imageId, 0, 0, 1, 1, 21, height, boundBoxOffsets.x, boundBoxOffsets.y, boundBoxOffsets.z, get_current_rotation());

        direction--;
        // If text shown
        if (direction < 2 && map_element->properties.path.ride_index != 255 && imageFlags == 0) {
            uint16 scrollingMode = footpathEntry->scrolling_mode;
            scrollingMode += direction;

            set_format_arg(0, uint32, 0);
            set_format_arg(4, uint32, 0);

            rct_ride* ride = get_ride(map_element->properties.path.ride_index);
            rct_string_id string_id = STR_RIDE_ENTRANCE_CLOSED;
            if (ride->status == RIDE_STATUS_OPEN && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)){
                set_format_arg(0, rct_string_id, ride->name);
                set_format_arg(2, uint32, ride->name_arguments);
                string_id = STR_RIDE_ENTRANCE_NAME;
            }
            if (gConfigGeneral.upper_case_banners) {
                format_string_to_upper(gCommonStringFormatBuffer, 256, string_id, gCommonFormatArgs);
            } else {
                format_string(gCommonStringFormatBuffer, 256, string_id, gCommonFormatArgs);
            }

            gCurrentFontSpriteBase = FONT_SPRITE_BASE_TINY;

            uint16 string_width = gfx_get_string_width(gCommonStringFormatBuffer);
            uint16 scroll = (gCurrentTicks / 2) % string_width;

            sub_98199C(scrolling_text_setup(string_id, scroll, scrollingMode), 0, 0, 1, 1, 21, height + 7,  boundBoxOffsets.x,  boundBoxOffsets.y,  boundBoxOffsets.z, get_current_rotation());
        }

        gPaintSession.InteractionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;
        if (imageFlags != 0) {
            gPaintSession.InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
        }
        return;
    }


    // save ecx, ebp, esi
    uint32 dword_F3EF80 = ebp;
    if (!(footpathEntry->flags & FOOTPATH_ENTRY_FLAG_HAS_PATH_BASE_SPRITE)) {
        dword_F3EF80 &= 0x0F;
    }

    if (footpath_element_is_sloped(map_element)) {
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
 * @param footpathEntry (0x00F3EF6C)
 * @param imageFlags (0x00F3EF70)
 * @param sceneryImageFlags (0x00F3EF74)
 */
static void sub_6A3F61(rct_map_element * map_element, uint16 bp, uint16 height, rct_footpath_entry * footpathEntry, uint32 imageFlags, uint32 sceneryImageFlags, bool word_F3F038)
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

    rct_drawpixelinfo * dpi = gPaintSession.Unk140E9A8;

    if (dpi->zoom_level <= 1) {
        if (!gTrackDesignSaveMode) {
            uint8 additions = map_element->properties.path.additions & 0xF;
            if (additions != 0) {
                gPaintSession.InteractionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM;
                if (sceneryImageFlags != 0) {
                    gPaintSession.InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
                }

                // Draw additional path bits (bins, benches, lamps, queue screens)
                rct_scenery_entry* sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(map_element));
                switch (sceneryEntry->path_bit.draw_type) {
                case PATH_BIT_DRAW_TYPE_LIGHTS:
                    path_bit_lights_paint(sceneryEntry, map_element, height, (uint8)bp, sceneryImageFlags);
                    break;
                case PATH_BIT_DRAW_TYPE_BINS:
                    path_bit_bins_paint(sceneryEntry, map_element, height, (uint8)bp, sceneryImageFlags);
                    break;
                case PATH_BIT_DRAW_TYPE_BENCHES:
                    path_bit_benches_paint(sceneryEntry, map_element, height, (uint8)bp, sceneryImageFlags);
                    break;
                case PATH_BIT_DRAW_TYPE_JUMPING_FOUNTAINS:
                    path_bit_jumping_fountains_paint(sceneryEntry, map_element, height, (uint8)bp, sceneryImageFlags, dpi);
                    break;
                }

                gPaintSession.InteractionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;

                if (sceneryImageFlags != 0) {
                    gPaintSession.InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
                }
            }
        }

        // Redundant zoom-level check removed

        sub_6A4101(map_element, height, bp, word_F3F038, footpathEntry, footpathEntry->image | imageFlags, imageFlags);
    }

    // This is about tunnel drawing
    uint8 direction = (map_element->properties.path.type + get_current_rotation()) & 0x03;
    uint8 diagonal = map_element->properties.path.type & 0x04;
    uint8 bl = direction | diagonal;

    if (bp & 2) {
        // Bottom right of tile is a tunnel
        if (bl == 5) {
            paint_util_push_tunnel_right(height + 16, TUNNEL_10);
        } else if (bp & 1) {
            paint_util_push_tunnel_right(height, TUNNEL_11);
        } else {
            paint_util_push_tunnel_right(height, TUNNEL_10);
        }
    }

    if (!(bp & 4)) {
        return;
    }

    // Bottom left of the tile is a tunnel
    if (bl == 6) {
        paint_util_push_tunnel_left(height + 16, TUNNEL_10);
    } else if (bp & 8) {
        paint_util_push_tunnel_left(height , TUNNEL_11);
    } else {
        paint_util_push_tunnel_left(height , TUNNEL_10);
    }
}

/**
 * rct2: 0x0006A3590
 */
void path_paint(uint8 direction, uint16 height, rct_map_element * map_element)
{
    gPaintSession.InteractionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;

    bool word_F3F038 = false;

    uint32 sceneryImageFlags = 0;
    uint32 imageFlags = 0;

    if (gTrackDesignSaveMode) {
        if (footpath_element_is_queue(map_element)) {
            if (map_element->properties.path.ride_index != gTrackDesignSaveRideIndex) {
                return;
            }
        }

        if (!track_design_save_contains_map_element(map_element)) {
            imageFlags = 0x21700000;
        }
    }

    if (footpath_element_path_scenery_is_ghost(map_element)) {
        sceneryImageFlags = construction_markers[gConfigGeneral.construction_marker_colour];
    }

    if (map_element->flags & MAP_ELEMENT_FLAG_GHOST) {
        gPaintSession.InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
        imageFlags = construction_markers[gConfigGeneral.construction_marker_colour];
    }

    sint16 x = gPaintSession.MapPosition.x, y = gPaintSession.MapPosition.y;

    rct_map_element * surface = map_get_surface_element_at(x / 32, y / 32);

    uint16 bl = height / 8;
    if (surface == NULL) {
        word_F3F038 = true;
    } else if (surface->base_height != bl) {
        word_F3F038 = true;
    } else {
        if (footpath_element_is_sloped(map_element)) {
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

    if (gStaffDrawPatrolAreas != 0xFFFF) {
        sint32 staffIndex = gStaffDrawPatrolAreas;
        uint8 staffType = staffIndex & 0x7FFF;
        bool is_staff_list = staffIndex & 0x8000;
        x = gPaintSession.MapPosition.x;
        y = gPaintSession.MapPosition.y;

        uint8 patrolColour = COLOUR_LIGHT_BLUE;

        if (!is_staff_list) {
            rct_peep * staff = GET_PEEP(staffIndex);
            if (!staff_is_patrol_area_set(staff->staff_id, x, y)) {
                patrolColour = COLOUR_GREY;
            }
            staffType = staff->staff_type;
        }

        if (staff_is_patrol_area_set(200 + staffType, x, y)) {
            uint32 imageId = 2618;
            sint32 height2 = map_element->base_height * 8;
            if (footpath_element_is_sloped(map_element)) {
                imageId = 2619 + ((map_element->properties.path.type + get_current_rotation()) & 3);
                height2 += 16;
            }

            sub_98196C(imageId | patrolColour << 19 | IMAGE_TYPE_REMAP, 16, 16, 1, 1, 0, height2 + 2, get_current_rotation());
        }
    }


    if (gCurrentViewportFlags & VIEWPORT_FLAG_PATH_HEIGHTS) {
        uint16 height2 = 3 + map_element->base_height * 8;
        if (footpath_element_is_sloped(map_element)) {
            height2 += 8;
        }
        uint32 imageId = (SPR_HEIGHT_MARKER_BASE + height2 / 16) | COLOUR_GREY << 19 | IMAGE_TYPE_REMAP;
        imageId += get_height_marker_offset();
        imageId -= gMapBaseZ;
        sub_98196C(imageId, 16, 16, 1, 1, 0, height2, get_current_rotation());
    }

    uint8 pathType = (map_element->properties.path.type & 0xF0) >> 4;
    rct_footpath_entry * footpathEntry = get_footpath_entry(pathType);

    if (footpathEntry != (void*)-1) {
        if (footpathEntry->support_type == FOOTPATH_ENTRY_SUPPORT_TYPE_POLE) {
            path_paint_pole_support(map_element, height, footpathEntry, word_F3F038, imageFlags, sceneryImageFlags);
        }
        else {
            path_paint_box_support(map_element, height, footpathEntry, word_F3F038, imageFlags, sceneryImageFlags);
        }
    }

#ifdef __ENABLE_LIGHTFX__
    if (gConfigGeneral.enable_light_fx) {
        if (footpath_element_has_path_scenery(map_element) && !(map_element->flags & MAP_ELEMENT_FLAG_BROKEN)) {
            rct_scenery_entry *sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(map_element));
            if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_LAMP) {
                if (!(map_element->properties.path.edges & (1 << 0))) {
                    lightfx_add_3d_light_magic_from_drawing_tile(-16, 0, height + 23, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                }
                if (!(map_element->properties.path.edges & (1 << 1))) {
                    lightfx_add_3d_light_magic_from_drawing_tile(0, 16, height + 23, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                }
                if (!(map_element->properties.path.edges & (1 << 2))) {
                    lightfx_add_3d_light_magic_from_drawing_tile(16, 0, height + 23, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                }
                if (!(map_element->properties.path.edges & (1 << 3))) {
                    lightfx_add_3d_light_magic_from_drawing_tile(0, -16, height + 23, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                }
            }
        }
    }
#endif
}

void path_paint_pole_support(rct_map_element * mapElement, sint32 height, rct_footpath_entry * footpathEntry, bool hasFences, uint32 imageFlags, uint32 sceneryImageFlags)
{
    // Rol edges around rotation
    uint8 edges = ((mapElement->properties.path.edges << get_current_rotation()) & 0xF) |
        (((mapElement->properties.path.edges & 0xF) << get_current_rotation()) >> 4);

    uint8 corners = (((mapElement->properties.path.edges >> 4) << get_current_rotation()) & 0xF) |
        (((mapElement->properties.path.edges >> 4) << get_current_rotation()) >> 4);


    rct_xy16 boundBoxOffset = {.x =stru_98D804[edges][0], .y = stru_98D804[edges][1]};
    rct_xy16 boundBoxSize = {.x =stru_98D804[edges][2], .y = stru_98D804[edges][3]};

    uint16 edi = edges | (corners << 4);

    uint32 imageId;
    if (footpath_element_is_sloped(mapElement)) {
        imageId = ((mapElement->properties.path.type + get_current_rotation()) & 3) + 16;
    } else {
        imageId = byte_98D6E0[edi];
    }

    imageId += footpathEntry->image;
    if (footpath_element_is_queue(mapElement)) {
        imageId += 51;
    }

    if (!gPaintSession.DidPassSurface) {
        boundBoxOffset.x = 3;
        boundBoxOffset.y = 3;
        boundBoxSize.x = 26;
        boundBoxSize.y = 26;
    }

    if (!hasFences || !gPaintSession.DidPassSurface) {
        sub_98197C(imageId | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y, height + 1, get_current_rotation());
    } else {
        uint32 image_id;
        if (footpath_element_is_sloped(mapElement)) {
            image_id = ((mapElement->properties.path.type + get_current_rotation()) & 3) + footpathEntry->bridge_image + 51;
        } else {
            image_id = byte_98D8A4[edges] + footpathEntry->bridge_image + 49;
        }

        sub_98197C(image_id | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y, height + 1, get_current_rotation());

        if (!footpath_element_is_queue(mapElement) && !(footpathEntry->flags & FOOTPATH_ENTRY_FLAG_HAS_PATH_BASE_SPRITE)) {
            // don't draw
        } else {
            sub_98199C(imageId | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y, height + 1, get_current_rotation());
        }
    }


    sub_6A3F61(mapElement, edi, height, footpathEntry, imageFlags, sceneryImageFlags, hasFences);

    uint16 ax = 0;
    if (footpath_element_is_sloped(mapElement)) {
        ax = ((mapElement->properties.path.type + get_current_rotation()) & 0x3) + 1;
    }

    if (byte_98D8A4[edges] == 0) {
        path_a_supports_paint_setup(0, ax, height, imageFlags, footpathEntry, NULL);
    } else {
        path_a_supports_paint_setup(1, ax, height, imageFlags, footpathEntry, NULL);
    }

    height += 32;
    if (footpath_element_is_sloped(mapElement)) {
        height += 16;
    }

    paint_util_set_general_support_height(height, 0x20);

    if (footpath_element_is_queue(mapElement)
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

void path_paint_box_support(rct_map_element* mapElement, sint16 height, rct_footpath_entry* footpathEntry, bool hasFences, uint32 imageFlags, uint32 sceneryImageFlags)
{
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
    if (footpath_element_is_queue(mapElement)) {
        imageId += 51;
    }

    // Below Surface
    if (!gPaintSession.DidPassSurface) {
        boundBoxOffset.x = 3;
        boundBoxOffset.y = 3;
        boundBoxSize.x = 26;
        boundBoxSize.y = 26;
    }

    if (!hasFences || !gPaintSession.DidPassSurface) {
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

        if (footpath_element_is_queue(mapElement) || (footpathEntry->flags & FOOTPATH_ENTRY_FLAG_HAS_PATH_BASE_SPRITE)) {
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

    for (sint8 i = 3; i > -1; --i) {
        if (!(edges & (1 << i))) {
            path_b_supports_paint_setup(supports[i], ax, height, imageFlags, footpathEntry);
        }
    }

    height += 32;
    if (footpath_element_is_sloped(mapElement)) {
        height += 16;
    }

    paint_util_set_general_support_height(height, 0x20);

    if (footpath_element_is_queue(mapElement)
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
