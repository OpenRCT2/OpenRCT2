/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../common.h"
#include "../../config/Config.h"
#include "../../drawing/Drawing.h"
#include "../../Game.h"
#include "../../localisation/Localisation.h"
#include "../../interface/Colour.h"
#include "../../interface/Viewport.h"
#include "../../ride/Track.h"
#include "../../ride/TrackDesign.h"
#include "../../world/Banner.h"
#include "../../world/Map.h"
#include "../../world/Scenery.h"
#include "../../world/Wall.h"
#include "../Paint.h"
#include "Paint.TileElement.h"

static constexpr const uint8_t byte_9A406C[] = {
    2, 2, 22, 26, 30, 34, 34, 34, 34, 34, 30, 26, 22, 2, 6, 2,
    2, 2, 6, 10, 14, 18, 18, 18, 18, 18, 14, 10, 6, 2, 22, 2
};

static constexpr const uint8_t byte_9A408C[] = {
    0, 0, 4, 8, 12, 16, 16, 16, 16, 16, 12, 8, 4, 0, 20, 0,
    0, 0, 20, 24, 28, 32, 32, 32, 32, 32, 28, 24, 20, 0, 4, 0
};

static constexpr const uint8_t byte_9A40AC[] = {
    2, 2, 6, 10, 14, 18, 18, 18, 18, 18, 14, 10, 6, 2, 22, 2,
    2, 2, 22, 26, 30, 34, 34, 34, 34, 34, 30, 26, 22, 2, 6, 2
};

static constexpr const uint8_t byte_9A40CC[] = {
    0, 0, 20, 24, 28, 32, 32, 32, 32, 32, 28, 24, 20, 0, 4, 0,
    0, 0, 4, 8, 12, 16, 16, 16, 16, 16, 12, 8, 4, 0, 20, 0
};

static void fence_paint_door(paint_session * session, uint32_t imageId,
                      rct_scenery_entry * sceneryEntry,
                      uint32_t imageColourFlags, uint32_t tertiaryColour, uint32_t dword_141F710,
                      LocationXYZ16 offset,
                      LocationXYZ16 boundsR1, LocationXYZ16 boundsR1_,
                      LocationXYZ16 boundsR2, LocationXYZ16 boundsR2_,
                      LocationXYZ16 boundsL1, LocationXYZ16 boundsL1_)
{
    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_PRIMARY_COLOUR) {
        imageId |= imageColourFlags;
    }

    if (dword_141F710 != 0) {
        imageId = (imageId & 0x7FFFF) | dword_141F710;
    }

    if (sceneryEntry->wall.flags & WALL_SCENERY_IS_BANNER) {
        paint_struct * ps;

        ps = sub_98197C(
            session, imageId, (int8_t)offset.x, (int8_t)offset.y, boundsR1.x, boundsR1.y, (int8_t)boundsR1.z, offset.z,
            boundsR1_.x, boundsR1_.y, boundsR1_.z);
        if (ps != nullptr) {
            ps->tertiary_colour = tertiaryColour;
        }

        ps = sub_98197C(
            session, imageId + 1, (int8_t)offset.x, (int8_t)offset.y, boundsR2.x, boundsR2.y, (int8_t)boundsR2.z, offset.z,
            boundsR2_.x, boundsR2_.y, boundsR2_.z);
        if (ps != nullptr) {
            ps->tertiary_colour = tertiaryColour;
        }
    } else {
        paint_struct * ps;

        ps = sub_98197C(
            session, imageId, (int8_t)offset.x, (int8_t)offset.y, boundsL1.x, boundsL1.y, (int8_t)boundsL1.z, offset.z,
            boundsL1_.x, boundsL1_.y, boundsL1_.z);
        if (ps != nullptr) {
            ps->tertiary_colour = tertiaryColour;
        }

        ps = sub_98199C(
            session, imageId + 1, (int8_t)offset.x, (int8_t)offset.y, boundsL1.x, boundsL1.y, (int8_t)boundsL1.z, offset.z,
            boundsL1_.x, boundsL1_.y, boundsL1_.z);
        if (ps != nullptr) {
            ps->tertiary_colour = tertiaryColour;
        }
    }
}

