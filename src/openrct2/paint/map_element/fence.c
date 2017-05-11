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
#include "../../world/map.h"
#include "../../drawing/drawing.h"
#include "../../world/scenery.h"
#include "../../game.h"
#include "../../ride/track.h"
#include "../../config/Config.h"
#include "../../localisation/localisation.h"
#include "../../interface/colour.h"
#include "../../interface/viewport.h"
#include "../../paint/map_element/map_element.h"
#include "../paint.h"

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

static void fence_paint_door(uint32 imageId,
                      rct_scenery_entry * sceneryEntry,
                      uint32 imageColourFlags, uint32 tertiaryColour, uint32 dword_141F710,
                      rct_xyz16 offset,
                      rct_xyz16 boundsR1, rct_xyz16 boundsR1_,
                      rct_xyz16 boundsR2, rct_xyz16 boundsR2_,
                      rct_xyz16 boundsL1, rct_xyz16 boundsL1_)
{
    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_PRIMARY_COLOUR) {
        imageId |= imageColourFlags;
    }

    if (dword_141F710 != 0) {
        imageId = (imageId & 0x7FFFF) | dword_141F710;
    }

    if (sceneryEntry->wall.flags & WALL_SCENERY_IS_BANNER) {
        paint_struct * ps;

        ps = sub_98197C(imageId, (sint8) offset.x, (sint8) offset.y, boundsR1.x, boundsR1.y, (sint8) boundsR1.z, offset.z, boundsR1_.x, boundsR1_.y, boundsR1_.z, get_current_rotation());
        if (ps != NULL) {
            ps->tertiary_colour = tertiaryColour;
        }

        ps = sub_98197C(imageId + 1, (sint8) offset.x, (sint8) offset.y, boundsR2.x, boundsR2.y, (sint8) boundsR2.z, offset.z, boundsR2_.x, boundsR2_.y, boundsR2_.z, get_current_rotation());
        if (ps != NULL) {
            ps->tertiary_colour = tertiaryColour;
        }
    } else {
        paint_struct * ps;

        ps = sub_98197C(imageId, (sint8) offset.x, (sint8) offset.y, boundsL1.x, boundsL1.y, (sint8) boundsL1.z, offset.z, boundsL1_.x, boundsL1_.y, boundsL1_.z, get_current_rotation());
        if (ps != NULL) {
            ps->tertiary_colour = tertiaryColour;
        }

        ps = sub_98199C(imageId + 1, (sint8) offset.x, (sint8) offset.y, boundsL1.x, boundsL1.y, (sint8) boundsL1.z, offset.z, boundsL1_.x, boundsL1_.y, boundsL1_.z, get_current_rotation());
        if (ps != NULL) {
            ps->tertiary_colour = tertiaryColour;
        }
    }
}

static void fence_paint_wall(uint32 frameNum, const rct_scenery_entry * sceneryEntry, uint32 dword_141F710, uint32 imageColourFlags, uint32 dword_141F718, uint32 tertiaryColour, uint32 imageOffset, rct_xyz16 offset, rct_xyz16 bounds, rct_xyz16 boundsOffset)
{
    uint32 baseImageId = sceneryEntry->image + imageOffset + frameNum;
    uint32 imageId = baseImageId;


    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_GLASS) {
        if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_PRIMARY_COLOUR) {
            imageId |= imageColourFlags;
        }

        if (dword_141F710 != 0) {
            imageId = (imageId & 0x7FFFF) | dword_141F710;
        }

        sub_98197C(imageId, (sint8)offset.x, (sint8)offset.y, bounds.x, bounds.y, (sint8)bounds.z, offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z, get_current_rotation());
        if (dword_141F710 == 0) {
            imageId = baseImageId + dword_141F718;
            sub_98199C(imageId, (sint8)offset.x, (sint8)offset.y, bounds.x, bounds.y, (sint8)bounds.z, offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z, get_current_rotation());
        }
    } else {
        if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_PRIMARY_COLOUR) {
            imageId |= imageColourFlags;
        }

        if (dword_141F710 != 0) {
            imageId = (imageId & 0x7FFFF) | dword_141F710;
        }

        paint_struct * paint = sub_98197C(imageId, (sint8)offset.x, (sint8)offset.y, bounds.x, bounds.y, (sint8)bounds.z, offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z, get_current_rotation());
        if (paint != NULL) {
            paint->tertiary_colour = tertiaryColour;
        }
    }
}
/**
 * rct2: 0x006E44B0
 * @param direction (cl)
 * @param height (dx)
 * @param map_element (esi)
 */
