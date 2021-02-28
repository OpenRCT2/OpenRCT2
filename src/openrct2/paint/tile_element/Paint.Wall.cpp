/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../Game.h"
#include "../../common.h"
#include "../../config/Config.h"
#include "../../drawing/Drawing.h"
#include "../../interface/Colour.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Localisation.h"
#include "../../ride/Track.h"
#include "../../ride/TrackDesign.h"
#include "../../world/Banner.h"
#include "../../world/Map.h"
#include "../../world/Scenery.h"
#include "../../world/Wall.h"
#include "../Paint.h"
#include "Paint.TileElement.h"

static constexpr const uint8_t byte_9A406C[] = {
    2, 2, 22, 26, 30, 34, 34, 34, 34, 34, 30, 26, 22, 2, 6, 2, 2, 2, 6, 10, 14, 18, 18, 18, 18, 18, 14, 10, 6, 2, 22, 2,
};

static constexpr const uint8_t byte_9A408C[] = {
    0, 0, 4, 8, 12, 16, 16, 16, 16, 16, 12, 8, 4, 0, 20, 0, 0, 0, 20, 24, 28, 32, 32, 32, 32, 32, 28, 24, 20, 0, 4, 0,
};

static constexpr const uint8_t byte_9A40AC[] = {
    2, 2, 6, 10, 14, 18, 18, 18, 18, 18, 14, 10, 6, 2, 22, 2, 2, 2, 22, 26, 30, 34, 34, 34, 34, 34, 30, 26, 22, 2, 6, 2,
};

static constexpr const uint8_t byte_9A40CC[] = {
    0, 0, 20, 24, 28, 32, 32, 32, 32, 32, 28, 24, 20, 0, 4, 0, 0, 0, 4, 8, 12, 16, 16, 16, 16, 16, 12, 8, 4, 0, 20, 0,
};

static void fence_paint_door(
    paint_session* session, uint32_t imageId, rct_scenery_entry* sceneryEntry, uint32_t imageColourFlags,
    uint32_t tertiaryColour, uint32_t dword_141F710, LocationXYZ16 offset, LocationXYZ16 boundsR1, LocationXYZ16 boundsR1_,
    LocationXYZ16 boundsR2, LocationXYZ16 boundsR2_, LocationXYZ16 boundsL1, LocationXYZ16 boundsL1_)
{
    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_PRIMARY_COLOUR)
    {
        imageId |= imageColourFlags;
    }

    if (dword_141F710 != 0)
    {
        imageId = (imageId & 0x7FFFF) | dword_141F710;
    }

    if (sceneryEntry->wall.flags & WALL_SCENERY_IS_BANNER)
    {
        paint_struct* ps;

        ps = PaintAddImageAsParent(
            session, imageId, static_cast<int8_t>(offset.x), static_cast<int8_t>(offset.y), boundsR1.x, boundsR1.y,
            static_cast<int8_t>(boundsR1.z), offset.z, boundsR1_.x, boundsR1_.y, boundsR1_.z);
        if (ps != nullptr)
        {
            ps->tertiary_colour = tertiaryColour;
        }

        ps = PaintAddImageAsParent(
            session, imageId + 1, static_cast<int8_t>(offset.x), static_cast<int8_t>(offset.y), boundsR2.x, boundsR2.y,
            static_cast<int8_t>(boundsR2.z), offset.z, boundsR2_.x, boundsR2_.y, boundsR2_.z);
        if (ps != nullptr)
        {
            ps->tertiary_colour = tertiaryColour;
        }
    }
    else
    {
        paint_struct* ps;

        ps = PaintAddImageAsParent(
            session, imageId, static_cast<int8_t>(offset.x), static_cast<int8_t>(offset.y), boundsL1.x, boundsL1.y,
            static_cast<int8_t>(boundsL1.z), offset.z, boundsL1_.x, boundsL1_.y, boundsL1_.z);
        if (ps != nullptr)
        {
            ps->tertiary_colour = tertiaryColour;
        }

        ps = PaintAddImageAsChild(
            session, imageId + 1, static_cast<int8_t>(offset.x), static_cast<int8_t>(offset.y), boundsL1.x, boundsL1.y,
            static_cast<int8_t>(boundsL1.z), offset.z, boundsL1_.x, boundsL1_.y, boundsL1_.z);
        if (ps != nullptr)
        {
            ps->tertiary_colour = tertiaryColour;
        }
    }
}