static void fence_paint_wall(paint_session * session, uint32_t frameNum, const rct_scenery_entry * sceneryEntry, uint32_t dword_141F710, uint32_t imageColourFlags, uint32_t dword_141F718, uint32_t tertiaryColour, uint32_t imageOffset, LocationXYZ16 offset, LocationXYZ16 bounds, LocationXYZ16 boundsOffset)
{
    uint32_t baseImageId = sceneryEntry->image + imageOffset + frameNum;
    uint32_t imageId = baseImageId;


    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_GLASS) {
        if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_PRIMARY_COLOUR) {
            imageId |= imageColourFlags;
        }

        if (dword_141F710 != 0) {
            imageId = (imageId & 0x7FFFF) | dword_141F710;
        }

        sub_98197C(
            session, imageId, (int8_t)offset.x, (int8_t)offset.y, bounds.x, bounds.y, (int8_t)bounds.z, offset.z, boundsOffset.x,
            boundsOffset.y, boundsOffset.z);
        if (dword_141F710 == 0) {
            imageId = baseImageId + dword_141F718;
            sub_98199C(
                session, imageId, (int8_t)offset.x, (int8_t)offset.y, bounds.x, bounds.y, (int8_t)bounds.z, offset.z,
                boundsOffset.x, boundsOffset.y, boundsOffset.z);
        }
    } else {
        if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_PRIMARY_COLOUR) {
            imageId |= imageColourFlags;
        }

        if (dword_141F710 != 0) {
            imageId = (imageId & 0x7FFFF) | dword_141F710;
        }

        paint_struct * paint = sub_98197C(
            session, imageId, (int8_t)offset.x, (int8_t)offset.y, bounds.x, bounds.y, (int8_t)bounds.z, offset.z, boundsOffset.x,
            boundsOffset.y, boundsOffset.z);
        if (paint != nullptr) {
            paint->tertiary_colour = tertiaryColour;
        }
    }
}
/**
 * rct2: 0x006E44B0
 * @param direction (cl)
 * @param height (dx)
 * @param tile_element (esi)
 */
