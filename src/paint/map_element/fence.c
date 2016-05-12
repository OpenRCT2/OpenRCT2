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

#include "../../common.h"
#include "../../addresses.h"
#include "../../world/map.h"
#include "../../drawing/drawing.h"
#include "../../world/scenery.h"
#include "../../game.h"
#include "../../ride/track.h"
#include "../../config.h"
#include "../../localisation/localisation.h"
#include "../../interface/colour.h"
#include "../../interface/viewport.h"
#include "../paint.h"

uint32 dword_993CC4[] = {
    COLOUR_DARK_GREEN << 19 | COLOUR_GREY << 24 | 0x20000000,
    COLOUR_YELLOW << 19 | COLOUR_GREY << 24 | 0x40000000
};

const uint8 byte_9A406C[] = {
    2, 2, 22, 26, 30, 34, 34, 34, 34, 34, 30, 26, 22, 2, 6, 2,
    2, 2, 6, 10, 14, 18, 18, 18, 18, 18, 14, 10, 6, 2, 22, 2
};

const uint8 byte_9A408C[] = {
    0, 0, 4, 8, 12, 16, 16, 16, 16, 16, 12, 8, 4, 0, 20, 0,
    0, 0, 20, 24, 28, 32, 32, 32, 32, 32, 28, 24, 20, 0, 4, 0
};

const uint8 byte_9A40AC[] = {
    2, 2, 6, 10, 14, 18, 18, 18, 18, 18, 14, 10, 6, 2, 22, 2,
    2, 2, 22, 26, 30, 34, 34, 34, 34, 34, 30, 26, 22, 2, 6, 2
};
const uint8 byte_9A40CC[] = {
    0, 0, 20, 24, 28, 32, 32, 32, 32, 32, 28, 24, 20, 0, 4, 0,
    0, 0, 4, 8, 12, 16, 16, 16, 16, 16, 12, 8, 4, 0, 20, 0
};

void do_part_1(uint32 imageId,
               rct_scenery_entry * sceneryEntry,
               uint32 dword_141F714, uint32 dword_141F71C, uint32 dword_141F710,
               rct_xyz16 offset,
               rct_xyz16 boundsR1, rct_xyz16 boundsR1_,
               rct_xyz16 boundsR2, rct_xyz16 boundsR2_,
               rct_xyz16 boundsL1, rct_xyz16 boundsL1_)
{
    if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG1) {
        imageId |= dword_141F714;
    }

    if (dword_141F710 != 0) {
        imageId = (imageId & 0x7FFFF) | dword_141F710;
    }

    if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG4) {
        paint_struct * ps;

        ps = sub_98197C(imageId, offset.x, offset.y, boundsR1.x, boundsR1.y, boundsR1.z, offset.z, boundsR1_.x, boundsR1_.y, boundsR1_.z, get_current_rotation());
        if (ps != NULL) {
            ps->tertiary_colour = dword_141F71C;
        }

        ps = sub_98197C(imageId + 1, offset.x, offset.y, boundsR2.x, boundsR2.y, boundsR2.z, offset.z, boundsR2_.x, boundsR2_.y, boundsR2_.z, get_current_rotation());
        if (ps != NULL) {
            ps->tertiary_colour = dword_141F71C;
        }
    } else {
        paint_struct * ps;

        ps = sub_98197C(imageId, offset.x, offset.y, boundsL1.x, boundsL1.y, boundsL1.z, offset.z, boundsL1_.x, boundsL1_.y, boundsL1_.z, get_current_rotation());
        if (ps != NULL) {
            ps->tertiary_colour = dword_141F71C;
        }

        ps = sub_98199C(imageId + 1, offset.x, offset.y, boundsL1.x, boundsL1.y, boundsL1.z, offset.z, boundsL1_.x, boundsL1_.y, boundsL1_.z, get_current_rotation());
        if (ps != NULL) {
            ps->tertiary_colour = dword_141F71C;
        }
    }
}

void do_part_2(uint32 frameNum, const rct_scenery_entry * sceneryEntry, uint32 dword_141F710, uint32 dword_141F714, uint32 dword_141F718, uint32 dword_141F71C, uint32 imageOffset, rct_xyz16 offset, rct_xyz16 bounds, rct_xyz16 boundsOffset)
{
    uint32 baseImageId = sceneryEntry->image + imageOffset + frameNum;
    uint32 imageId = baseImageId;


    if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG2) {
        if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG1) {
            imageId |= dword_141F714;
        }

        if (dword_141F710 != 0) {
            imageId = (imageId & 0x7FFFF) | dword_141F710;
        }

        sub_98197C(imageId, offset.x, offset.y, bounds.x, bounds.y, bounds.z, offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z, get_current_rotation());
        if (dword_141F710 == 0) {
            imageId = baseImageId + dword_141F718;
            sub_98199C(imageId, offset.x, offset.y, bounds.x, bounds.y, bounds.z, offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z, get_current_rotation());
        }
    } else {
        if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG1) {
            imageId |= dword_141F714;
        }

        if (dword_141F710 != 0) {
            imageId = (imageId & 0x7FFFF) | dword_141F710;
        }

        paint_struct * paint = sub_98197C(imageId, offset.x, offset.y, bounds.x, bounds.y, bounds.z, offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z, get_current_rotation());
        if (paint != NULL) {
            paint->tertiary_colour = dword_141F71C;
        }
    }
}
/**
 * rct2: 0x006E44B0
 * @param direction (cl)
 * @param height (dx)
 * @param map_element (esi)
 */