static void fence_paint_wall(
    paint_session* session, uint32_t frameNum, const rct_scenery_entry* sceneryEntry, uint32_t dword_141F710,
    uint32_t imageColourFlags, uint32_t dword_141F718, uint32_t tertiaryColour, uint32_t imageOffset, LocationXYZ16 offset,
    LocationXYZ16 bounds, LocationXYZ16 boundsOffset)
{
    uint32_t baseImageId = sceneryEntry->image + imageOffset + frameNum;
    uint32_t imageId = baseImageId;

    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_GLASS)
    {
        if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_PRIMARY_COLOUR)
        {
            imageId |= imageColourFlags;
        }

        if (dword_141F710 != 0)
        {
            imageId = (imageId & 0x7FFFF) | dword_141F710;
        }

        PaintAddImageAsParent(
            session, imageId, static_cast<int8_t>(offset.x), static_cast<int8_t>(offset.y), bounds.x, bounds.y,
            static_cast<int8_t>(bounds.z), offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z);
        if (dword_141F710 == 0)
        {
            imageId = baseImageId + dword_141F718;
            PaintAddImageAsChild(
                session, imageId, static_cast<int8_t>(offset.x), static_cast<int8_t>(offset.y), bounds.x, bounds.y,
                static_cast<int8_t>(bounds.z), offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z);
        }
    }
    else
    {
        if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_PRIMARY_COLOUR)
        {
            imageId |= imageColourFlags;
        }

        if (dword_141F710 != 0)
        {
            imageId = (imageId & 0x7FFFF) | dword_141F710;
        }

        paint_struct* paint = PaintAddImageAsParent(
            session, imageId, static_cast<int8_t>(offset.x), static_cast<int8_t>(offset.y), bounds.x, bounds.y,
            static_cast<int8_t>(bounds.z), offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z);
        if (paint != nullptr)
        {
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
void fence_paint(paint_session* session, uint8_t direction, int32_t height, const TileElement* tile_element)
{
    session->InteractionType = ViewportInteractionItem::Wall;

    rct_scenery_entry* sceneryEntry = tile_element->AsWall()->GetEntry();
    if (sceneryEntry == nullptr)
    {
        return;
    }
    uint32_t frameNum = 0;

    if (sceneryEntry->wall.flags2 & WALL_SCENERY_2_ANIMATED)
    {
        frameNum = (gCurrentTicks & 7) * 2;
    }

    int32_t primaryColour = tile_element->AsWall()->GetPrimaryColour();
    uint32_t imageColourFlags = SPRITE_ID_PALETTE_COLOUR_1(primaryColour);
    uint32_t dword_141F718 = imageColourFlags + 0x23800006;

    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
    {
        uint8_t secondaryColour = tile_element->AsWall()->GetSecondaryColour();
        imageColourFlags |= secondaryColour << 24 | IMAGE_TYPE_REMAP_2_PLUS;
    }

    uint32_t tertiaryColour = 0;
    if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_TERNARY_COLOUR)
    {
        tertiaryColour = tile_element->AsWall()->GetTertiaryColour();
        imageColourFlags &= 0x0DFFFFFFF;
    }

    paint_util_set_general_support_height(session, 8 * tile_element->clearance_height, 0x20);

    uint32_t dword_141F710 = 0;
    if (gTrackDesignSaveMode || (session->ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
    {
        if (!track_design_save_contains_tile_element(tile_element))
        {
            dword_141F710 = SPRITE_ID_PALETTE_COLOUR_1(EnumValue(FilterPaletteID::Palette46));
        }
    }

    if (tile_element->IsGhost())
    {
        session->InteractionType = ViewportInteractionItem::None;
        dword_141F710 = CONSTRUCTION_MARKER;
    }

    // Save tile_element

    uint8_t ah = sceneryEntry->wall.height * 8 - 2;

    if (sceneryEntry->wall.flags & WALL_SCENERY_IS_DOOR)
    {
        LocationXYZ16 offset;
        LocationXYZ16 boundsR1, boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_;
        uint8_t animationFrame = tile_element->AsWall()->GetAnimationFrame();
        // Add the direction as well
        if (tile_element->AsWall()->AnimationIsBackwards())
            animationFrame |= (1 << 4);
        uint32_t imageId;
        switch (direction)
        {
            case 0:
                imageId = sceneryEntry->image + byte_9A406C[animationFrame];

                boundsR1 = { 1, 3, static_cast<int16_t>(ah - 5) };
                boundsR1_ = { 1, 1, static_cast<int16_t>(height + 1) };
                boundsR2 = { 1, 28, 3 };
                boundsR2_ = { 1, 1, static_cast<int16_t>(height + ah - 9) };

                boundsL1 = { 1, 28, ah };
                boundsL1_ = { 1, 1, static_cast<int16_t>(height + 1) };

                offset = { 0, 0, static_cast<int16_t>(height) };

                fence_paint_door(
                    session, imageId, sceneryEntry, imageColourFlags, tertiaryColour, dword_141F710, offset, boundsR1,
                    boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;

            case 1:
                imageId = sceneryEntry->image + byte_9A408C[animationFrame];

                boundsR1 = { 3, 3, static_cast<int16_t>(ah - 5) };
                boundsR1_ = { 1, 30, static_cast<int16_t>(height + 1) };
                boundsR2 = { 29, 3, 2 };
                boundsR2_ = { 1, 30, static_cast<int16_t>(height + ah - 8) };

                boundsL1 = { 29, 1, ah };
                boundsL1_ = { 2, 30, static_cast<int16_t>(height + 1) };

                offset = { 1, 31, static_cast<int16_t>(height) };

                fence_paint_door(
                    session, imageId, sceneryEntry, imageColourFlags, tertiaryColour, dword_141F710, offset, boundsR1,
                    boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;

            case 2:
                imageId = sceneryEntry->image + byte_9A40AC[animationFrame];

                boundsR1 = { 3, 3, static_cast<int16_t>(ah - 5) };
                boundsR1_ = { 30, 1, static_cast<int16_t>(height + 1) };
                boundsR2 = { 3, 29, 2 };
                boundsR2_ = { 30, 1, static_cast<int16_t>(height + ah - 8) };

                boundsL1 = { 1, 29, ah };
                boundsL1_ = { 30, 2, static_cast<int16_t>(height + 1) };

                offset = { 31, 0, static_cast<int16_t>(height) };

                fence_paint_door(
                    session, imageId, sceneryEntry, imageColourFlags, tertiaryColour, dword_141F710, offset, boundsR1,
                    boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;

            case 3:
                imageId = sceneryEntry->image + byte_9A40CC[animationFrame];

                boundsR1 = { 3, 1, static_cast<int16_t>(ah - 5) };
                boundsR1_ = { 1, 1, static_cast<int16_t>(height + 1) };
                boundsR2 = { 28, 1, 3 };
                boundsR2_ = { 1, 1, static_cast<int16_t>(height + ah - 9) };

                boundsL1 = { 28, 1, ah };
                boundsL1_ = { 1, 1, static_cast<int16_t>(height + 1) };

                offset = { 2, 1, static_cast<int16_t>(height) };

                fence_paint_door(
                    session, imageId, sceneryEntry, imageColourFlags, tertiaryColour, dword_141F710, offset, boundsR1,
                    boundsR1_, boundsR2, boundsR2_, boundsL1, boundsL1_);
                break;
        }

        return;
    }

    uint32_t imageOffset = 0;
    LocationXYZ16 offset = { 0, 0, 0 }, bounds = { 0, 0, 0 }, boundsOffset = { 0, 0, 0 };

    switch (direction)
    {
        case 0:
            if (tile_element->AsWall()->GetSlope() == 2)
            {
                imageOffset = 3;
            }
            else if (tile_element->AsWall()->GetSlope() == 1)
            {
                imageOffset = 5;
            }
            else
            {
                imageOffset = 1;
            }

            offset = { 0, 0, static_cast<int16_t>(height) };
            bounds = { 1, 28, ah };
            boundsOffset = { 1, 1, static_cast<int16_t>(height + 1) };
            break;

        case 1:
            if (tile_element->AsWall()->GetSlope() == 2)
            {
                imageOffset = 2;
            }
            else if (tile_element->AsWall()->GetSlope() == 1)
            {
                imageOffset = 4;
            }
            else
            {
                imageOffset = 0;
            }

            if (sceneryEntry->wall.flags & WALL_SCENERY_HAS_GLASS)
            {
                if (sceneryEntry->wall.flags & WALL_SCENERY_IS_BANNER)
                {
                    imageOffset += 12;
                }
            }
            else
            {
                if (sceneryEntry->wall.flags & WALL_SCENERY_IS_BANNER)
                {
                    imageOffset += 6;
                }
            }

            offset = { 1, 31, static_cast<int16_t>(height) };
            bounds = { 29, 1, ah };
            boundsOffset = { 2, 30, static_cast<int16_t>(height + 1) };
            break;

        case 2:
            if (tile_element->AsWall()->GetSlope() == 2)
            {
                imageOffset = 5;
            }
            else if (tile_element->AsWall()->GetSlope() == 1)
            {
                imageOffset = 3;
            }
            else
            {
                imageOffset = 1;
            }

            if (sceneryEntry->wall.flags & WALL_SCENERY_IS_BANNER)
            {
                imageOffset += 6;
            }

            offset = { 31, 0, static_cast<int16_t>(height) };
            bounds = { 1, 29, ah };
            boundsOffset = { 30, 2, static_cast<int16_t>(height + 1) };
            break;

        case 3:
            if (tile_element->AsWall()->GetSlope() == 2)
            {
                imageOffset = 4;
            }
            else if (tile_element->AsWall()->GetSlope() == 1)
            {
                imageOffset = 2;
            }
            else
            {
                imageOffset = 0;
            }

            offset = { 2, 1, static_cast<int16_t>(height) };
            bounds = { 28, 1, ah };
            boundsOffset = { 1, 1, static_cast<int16_t>(height + 1) };
            break;
    }

    fence_paint_wall(
        session, frameNum, sceneryEntry, dword_141F710, imageColourFlags, dword_141F718, tertiaryColour, imageOffset, offset,
        bounds, boundsOffset);

    if (sceneryEntry->wall.scrolling_mode == SCROLLING_MODE_NONE)
    {
        return;
    }

    if (direction != 0 && direction != 3)
    {
        return;
    }

    auto secondaryColour = tile_element->AsWall()->GetSecondaryColour();
    if (dword_141F710 != 0)
    {
        secondaryColour = COLOUR_GREY;
    }
    if (direction == 0)
    {
        secondaryColour = ColourMapA[secondaryColour].mid_dark;
    }
    else
    {
        secondaryColour = ColourMapA[secondaryColour].light;
    }

    uint16_t scrollingMode = sceneryEntry->wall.scrolling_mode + ((direction + 1) & 0x3);
    if (scrollingMode >= MAX_SCROLLING_TEXT_MODES)
    {
        return;
    }

    auto banner = tile_element->AsWall()->GetBanner();
    if (banner != nullptr && !banner->IsNull())
    {
        auto ft = Formatter();
        banner->FormatTextTo(ft);
        utf8 signString[256];
        if (gConfigGeneral.upper_case_banners)
        {
            format_string_to_upper(signString, sizeof(signString), STR_SCROLLING_SIGN_TEXT, ft.Data());
        }
        else
        {
            format_string(signString, sizeof(signString), STR_SCROLLING_SIGN_TEXT, ft.Data());
        }

        uint16_t stringWidth = gfx_get_string_width(signString, FontSpriteBase::TINY);
        uint16_t scroll = stringWidth > 0 ? (gCurrentTicks / 2) % stringWidth : 0;

        PaintAddImageAsChild(
            session, scrolling_text_setup(session, STR_SCROLLING_SIGN_TEXT, ft, scroll, scrollingMode, secondaryColour), 0, 0,
            1, 1, 13, height + 8, boundsOffset.x, boundsOffset.y, boundsOffset.z);
    }
}