void fence_paint(paint_session * session, uint8_t direction, int32_t height, const rct_tile_element * tile_element)
{
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_WALL;

    rct_scenery_entry * sceneryEntry = get_wall_entry(tile_element->properties.wall.type);
    if (sceneryEntry == nullptr) {
        return;
    }
    uint32_t frameNum = 0;

    if (sceneryEntry->wall.flags2 & WALL_SCENERY_2_ANIMATED) {
        frameNum = (gCurrentTicks & 7) * 2;
    }


    int32_t primaryColour = wall_get_primary_colour(tile_element);
    uint32_t imageColourFlags = primaryColour << 19 | IMAGE_TYPE_REMAP;
    uint32_t dword_141F718 = imageColourFlags + 0x23800006;

    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR) {
        uint8_t secondaryColour = wall_get_secondary_colour(tile_element);
        imageColourFlags |= secondaryColour << 24 | IMAGE_TYPE_REMAP_2_PLUS;
    }

    uint32_t tertiaryColour = 0;
    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR) {
        tertiaryColour = wall_get_tertiary_colour(tile_element);
        imageColourFlags &= 0x0DFFFFFFF;
    }

    paint_util_set_general_support_height(session, height, 0x20);

    uint32_t dword_141F710 = 0;
    if (gTrackDesignSaveMode || (gCurrentViewportFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)) {
        if (!track_design_save_contains_tile_element(tile_element)) {
            dword_141F710 = SPRITE_ID_PALETTE_COLOUR_1(PALETTE_46);
        }
    }

    if (tile_element->flags & TILE_ELEMENT_FLAG_GHOST) {
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
        dword_141F710 = CONSTRUCTION_MARKER;
    }

    // Save tile_element

    uint8_t ah = sceneryEntry->wall.height * 8 - 2;


    if (sceneryEntry->wall.flags & WALL_SCENERY_IS_DOOR) {
        LocationXYZ16 offset;
        LocationXYZ16 boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_;
        uint8_t animationFrame = wall_get_animation_frame(tile_element);
        // Add the direction as well
        animationFrame |= (tile_element->properties.wall.animation & WALL_ANIMATION_FLAG_DIRECTION_BACKWARD) >> 3;
        uint32_t imageId;
        switch (direction) {
            case 0:
                imageId = sceneryEntry->image + byte_9A406C[animationFrame];

                boundsR1 = {1, 3, static_cast<int16_t>(ah - 5)};
                boundsR1_ = {1, 1, static_cast<int16_t>(height + 1)};
                boundsR2 = {1, 28, 3};
                boundsR2_ = {1, 1, static_cast<int16_t>(height + ah - 9)};

                boundsL1 = {1, 28, ah};
                boundsL1_ = {1, 1, static_cast<int16_t>(height + 1)};

                offset = {0, 0, static_cast<int16_t>(height)};

                fence_paint_door(session, imageId, sceneryEntry, imageColourFlags, tertiaryColour, dword_141F710, offset, boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;

            case 1:
                imageId = sceneryEntry->image + byte_9A408C[animationFrame];

                boundsR1 = {3, 3, static_cast<int16_t>(ah - 5)};
                boundsR1_ = {1, 30, static_cast<int16_t>(height + 1)};
                boundsR2 = {29, 3, 2};
                boundsR2_ = {1, 30, static_cast<int16_t>(height + ah - 8)};

                boundsL1 = {29, 1, ah};
                boundsL1_ = {2, 30, static_cast<int16_t>(height + 1)};

                offset = {1, 31, static_cast<int16_t>(height)};

                fence_paint_door(session, imageId, sceneryEntry, imageColourFlags, tertiaryColour, dword_141F710, offset, boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;

            case 2:
                imageId = sceneryEntry->image + byte_9A40AC[animationFrame];

                boundsR1 = {3, 3, static_cast<int16_t>(ah - 5)};
                boundsR1_ = {30, 1, static_cast<int16_t>(height + 1)};
                boundsR2 = {3, 29, 2};
                boundsR2_ = {30, 1, static_cast<int16_t>(height + ah - 8)};

                boundsL1 = {1, 29, ah};
                boundsL1_ = {30, 2, static_cast<int16_t>(height + 1)};

                offset = {31, 0, static_cast<int16_t>(height)};

                fence_paint_door(session, imageId, sceneryEntry, imageColourFlags, tertiaryColour, dword_141F710, offset, boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;

            case 3:
                imageId = sceneryEntry->image + byte_9A40CC[animationFrame];

                boundsR1 = {3, 1, static_cast<int16_t>(ah - 5)};
                boundsR1_ = {1, 1, static_cast<int16_t>(height + 1)};
                boundsR2 = {28, 1, 3};
                boundsR2_ = {1, 1, static_cast<int16_t>(height + ah - 9)};

                boundsL1 = {28, 1, ah};
                boundsL1_ = {1, 1, static_cast<int16_t>(height + 1)};

                offset = {2, 1, static_cast<int16_t>(height)};

                fence_paint_door(session, imageId, sceneryEntry, imageColourFlags, tertiaryColour, dword_141F710, offset, boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;
        }

        return;
    }


    uint32_t imageOffset = 0;
    LocationXYZ16 offset = { 0, 0, 0 }, bounds = { 0, 0, 0 }, boundsOffset = { 0, 0, 0 };

    switch (direction) {
        case 0:
            if (tile_element->type & 0x80) {
                imageOffset = 3;
            } else if (tile_element->type & 0x40) {
                imageOffset = 5;
            } else {
                imageOffset = 1;
            }

            offset = {0, 0, static_cast<int16_t>(height)};
            bounds = {1, 28, ah};
            boundsOffset = {1, 1, static_cast<int16_t>(height + 1)};
            break;

        case 1:
            if (tile_element->type & 0x80) {
                imageOffset = 2;
            } else if (tile_element->type & 0x40) {
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

            offset = {1, 31, static_cast<int16_t>(height)};
            bounds = {29, 1, ah};
            boundsOffset = {2, 30, static_cast<int16_t>(height + 1)};
            break;

        case 2:
            if (tile_element->type & 0x80) {
                imageOffset = 5;
            } else if (tile_element->type & 0x40) {
                imageOffset = 3;
            } else {
                imageOffset = 1;
            }

            if (sceneryEntry->wall.flags & WALL_SCENERY_IS_BANNER) {
                imageOffset += 6;
            }

            offset = {31, 0, static_cast<int16_t>(height)};
            bounds = {1, 29, ah};
            boundsOffset = {30, 2, static_cast<int16_t>(height + 1)};
            break;

        case 3:
            if (tile_element->type & 0x80) {
                imageOffset = 4;
            } else if (tile_element->type & 0x40) {
                imageOffset = 2;
            } else {
                imageOffset = 0;
            }

            offset = {2, 1, static_cast<int16_t>(height)};
            bounds = {28, 1, ah};
            boundsOffset = {1, 1, static_cast<int16_t>(height + 1)};
            break;
    }

    fence_paint_wall(session, frameNum, sceneryEntry, dword_141F710, imageColourFlags, dword_141F718, tertiaryColour, imageOffset, offset, bounds, boundsOffset);


    if (sceneryEntry->wall.scrolling_mode == 0xFF) {
        return;
    }

    if (direction != 0 && direction != 3) {
        return;
    }

    set_format_arg(0, uint32_t, 0);
    set_format_arg(4, uint32_t, 0);

    uint8_t secondaryColour = wall_get_secondary_colour(tile_element);

    if (dword_141F710 != 0) {
        secondaryColour = COLOUR_GREY;
    }

    if (direction == 0) {
        secondaryColour |= 0x80;
    }

    set_format_arg(7, uint8_t, secondaryColour);

    uint16_t scrollingMode = sceneryEntry->wall.scrolling_mode + ((direction + 1) & 0x3);

    uint8_t bannerIndex = tile_element->properties.wall.banner_index;
    rct_banner * banner = &gBanners[bannerIndex];

    set_format_arg(0, rct_string_id, banner->string_idx);
    if (banner->flags & BANNER_FLAG_LINKED_TO_RIDE)
    {
        Ride * ride = get_ride(banner->ride_index);
        set_format_arg(0, rct_string_id, ride->name);
        set_format_arg(2, uint32_t, ride->name_arguments);
    }

    utf8 signString[256];
    rct_string_id stringId = STR_SCROLLING_SIGN_TEXT;
    if (gConfigGeneral.upper_case_banners) {
        format_string_to_upper(signString, sizeof(signString), stringId, gCommonFormatArgs);
    } else {
        format_string(signString, sizeof(signString), stringId, gCommonFormatArgs);
    }

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_TINY;

    uint16_t string_width = gfx_get_string_width(signString);
    uint16_t scroll = (gCurrentTicks / 2) % string_width;

    sub_98199C(
        session, scrolling_text_setup(session, stringId, scroll, scrollingMode), 0, 0, 1, 1, 13, height + 8, boundsOffset.x,
        boundsOffset.y, boundsOffset.z);
}