void fence_paint(uint8 direction, int height, rct_map_element * map_element)
{
    rct_drawpixelinfo * dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo *);
    RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_WALL;

    rct_scenery_entry * sceneryEntry = g_wallSceneryEntries[map_element->properties.fence.type];
    uint32 frameNum = 0;

    if (sceneryEntry->wall.flags2 & WALL_SCENERY_FLAG5) {
        // animation?
        frameNum = (gCurrentTicks & 7) * 2;
    }


    uint32 dword_141F714 = (map_element->properties.fence.item[1] & 0b11111) << 19 | 0x20000000;
    uint32 dword_141F718 = dword_141F714 + 0x23800006;

    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
        uint8 secondaryColour = (map_element->properties.fence.item[1] >> 5) | ((map_element->flags & 0x60) >> 2);
        dword_141F714 |= secondaryColour << 24 | 0x80000000;
    }

    uint32 dword_141F71C = 0;
    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR) {
        dword_141F71C = map_element->properties.fence.item[0];
        dword_141F714 &= 0x0DFFFFFFF;
    }

    int clearanceHeight = ceil2(map_element->clearance_height * 8 + 15, 16);
    if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
        RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
        RCT2_GLOBAL(0x141E9DA, uint8) = 0x20;
    }

    uint32 dword_141F710 = 0;
    if (RCT2_GLOBAL(0x9DEA6F, uint8) & 1) {
        if (!track_design_save_contains_map_element(map_element)) {
            dword_141F710 = 0x21700000;
        }
    }

    if (map_element->flags & MAP_ELEMENT_FLAG_GHOST) {
        RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_NONE;
        dword_141F710 = dword_993CC4[gConfigGeneral.construction_marker_colour];
    }

    // save map_element

    uint8 ah = sceneryEntry->wall.height * 8 - 2;


    if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG5) {
        rct_xyz16 offset, boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_;
        uint32 ebx = (map_element->properties.fence.item[2] >> 3) & 0x1F;
        switch (direction) {
            case 0:
                ebx = sceneryEntry->image + byte_9A406C[ebx];

                boundsR1 = (rct_xyz16) {1, 3, ah - 5};
                boundsR1_ = (rct_xyz16) {1, 1, height + 1};
                boundsR2 = (rct_xyz16) {1, 28, 3};
                boundsR2_ = (rct_xyz16) {1, 1, height + ah - 9};

                boundsL1 = (rct_xyz16) {1, 28, ah};
                boundsL1_ = (rct_xyz16) {1, 1, height + 1};

                offset = (rct_xyz16) {0, 0, height};

                do_part_1(ebx, sceneryEntry, dword_141F714, dword_141F71C, dword_141F710, offset, boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;

            case 1:
                ebx = sceneryEntry->image + byte_9A408C[ebx];

                boundsR1 = (rct_xyz16) {3, 3, ah - 5};
                boundsR1_ = (rct_xyz16) {1, 30, height + 1};
                boundsR2 = (rct_xyz16) {29, 3, 2};
                boundsR2_ = (rct_xyz16) {1, 30, height + ah - 8};

                boundsL1 = (rct_xyz16) {29, 1, ah};
                boundsL1_ = (rct_xyz16) {2, 30, height + 1};

                offset = (rct_xyz16) {1, 31, height};

                do_part_1(ebx, sceneryEntry, dword_141F714, dword_141F71C, dword_141F710, offset, boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;

            case 2:
                ebx = sceneryEntry->image + byte_9A40AC[ebx];

                boundsR1 = (rct_xyz16) {3, 3, ah - 5};
                boundsR1_ = (rct_xyz16) {30, 1, height + 1};
                boundsR2 = (rct_xyz16) {3, 29, 2};
                boundsR2_ = (rct_xyz16) {30, 1, height + ah - 8};

                boundsL1 = (rct_xyz16) {1, 29, ah};
                boundsL1_ = (rct_xyz16) {30, 2, height + 1};

                offset = (rct_xyz16) {31, 0, height};

                do_part_1(ebx, sceneryEntry, dword_141F714, dword_141F71C, dword_141F710, offset, boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;

            case 3:
                ebx = sceneryEntry->image + byte_9A40CC[ebx];

                boundsR1 = (rct_xyz16) {3, 1, ah - 5};
                boundsR1_ = (rct_xyz16) {1, 1, height + 1};
                boundsR2 = (rct_xyz16) {28, 1, 3};
                boundsR2_ = (rct_xyz16) {1, 1, height + ah - 9};

                boundsL1 = (rct_xyz16) {28, 1, ah};
                boundsL1_ = (rct_xyz16) {1, 1, height + 1};

                offset = (rct_xyz16) {2, 1, height};

                do_part_1(ebx, sceneryEntry, dword_141F714, dword_141F71C, dword_141F710, offset, boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;
        }

        return;
    }


    uint8 al = map_element->type;
    uint32 imageOffset;
    rct_xyz16 offset, bounds, boundsOffset;

    switch (direction) {
        case 0:
            if (al & 0x80) {
                imageOffset = 3;
            } else if (al & 0x40) {
                imageOffset = 5;
            } else {
                imageOffset = 1;
            }

            offset = (rct_xyz16){0, 0, height};
            bounds = (rct_xyz16){1, 28, ah};
            boundsOffset = (rct_xyz16){1, 1, height + 1};
			break;

        case 1:
            if (al & 0x80) {
                imageOffset = 2;
            } else if (al & 0x40) {
                imageOffset = 4;
            } else {
                imageOffset = 0;
            }

            if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG2) {
                if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG4) {
                    imageOffset += 12;
                }
            } else {
                if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG4) {
                    imageOffset += 6;
                }
            }

            offset = (rct_xyz16){1, 31, height};
            bounds = (rct_xyz16){29, 1, ah};
            boundsOffset = (rct_xyz16){2, 30, height + 1};
            break;

        case 2:
            if (al & 0x80) {
                imageOffset = 5;
            } else if (al & 0x40) {
                imageOffset = 3;
            } else {
                imageOffset = 1;
            }

            if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG4) {
                imageOffset += 6;
            }

            offset = (rct_xyz16){30, 0, height};
            bounds = (rct_xyz16){1, 29, ah};
            boundsOffset = (rct_xyz16){30, 2, height + 1};
            break;

        case 3:
            if (al & 0x80) {
                imageOffset = 4;
            } else if (al & 0x40) {
                imageOffset = 2;
            } else {
                imageOffset = 0;
            }

            offset = (rct_xyz16){2, 1, height};
            bounds = (rct_xyz16){28, 1, ah};
            boundsOffset = (rct_xyz16){1, 1, height + 1};
            break;
    }

    do_part_2(frameNum, sceneryEntry, dword_141F710, dword_141F714, dword_141F718, dword_141F71C, imageOffset, offset, bounds, boundsOffset);


    if (sceneryEntry->wall.var_0D == 0xFF) {
        return;
    }

    if (direction != 0 && direction != 3) {
        return;
    }

    if (sceneryEntry->wall.flags & WALL_SCENERY_FLAG1) {
        return;
    }

    RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint32) = 0;
    RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 4, uint32) = 0;

    uint8 al_2 = map_element->properties.fence.item[1] >> 5 | (map_element->flags & 0x60) >> 2;

    if (dword_141F710 != 0) {
        al_2 = COLOUR_GREY;
    }

    if (get_current_rotation() == 0) {
        al_2 |= 0x80;
    }

    RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 7, uint8) = al_2;

    uint16 scrollingMode = sceneryEntry->wall.var_0D + (get_current_rotation() + 1) & 0x3;

    uint8 bannerIndex = map_element->properties.fence.item[0];
    rct_banner * banner = &gBanners[bannerIndex];
    RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, rct_string_id) = banner->string_idx;
    if (banner->flags & 0x04) {
        rct_ride * ride = get_ride(banner->colour);
        RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, rct_string_id) = ride->name;
        RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, uint32) = ride->name_arguments;
    }

    sub_98199C(0, 0, 0, 1, 1, 13, height + 8, 0, 0, 0, get_current_rotation());


    utf8 signString[MAX_PATH];
    rct_string_id stringId = STR_SCROLLING_SIGN_TEXT;
    if (gConfigGeneral.upper_case_banners) {
        format_string_to_upper(signString, stringId, RCT2_ADDRESS(RCT2_ADDRESS_COMMON_FORMAT_ARGS, void));
    } else {
        format_string(signString, stringId, RCT2_ADDRESS(RCT2_ADDRESS_COMMON_FORMAT_ARGS, void));
    }

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_TINY;

    uint16 string_width = gfx_get_string_width(signString);
    uint16 scroll = (gCurrentTicks / 2) % string_width;

    sub_98199C(scrolling_text_setup(stringId, scroll, scrollingMode), 0, 0, 1, 1, 13, height + 8, 1, 1, height + 1, get_current_rotation());
}

