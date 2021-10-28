/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../Game.h"
#include "../../config/Config.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Date.h"
#include "../../ride/TrackDesign.h"
#include "../../util/Util.h"
#include "../../world/Map.h"
#include "../../world/Scenery.h"
#include "../../world/SmallScenery.h"
#include "../../world/TileInspector.h"
#include "../Paint.h"
#include "../Supports.h"
#include "Paint.TileElement.h"

static constexpr const CoordsXY lengths[] = {
    { 12, 26 },
    { 26, 12 },
    { 12, 26 },
    { 26, 12 },
};

/**
 *
 *  rct2: 0x006DFF47
 */
void PaintSmallScenery(paint_session* session, uint8_t direction, int32_t height, const SmallSceneryElement& sceneryElement)
{
    if (session->ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)
    {
        return;
    }
    session->InteractionType = ViewportInteractionItem::Scenery;
    CoordsXYZ boxlength;
    CoordsXYZ boxoffset;
    boxoffset.x = 0;
    boxoffset.y = 0;
    boxoffset.z = height;
    uint32_t marker = 0;
    const int32_t rotation = session->CurrentRotation;
    if (gTrackDesignSaveMode)
    {
        if (!track_design_save_contains_tile_element(reinterpret_cast<const TileElement*>(&sceneryElement)))
        {
            marker = SPRITE_ID_PALETTE_COLOUR_1(EnumValue(FilterPaletteID::Palette46));
        }
    }
    if (sceneryElement.IsGhost())
    {
        session->InteractionType = ViewportInteractionItem::None;
        marker = CONSTRUCTION_MARKER;
    }
    else if (OpenRCT2::TileInspector::IsElementSelected(reinterpret_cast<const TileElement*>(&sceneryElement)))
    {
        marker = CONSTRUCTION_MARKER;
    }

    auto* sceneryEntry = sceneryElement.GetEntry();

    if (sceneryEntry == nullptr)
    {
        return;
    }

    int32_t baseImageid = sceneryEntry->image + direction;
    boxlength.x = 2;
    boxlength.y = 2;
    int8_t x_offset = 0;
    int8_t y_offset = 0;
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
    {
        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HALF_SPACE))
        {
            // 6DFFE3:
            static constexpr const CoordsXY scenery_half_tile_offsets[] = {
                { 3, 3 },
                { 3, 17 },
                { 17, 3 },
                { 3, 3 },
            };
            boxoffset.x = scenery_half_tile_offsets[direction].x;
            boxoffset.y = scenery_half_tile_offsets[direction].y;
            boxlength.x = lengths[direction].x;
            boxlength.y = lengths[direction].y;
            x_offset = 3;
            y_offset = 3;
        }
        else
        {
            x_offset = 15;
            y_offset = 15;
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
            {
                x_offset = 3;
                y_offset = 3;
                boxlength.x = 26;
                boxlength.y = 26;
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_NO_WALLS))
                {
                    x_offset = 1;
                    y_offset = 1;
                    boxlength.x = 30;
                    boxlength.y = 30;
                }
            }
            boxoffset.x = x_offset;
            boxoffset.y = y_offset;
        }
    }
    else
    {
        // 6DFFC2:
        uint8_t quadrant = (sceneryElement.GetSceneryQuadrant() + rotation) & 3;
        x_offset = SceneryQuadrantOffsets[quadrant].x;
        y_offset = SceneryQuadrantOffsets[quadrant].y;
        boxoffset.x = x_offset;
        boxoffset.y = y_offset;
    }
    // 6E007F:
    boxlength.z = sceneryEntry->height - 4;
    if (boxlength.z > 128 || boxlength.z < 0)
    {
        boxlength.z = 128;
    }
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_CAN_WITHER))
    {
        if (sceneryElement.GetAge() >= SCENERY_WITHER_AGE_THRESHOLD_1)
        {
            baseImageid += 4;
        }
        if (sceneryElement.GetAge() >= SCENERY_WITHER_AGE_THRESHOLD_2)
        {
            baseImageid += 4;
        }
    }
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
    {
        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
        {
            baseImageid |= SPRITE_ID_PALETTE_COLOUR_2(sceneryElement.GetPrimaryColour(), sceneryElement.GetSecondaryColour());
        }
        else
        {
            baseImageid |= SPRITE_ID_PALETTE_COLOUR_1(sceneryElement.GetPrimaryColour());
        }
    }
    if (marker != 0)
    {
        baseImageid = (baseImageid & 0x7FFFF) | marker;
    }

    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_IS_TREE) && (session->ViewFlags & VIEWPORT_FLAG_INVISIBLE_TREES)
        && (session->ViewFlags & VIEWPORT_FLAG_SEETHROUGH_TREES))
    {
        baseImageid = 0x7FFFF;
    }
    else if (
        sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_IS_TREE) && (session->ViewFlags & VIEWPORT_FLAG_SEETHROUGH_TREES)
        && !(session->ViewFlags & VIEWPORT_FLAG_INVISIBLE_TREES))
    {
        baseImageid &= 0x7FFFF;
        baseImageid |= gColourifyImageSeeThroughFlags;
    }
    if (!(sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED)))
    {
        PaintAddImageAsParent(
            session, baseImageid, { x_offset, y_offset, height }, { boxlength.x, boxlength.y, boxlength.z - 1 }, boxoffset);
    }

    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_GLASS))
    {
        if (marker == 0)
        {
            // Draw translucent overlay:
            // TODO: Name palette entries
            int32_t image_id = (baseImageid & 0x7FFFF) + (EnumValue(GlassPaletteIds[sceneryElement.GetPrimaryColour()]) << 19)
                + 0x40000004;
            PaintAddImageAsChild(
                session, image_id, x_offset, y_offset, boxlength.x, boxlength.y, boxlength.z - 1, height, boxoffset.x,
                boxoffset.y, boxoffset.z);
        }
    }

    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ANIMATED))
    {
        rct_drawpixelinfo* dpi = &session->DPI;
        if ((sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED)) || (dpi->zoom_level <= 1))
        {
            // 6E01A9:
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1))
            {
                // 6E0512:
                int32_t image_id = ((gCurrentTicks / 2) & 0xF) + sceneryEntry->image + 4;
                if (marker != 0)
                {
                    image_id = (image_id & 0x7FFFF) | marker;
                }
                PaintAddImageAsChild(
                    session, image_id, x_offset, y_offset, boxlength.x, boxlength.y, boxlength.z - 1, height, boxoffset.x,
                    boxoffset.y, boxoffset.z);
            }
            else if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4))
            {
                // 6E043B:
                int32_t image_id = ((gCurrentTicks / 2) & 0xF) + sceneryEntry->image + 8;
                if (marker != 0)
                {
                    image_id = (image_id & 0x7FFFF) | marker;
                }
                PaintAddImageAsChild(
                    session, image_id, x_offset, y_offset, boxlength.x, boxlength.y, boxlength.z - 1, height, boxoffset.x,
                    boxoffset.y, boxoffset.z);

                image_id = direction + sceneryEntry->image + 4;
                if (marker != 0)
                {
                    image_id = (image_id & 0x7FFFF) | marker;
                }
                PaintAddImageAsChild(
                    session, image_id, x_offset, y_offset, boxlength.x, boxlength.y, boxlength.z - 1, height, boxoffset.x,
                    boxoffset.y, boxoffset.z);

                image_id = ((gCurrentTicks / 2) & 0xF) + sceneryEntry->image + 24;
                if (marker != 0)
                {
                    image_id = (image_id & 0x7FFFF) | marker;
                }
                PaintAddImageAsChild(
                    session, image_id, x_offset, y_offset, boxlength.x, boxlength.y, boxlength.z - 1, height, boxoffset.x,
                    boxoffset.y, boxoffset.z);
            }
            else if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_IS_CLOCK))
            {
                // 6E035C:
                int32_t minuteImageOffset = ((gRealTimeOfDay.minute + 6) * 17) / 256;
                int32_t timeImageBase = gRealTimeOfDay.hour;
                while (timeImageBase >= 12)
                {
                    timeImageBase -= 12;
                }
                timeImageBase = (timeImageBase * 4) + minuteImageOffset;
                if (timeImageBase >= 48)
                {
                    timeImageBase -= 48;
                }
                int32_t image_id = timeImageBase + (direction * 12);
                if (image_id >= 48)
                {
                    image_id -= 48;
                }

                image_id = image_id + sceneryEntry->image + 68;
                if (marker != 0)
                {
                    image_id = (image_id & 0x7FFFF) | marker;
                }
                PaintAddImageAsChild(
                    session, image_id, x_offset, y_offset, boxlength.x, boxlength.y, boxlength.z - 1, height, boxoffset.x,
                    boxoffset.y, boxoffset.z);

                image_id = gRealTimeOfDay.minute + (direction * 15);
                if (image_id >= 60)
                {
                    image_id -= 60;
                }
                image_id = image_id + sceneryEntry->image + 8;
                if (marker != 0)
                {
                    image_id = (image_id & 0x7FFFF) | marker;
                }
                PaintAddImageAsChild(
                    session, image_id, x_offset, y_offset, boxlength.x, boxlength.y, boxlength.z - 1, height, boxoffset.x,
                    boxoffset.y, boxoffset.z);
            }
            else if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_SWAMP_GOO))
            {
                // 6E02F6:
                int32_t image_id = gCurrentTicks;
                image_id += session->SpritePosition.x / 4;
                image_id += session->SpritePosition.y / 4;
                image_id = (image_id / 4) & 15;
                image_id += sceneryEntry->image;
                if (marker != 0)
                {
                    image_id = (image_id & 0x7FFFF) | marker;
                }
                PaintAddImageAsChild(
                    session, image_id, x_offset, y_offset, boxlength.x, boxlength.y, boxlength.z - 1, height, boxoffset.x,
                    boxoffset.y, boxoffset.z);
            }
            else if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS))
            {
                int32_t frame = gCurrentTicks;
                if (!(sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_COG)))
                {
                    // 6E01F8:
                    frame += ((session->SpritePosition.x / 4) + (session->SpritePosition.y / 4));
                    frame += sceneryElement.GetSceneryQuadrant() << 2;
                }
                // 6E0222:
                uint16_t delay = sceneryEntry->animation_delay & 0xFF;
                frame >>= delay;
                frame &= sceneryEntry->animation_mask;
                int32_t image_id = 0;
                if (frame < sceneryEntry->num_frames)
                {
                    image_id = sceneryEntry->frame_offsets[frame];
                }
                image_id = (image_id * 4) + direction + sceneryEntry->image;
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED | SMALL_SCENERY_FLAG17))
                {
                    image_id += 4;
                }
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
                {
                    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
                    {
                        image_id |= SPRITE_ID_PALETTE_COLOUR_2(
                            sceneryElement.GetPrimaryColour(), sceneryElement.GetSecondaryColour());
                    }
                    else
                    {
                        image_id |= SPRITE_ID_PALETTE_COLOUR_1(sceneryElement.GetPrimaryColour());
                    }
                }
                if (marker != 0)
                {
                    image_id = (image_id & 0x7FFFF) | marker;
                }
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED))
                {
                    PaintAddImageAsParent(
                        session, image_id, { x_offset, y_offset, height }, { boxlength.x, boxlength.y, boxlength.z - 1 },
                        boxoffset);
                }
                else
                {
                    PaintAddImageAsChild(
                        session, image_id, x_offset, y_offset, boxlength.x, boxlength.y, boxlength.z - 1, height, boxoffset.x,
                        boxoffset.y, boxoffset.z);
                }
            }
        }
    }
    // 6E0556: Draw supports:
    if (sceneryElement.NeedsSupports())
    {
        if (!(sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_NO_SUPPORTS)))
        {
            int32_t ax = 0;
            int32_t supportHeight = height;
            if (supportHeight & 0xF)
            {
                supportHeight &= 0xFFFFFFF0;
                ax = 49;
            }
            uint32_t supportImageColourFlags = IMAGE_TYPE_REMAP;
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_PAINT_SUPPORTS))
            {
                supportImageColourFlags = SPRITE_ID_PALETTE_COLOUR_1(sceneryElement.GetPrimaryColour());
            }
            if (marker != 0)
            {
                supportImageColourFlags = marker;
            }
            if (direction & 1)
            {
                wooden_b_supports_paint_setup(session, 1, ax, supportHeight, supportImageColourFlags);
            }
            else
            {
                wooden_b_supports_paint_setup(session, 0, ax, supportHeight, supportImageColourFlags);
            }
        }
    }
    // 6E05D1:
    height += sceneryEntry->height;

    paint_util_set_general_support_height(session, ceil2(height, 8), 0x20);
    // 6E05FF:
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_BUILD_DIRECTLY_ONTOP))
    {
        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
        {
            // 6E0825:
            paint_util_set_segment_support_height(session, SEGMENT_C4, height, 0x20);
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
            {
                paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~SEGMENT_C4, height, 0x20);
            }
            return;
        }
        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
        {
            // 6E075C:
            direction = (sceneryElement.GetSceneryQuadrant() + rotation) % 4;
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), height, 0x20);
            return;
        }
        return;
    }
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG27 | SMALL_SCENERY_FLAG_FULL_TILE))
    {
        paint_util_set_segment_support_height(session, SEGMENT_C4, 0xFFFF, 0);
        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
        {
            paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~SEGMENT_C4, 0xFFFF, 0);
        }
        return;
    }
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
    {
        direction = (sceneryElement.GetSceneryQuadrant() + rotation) % 4;
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
        return;
    }
}