void fence_paint(uint8 direction, sint32 height, rct_map_element * map_element)
{
    gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_WALL;

    rct_scenery_entry * sceneryEntry = get_wall_entry(map_element->properties.wall.type);
    if (sceneryEntry == NULL || sceneryEntry == (rct_scenery_entry *)-1) {
        return;
    }
    uint32 frameNum = 0;

    if (sceneryEntry->wall.flags2 & WALL_SCENERY_2_FLAG5) {
        frameNum = (gCurrentTicks & 7) * 2;
    }


    sint32 primaryColour = map_element->properties.wall.colour_1 & 0x1F;
    uint32 imageColourFlags = primaryColour << 19 | 0x20000000;
    uint32 dword_141F718 = imageColourFlags + 0x23800006;

    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
        uint8 secondaryColour = wall_element_get_secondary_colour(map_element);
        imageColourFlags |= secondaryColour << 24 | 0x80000000;
    }

    uint32 tertiaryColour = 0;
    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR) {
        tertiaryColour = map_element->properties.wall.colour_3;
        imageColourFlags &= 0x0DFFFFFFF;
    }

    paint_util_set_general_support_height(height, 0x20);

    uint32 dword_141F710 = 0;
    if (gTrackDesignSaveMode) {
        if (!track_design_save_contains_map_element(map_element)) {
            dword_141F710 = 0x21700000;
        }
    }

    if (map_element->flags & MAP_ELEMENT_FLAG_GHOST) {
        gPaintInteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
        dword_141F710 = construction_markers[gConfigGeneral.construction_marker_colour];
    }

    // Save map_element

    uint8 ah = sceneryEntry->wall.height * 8 - 2;


    if (sceneryEntry->wall.flags & WALL_SCENERY_IS_DOOR) {
        rct_xyz16 offset;
        rct_xyz16 boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_;
        uint8 animationFrame = wall_element_get_animation_frame(map_element);
        // Add the direction as well
        animationFrame |= (map_element->properties.wall.animation & WALL_ANIMATION_FLAG_DIRECTION_BACKWARD) >> 3;
        uint32 imageId;
        switch (direction) {
            case 0:
                imageId = sceneryEntry->image + byte_9A406C[animationFrame];

                boundsR1 = (rct_xyz16) {1, 3, ah - 5};
                boundsR1_ = (rct_xyz16) {1, 1, height + 1};
                boundsR2 = (rct_xyz16) {1, 28, 3};
                boundsR2_ = (rct_xyz16) {1, 1, height + ah - 9};

                boundsL1 = (rct_xyz16) {1, 28, ah};
                boundsL1_ = (rct_xyz16) {1, 1, height + 1};

                offset = (rct_xyz16) {0, 0, height};

                fence_paint_door(imageId, sceneryEntry, imageColourFlags, tertiaryColour, dword_141F710, offset, boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;

            case 1:
                imageId = sceneryEntry->image + byte_9A408C[animationFrame];

                boundsR1 = (rct_xyz16) {3, 3, ah - 5};
                boundsR1_ = (rct_xyz16) {1, 30, height + 1};
                boundsR2 = (rct_xyz16) {29, 3, 2};
                boundsR2_ = (rct_xyz16) {1, 30, height + ah - 8};

                boundsL1 = (rct_xyz16) {29, 1, ah};
                boundsL1_ = (rct_xyz16) {2, 30, height + 1};

                offset = (rct_xyz16) {1, 31, height};

                fence_paint_door(imageId, sceneryEntry, imageColourFlags, tertiaryColour, dword_141F710, offset, boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;

            case 2:
                imageId = sceneryEntry->image + byte_9A40AC[animationFrame];

                boundsR1 = (rct_xyz16) {3, 3, ah - 5};
                boundsR1_ = (rct_xyz16) {30, 1, height + 1};
                boundsR2 = (rct_xyz16) {3, 29, 2};
                boundsR2_ = (rct_xyz16) {30, 1, height + ah - 8};

                boundsL1 = (rct_xyz16) {1, 29, ah};
                boundsL1_ = (rct_xyz16) {30, 2, height + 1};

                offset = (rct_xyz16) {31, 0, height};

                fence_paint_door(imageId, sceneryEntry, imageColourFlags, tertiaryColour, dword_141F710, offset, boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;

            case 3:
                imageId = sceneryEntry->image + byte_9A40CC[animationFrame];

                boundsR1 = (rct_xyz16) {3, 1, ah - 5};
                boundsR1_ = (rct_xyz16) {1, 1, height + 1};
                boundsR2 = (rct_xyz16) {28, 1, 3};
                boundsR2_ = (rct_xyz16) {1, 1, height + ah - 9};

                boundsL1 = (rct_xyz16) {28, 1, ah};
                boundsL1_ = (rct_xyz16) {1, 1, height + 1};

                offset = (rct_xyz16) {2, 1, height};

                fence_paint_door(imageId, sceneryEntry, imageColourFlags, tertiaryColour, dword_141F710, offset, boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;
        }

        return;
    }


    uint32 imageOffset = 0;
    rct_xyz16 offset = { 0, 0, 0 }, bounds = { 0, 0, 0 }, boundsOffset = { 0, 0, 0 };

    switch (direction) {
        case 0:
            if (map_element->type & 0x80) {
                imageOffset = 3;
            } else if (map_element->type & 0x40) {
                imageOffset = 5;
            } else {
                imageOffset = 1;
            }

            offset = (rct_xyz16){0, 0, height};
            bounds = (rct_xyz16){1, 28, ah};
            boundsOffset = (rct_xyz16){1, 1, height + 1};
			break;

        case 1:
            if (map_element->type & 0x80) {
                imageOffset = 2;
            } else if (map_element->type & 0x40) {
                imageOffset = 4;
            } else {
                imageOffset = 0;
            }

            if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_GLASS) {
                if (sceneryEntry->wall.flags & WALL_SCENERY_IS_BANNER) {
                    imageOffset += 12;
                }
            } else {
                if (sceneryEntry->wall.flags & WALL_SCENERY_IS_BANNER) {
                    imageOffset += 6;
                }
            }

            offset = (rct_xyz16){1, 31, height};
            bounds = (rct_xyz16){29, 1, ah};
            boundsOffset = (rct_xyz16){2, 30, height + 1};
            break;

        case 2:
            if (map_element->type & 0x80) {
                imageOffset = 5;
            } else if (map_element->type & 0x40) {
                imageOffset = 3;
            } else {
                imageOffset = 1;
            }

            if (sceneryEntry->wall.flags & WALL_SCENERY_IS_BANNER) {
                imageOffset += 6;
            }

            offset = (rct_xyz16){31, 0, height};
            bounds = (rct_xyz16){1, 29, ah};
            boundsOffset = (rct_xyz16){30, 2, height + 1};
            break;

        case 3:
            if (map_element->type & 0x80) {
                imageOffset = 4;
            } else if (map_element->type & 0x40) {
                imageOffset = 2;
            } else {
                imageOffset = 0;
            }

            offset = (rct_xyz16){2, 1, height};
            bounds = (rct_xyz16){28, 1, ah};
            boundsOffset = (rct_xyz16){1, 1, height + 1};
            break;
    }

    fence_paint_wall(frameNum, sceneryEntry, dword_141F710, imageColourFlags, dword_141F718, tertiaryColour, imageOffset, offset, bounds, boundsOffset);


    if (sceneryEntry->wall.scrolling_mode == 0xFF) {
        return;
    }

    if (direction != 0 && direction != 3) {
        return;
    }

    set_format_arg(0, uint32, 0);
    set_format_arg(4, uint32, 0);

    uint8 secondaryColour = wall_element_get_secondary_colour(map_element);

    if (dword_141F710 != 0) {
        secondaryColour = COLOUR_GREY;
    }

    if (direction == 0) {
        secondaryColour |= 0x80;
    }

    set_format_arg(7, uint8, secondaryColour);

    uint16 scrollingMode = sceneryEntry->wall.scrolling_mode + ((direction + 1) & 0x3);

    uint8 bannerIndex = map_element->properties.wall.banner_index;
    rct_banner * banner = &gBanners[bannerIndex];

    set_format_arg(0, rct_string_id, banner->string_idx);
    if (banner->flags & BANNER_FLAG_LINKED_TO_RIDE) {
        rct_ride * ride = get_ride(banner->colour);
        set_format_arg(0, rct_string_id, ride->name);
        set_format_arg(2, uint32, ride->name_arguments);
    }

    utf8 signString[MAX_PATH];
    rct_string_id stringId = STR_SCROLLING_SIGN_TEXT;
    if (gConfigGeneral.upper_case_banners) {
        format_string_to_upper(signString, MAX_PATH, stringId, gCommonFormatArgs);
    } else {
        format_string(signString, MAX_PATH, stringId, gCommonFormatArgs);
    }

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_TINY;

    uint16 string_width = gfx_get_string_width(signString);
    uint16 scroll = (gCurrentTicks / 2) % string_width;

    sub_98199C(scrolling_text_setup(stringId, scroll, scrollingMode), 0, 0, 1, 1, 13, height + 8, boundsOffset.x, boundsOffset.y, boundsOffset.z, get_current_rotation());
}
