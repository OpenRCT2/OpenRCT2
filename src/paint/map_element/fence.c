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

void fence_paint(uint8 direction, int height, rct_map_element * map_element)
{
    //RCT2_CALLPROC_X(0x6E44B0, 0, 0, direction, height, (int) map_element, 0, 0);

    rct_drawpixelinfo * dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo *);
    RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_WALL;

    rct_scenery_entry * dword_141F70C = g_wallSceneryEntries[map_element->properties.fence.type];
    uint32 dword_141F708 = 0;

    if (dword_141F70C->wall.flags2 & WALL_SCENERY_FLAG5) {
        // animation?
        dword_141F708 = (gCurrentTicks & 7) * 2;
    }


    uint32 dword_141F714 = (map_element->properties.fence.item[1] & 0b11111) << 19 | 0x20000000;
    uint32 dword_141F718 = dword_141F714 + 0x23800006;

    if (dword_141F70C->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
        uint8 secondaryColour = (map_element->properties.fence.item[1] >> 5) | ((map_element->flags & 0x60) >> 2);
        dword_141F714 |= secondaryColour << 24 | 0x80000000;
    }

    uint32 dword_141F71C = 0;
    if (dword_141F70C->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR) {
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
        if (!track_save_contains_map_element(map_element)) {
            dword_141F710 = 0x21700000;
        }
    }

    if (map_element->flags & MAP_ELEMENT_FLAG_GHOST) {
        RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_NONE;
        dword_141F710 = dword_993CC4[gConfigGeneral.construction_marker_colour];
    }

    // save map_element

    uint8 ah = dword_141F70C->wall.height * 8 - 2;

    if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG5) {
        uint32 ebx = (map_element->properties.fence.item[2] >> 3) & 0x1F;

        switch (direction) {
            case 0:
                ebx = dword_141F70C->image + byte_9A406C[ebx];
                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG1) {
                    ebx |= dword_141F714;
                }

                if (dword_141F710 != 0) {
                    ebx = (ebx & 0x7FFFF) | dword_141F710;
                }

                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG4) {
                    paint_struct * ps;

                    ps = sub_98197C(ebx, 0, 0, 1, 3, ah - 5, height, 1, 1, height + 1, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }

                    ps = sub_98197C(ebx + 1, 0, 0, 1, 28, 3, height, 1, 1, height + ah - 5 - 4, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }
                } else {
                    paint_struct * ps;

                    ps = sub_98197C(ebx, 0, 0, 1, 28, ah, height, 1, 1, height + 1, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }

                    ps = sub_98199C(ebx + 1, 0, 0, 1, 28, ah, height, 1, 1, height + 1, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }
                }
                break;

            case 1:
                ebx = dword_141F70C->image + byte_9A408C[ebx];
                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG1) {
                    ebx |= dword_141F714;
                }

                if (dword_141F710 != 0) {
                    ebx = (ebx & 0x7FFFF) | dword_141F710;
                }

                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG4) {
                    paint_struct * ps;

                    ps = sub_98197C(ebx, 1, 31, 3, 3, ah - 5, height, 1, 30, height + 1, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }

                    ps = sub_98197C(ebx + 1, 1, 31, 29, 3, 2, height, 1, 30, height + ah - 8, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }
                } else {
                    paint_struct * ps;

                    ps = sub_98197C(ebx, 1, 31, 29, 1, ah, height, 2, 30, height + 1, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }

                    ps = sub_98199C(ebx + 1, 1, 31, 29, 1, ah, height, 2, 30, height + 1, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }
                }
                break;

            case 2:
                ebx = dword_141F70C->image + byte_9A40AC[ebx];
                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG1) {
                    ebx |= dword_141F714;
                }

                if (dword_141F710 != 0) {
                    ebx = (ebx & 0x7FFFF) | dword_141F710;
                }

                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG4) {
                    paint_struct * ps;

                    ps = sub_98197C(ebx, 31, 0, 3, 3, ah - 5, height, 1, 30, height + 1, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }

                    ps = sub_98197C(ebx + 1, 31, 0, 3, 29, 2, height, 1, 30, height + ah - 8, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }
                } else {
                    paint_struct * ps;

                    ps = sub_98197C(ebx, 31, 0, 1, 29, ah, height, 30, 2, height + 1, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }

                    ps = sub_98199C(ebx + 1, 31, 0, 1, 29, ah, height, 30, 2, height + 1, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }
                }
                break;

            case 3:
                ebx = dword_141F70C->image + byte_9A40CC[ebx];
                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG1) {
                    ebx |= dword_141F714;
                }

                if (dword_141F710 != 0) {
                    ebx = (ebx & 0x7FFFF) | dword_141F710;
                }

                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG4) {
                    paint_struct * ps;

                    ps = sub_98197C(ebx, 2, 1, 3, 1, ah - 5, height, 1, 1, height + 1, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }

                    ps = sub_98197C(ebx + 1, 2, 1, 28, 1, 3, height, 1, 1, height + ah - 5 - 4, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }
                } else {
                    paint_struct * ps;

                    ps = sub_98197C(ebx, 2, 1, 28, 1, ah, height, 1, 1, height + 1, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }

                    ps = sub_98199C(ebx + 1, 2, 1, 28, 1, ah, height, 1, 1, height + 1, get_current_rotation());
                    if (ps != NULL) {
                        ps->tertiary_colour = dword_141F71C;
                    }
                }
                break;
        }

        return;
    }


    uint8 al = map_element->type;

    switch (direction) {
        case 0: {
            uint32 offset;
            if (al & 0x80) {
                offset = 3;
            } else if (al & 0x40) {
                offset = 5;
            } else {
                offset = 1;
            }

            uint32 baseImageId = offset + dword_141F70C->image + dword_141F708;

            uint32 imageId = baseImageId;
            if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG2) {
                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG1) {
                    imageId |= dword_141F714;
                }

                if (dword_141F710 != 0) {
                    imageId = (imageId & 0x7FFFF) | dword_141F710;
                }

                sub_98197C(imageId, 0, 0, 1, 28, ah, height, 1, 1, height + 1, get_current_rotation());
                if (dword_141F710 == 0) {
                    imageId = baseImageId + dword_141F718;
                    sub_98199C(imageId, 0, 0, 1, 28, ah, height, 1, 1, height + 1, get_current_rotation());
                }
            } else {
                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG1) {
                    imageId |= dword_141F714;
                }

                if (dword_141F710 != 0) {
                    imageId = (imageId & 0x7FFFF) | dword_141F710;
                }

                paint_struct * paint = sub_98197C(imageId, 0, 0, 1, 28, ah, height, 1, 1, height + 1, get_current_rotation());
                if (paint != NULL) {
                    paint->tertiary_colour = dword_141F71C;
                }
            }

            break;
        }

        case 1: {
            uint32 offset;
            if (al & 0x80) {
                offset = 2;
            } else if (al & 0x40) {
                offset = 4;
            } else {
                offset = 0;
            }

            if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG2) {
                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG4) {
                    offset += 12;
                }

                uint32 baseImageId = offset + dword_141F70C->image;
                uint32 imageId = baseImageId;

                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG1) {
                    imageId |= dword_141F714;
                }

                if (dword_141F710 != 0) {
                    imageId = (imageId & 0x7FFFF) | dword_141F710;
                }

                sub_98197C(imageId, 1, 31, 29, 1, ah, height, 2, 30, height + 1, get_current_rotation());
                if (dword_141F710 == 0) {
                    imageId = baseImageId + dword_141F718;
                    sub_98199C(imageId, 1, 31, 29, 1, ah, height, 2, 30, height + 1, get_current_rotation());
                }
            } else {
                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG4) {
                    offset += 6;
                }

                uint32 imageId = offset + dword_141F70C->image + dword_141F708;

                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG1) {
                    imageId |= dword_141F714;
                }

                if (dword_141F710 != 0) {
                    imageId = (imageId & 0x7FFFF) | dword_141F710;
                }

                paint_struct * paint = sub_98197C(imageId, 1, 31, 29, 1, ah, height, 2, 30, height + 1, get_current_rotation());
                if (paint != NULL) {
                    paint->tertiary_colour = dword_141F71C;
                }
            }

            break;
        }

        case 2: {
            uint32 offset;
            if (al & 0x80) {
                offset = 5;
            } else if (al & 0x40) {
                offset = 3;
            } else {
                offset = 1;
            }

            if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG4) {
                offset += 6;
            }

            uint32 baseImageId = offset + dword_141F70C->image;
            uint32 imageId = baseImageId;

            if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG2) {
                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG1) {
                    imageId |= dword_141F714;
                }

                if (dword_141F710 != 0) {
                    imageId = (imageId & 0x7FFFF) | dword_141F710;
                }

                sub_98197C(imageId, 31, 0, 1, 29, ah, height, 30, 2, height + 1, get_current_rotation());
                if (dword_141F710 == 0) {
                    imageId = baseImageId + dword_141F718;
                    sub_98199C(imageId, 31, 0, 1, 29, ah, height, 30, 2, height + 1, get_current_rotation());
                }
            } else {
                uint32 imageId = imageId + dword_141F708;

                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG1) {
                    imageId |= dword_141F714;
                }

                if (dword_141F710 != 0) {
                    imageId = (imageId & 0x7FFFF) | dword_141F710;
                }

                paint_struct * paint = sub_98197C(imageId, 31, 0, 1, 29, ah, height, 30, 2, height + 1, get_current_rotation());
                if (paint != NULL) {
                    paint->tertiary_colour = dword_141F71C;
                }
            }

            break;
        }

        case 3: {
            uint32 offset;
            if (al & 0x80) {
                offset = 4;
            } else if (al & 0x40) {
                offset = 2;
            } else {
                offset = 0;
            }

            uint32 baseImageId = offset + dword_141F70C->image + dword_141F708;
            uint32 imageId = baseImageId;

            if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG2) {
                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG1) {
                    imageId |= dword_141F714;
                }

                if (dword_141F710 != 0) {
                    imageId = (imageId & 0x7FFFF) | dword_141F710;
                }

                sub_98197C(imageId, 2, 1, 28, 1, ah, height, 1, 1, height + 1, get_current_rotation());
                if (dword_141F710 == 0) {
                    imageId = baseImageId + dword_141F718;
                    sub_98199C(imageId, 2, 1, 28, 1, ah, height, 1, 1, height + 1, get_current_rotation());
                }
            } else {
                if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG1) {
                    imageId |= dword_141F714;
                }

                if (dword_141F710 != 0) {
                    imageId = (imageId & 0x7FFFF) | dword_141F710;
                }

                paint_struct * paint = sub_98197C(imageId, 2, 1, 28, 1, ah, height, 1, 1, height + 1, get_current_rotation());
                if (paint != NULL) {
                    paint->tertiary_colour = dword_141F71C;
                }
            }

            break;
        }
    }

    if (dword_141F70C->wall.var_0D == 0xFF) {
        return;
    }

    if (direction != 0 && direction != 3) {
        return;
    }

    if (dword_141F70C->wall.flags & WALL_SCENERY_FLAG1) {
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

    uint16 scrollingMode = dword_141F70C->wall.var_0D + (get_current_rotation() + 1) & 0x3;

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

