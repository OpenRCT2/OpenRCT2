/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Paint.h"

#include "../../Game.h"
#include "../../config/Config.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Date.h"
#include "../../profiling/Profiling.h"
#include "../../ride/TrackDesign.h"
#include "../../util/Util.h"
#include "../../world/Map.h"
#include "../../world/Scenery.h"
#include "../../world/SmallScenery.h"
#include "../../world/TileInspector.h"
#include "../Supports.h"
#include "Paint.TileElement.h"

static constexpr const CoordsXY lengths[] = {
    { 12, 26 },
    { 26, 12 },
    { 12, 26 },
    { 26, 12 },
};

static void PaintSmallScenerySupports(
    paint_session& session, const SmallSceneryEntry& sceneryEntry, const SmallSceneryElement& sceneryElement,
    Direction direction, int32_t height, ImageId imageTemplate)
{
    PROFILED_FUNCTION();

    if (!sceneryElement.NeedsSupports())
        return;

    if (sceneryEntry.HasFlag(SMALL_SCENERY_FLAG_NO_SUPPORTS))
        return;

    auto special = 0;
    auto supportHeight = height;
    if (supportHeight & 0xF)
    {
        supportHeight &= ~0xF;
        special = 49;
    }

    auto supportImageTemplate = ImageId().WithRemap(0);
    if (sceneryEntry.HasFlag(SMALL_SCENERY_FLAG_PAINT_SUPPORTS))
    {
        supportImageTemplate = ImageId().WithPrimary(sceneryElement.GetPrimaryColour());
    }
    if (imageTemplate.IsRemap())
    {
        supportImageTemplate = imageTemplate;
    }

    auto supportType = (direction & 1) ? 1 : 0;
    wooden_b_supports_paint_setup(session, supportType, special, supportHeight, supportImageTemplate);
}

static void SetSupportHeights(
    paint_session& session, const SmallSceneryEntry& sceneryEntry, const SmallSceneryElement& sceneryElement, int32_t height)
{
    height += sceneryEntry.height;

    paint_util_set_general_support_height(session, ceil2(height, 8), 0x20);
    if (sceneryEntry.HasFlag(SMALL_SCENERY_FLAG_BUILD_DIRECTLY_ONTOP))
    {
        if (sceneryEntry.HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
        {
            paint_util_set_segment_support_height(session, SEGMENT_C4, height, 0x20);
            if (sceneryEntry.HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
            {
                paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~SEGMENT_C4, height, 0x20);
            }
        }
        else if (sceneryEntry.HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
        {
            auto direction = (sceneryElement.GetSceneryQuadrant() + session.CurrentRotation) % 4;
            paint_util_set_segment_support_height(
                session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), height, 0x20);
        }
    }
    else if (sceneryEntry.HasFlag(SMALL_SCENERY_FLAG27 | SMALL_SCENERY_FLAG_FULL_TILE))
    {
        paint_util_set_segment_support_height(session, SEGMENT_C4, 0xFFFF, 0);
        if (sceneryEntry.HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
        {
            paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~SEGMENT_C4, 0xFFFF, 0);
        }
    }
    else if (sceneryEntry.HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
    {
        auto direction = (sceneryElement.GetSceneryQuadrant() + session.CurrentRotation) % 4;
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
    }
}

static void PaintSmallSceneryBody(
    paint_session& session, uint8_t direction, int32_t height, const SmallSceneryElement& sceneryElement,
    const SmallSceneryEntry* sceneryEntry, ImageId imageTemplate)
{
    PROFILED_FUNCTION();

    session.InteractionType = ViewportInteractionItem::Scenery;
    CoordsXYZ boxLength;
    CoordsXYZ boxOffset{ 0, 0, height };

    boxLength.x = 2;
    boxLength.y = 2;

    CoordsXYZ offset = { 0, 0, height };
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
    {
        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HALF_SPACE))
        {
            static constexpr const CoordsXY sceneryHalfTileOffsets[] = {
                { 3, 3 },
                { 3, 17 },
                { 17, 3 },
                { 3, 3 },
            };
            boxOffset.x = sceneryHalfTileOffsets[direction].x;
            boxOffset.y = sceneryHalfTileOffsets[direction].y;
            boxLength.x = lengths[direction].x;
            boxLength.y = lengths[direction].y;
            offset.x = 3;
            offset.y = 3;
        }
        else
        {
            offset.x = 15;
            offset.y = 15;
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
            {
                offset.x = 3;
                offset.y = 3;
                boxLength.x = 26;
                boxLength.y = 26;
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_NO_WALLS))
                {
                    offset.x = 1;
                    offset.y = 1;
                    boxLength.x = 30;
                    boxLength.y = 30;
                }
            }
            boxOffset.x = offset.x;
            boxOffset.y = offset.y;
        }
    }
    else
    {
        uint8_t quadrant = (sceneryElement.GetSceneryQuadrant() + session.CurrentRotation) & 3;
        offset.x = SceneryQuadrantOffsets[quadrant].x;
        offset.y = SceneryQuadrantOffsets[quadrant].y;
        boxOffset.x = offset.x;
        boxOffset.y = offset.y;
    }
    boxLength.z = sceneryEntry->height - 4;
    if (boxLength.z > 128 || boxLength.z < 0)
    {
        boxLength.z = 128;
    }
    boxLength.z--;

    ImageIndex baseImageIndex = sceneryEntry->image + direction;
    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_CAN_WITHER))
    {
        if (sceneryElement.GetAge() >= SCENERY_WITHER_AGE_THRESHOLD_1)
        {
            baseImageIndex += 4;
        }
        if (sceneryElement.GetAge() >= SCENERY_WITHER_AGE_THRESHOLD_2)
        {
            baseImageIndex += 4;
        }
    }
    if (!(sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED)))
    {
        auto imageId = imageTemplate.WithIndex(baseImageIndex);
        if (!imageTemplate.IsRemap())
        {
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
            {
                imageId = imageId.WithPrimary(sceneryElement.GetPrimaryColour());
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
                {
                    imageId = imageId.WithSecondary(sceneryElement.GetSecondaryColour());
                }
            }
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR))
            {
                imageId = imageId.WithTertiary(sceneryElement.GetTertiaryColour());
            }
        }
        PaintAddImageAsParent(session, imageId, offset, boxLength, boxOffset);
    }

    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_GLASS) && !imageTemplate.IsRemap())
    {
        auto imageId = ImageId(baseImageIndex + 4).WithTransparancy(sceneryElement.GetPrimaryColour());
        PaintAddImageAsChild(session, imageId, offset, boxLength, boxOffset);
    }

    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ANIMATED))
    {
        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED) || (session.DPI.zoom_level <= ZoomLevel{ 1 }))
        {
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_1))
            {
                auto imageIndex = sceneryEntry->image + 4 + ((gCurrentTicks / 2) & 0xF);
                auto imageId = imageTemplate.WithIndex(imageIndex);
                PaintAddImageAsChild(session, imageId, offset, boxLength, boxOffset);
            }
            else if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FOUNTAIN_SPRAY_4))
            {
                auto imageIndex = sceneryEntry->image + 8 + ((gCurrentTicks / 2) & 0xF);
                PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), offset, boxLength, boxOffset);

                imageIndex = direction + sceneryEntry->image + 4;
                PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), offset, boxLength, boxOffset);

                imageIndex = sceneryEntry->image + 24 + ((gCurrentTicks / 2) & 0xF);
                PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), offset, boxLength, boxOffset);
            }
            else if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_IS_CLOCK))
            {
                auto minuteImageOffset = ((gRealTimeOfDay.minute + 6) * 17) / 256;
                auto timeImageBase = gRealTimeOfDay.hour;
                while (timeImageBase >= 12)
                {
                    timeImageBase -= 12;
                }
                timeImageBase = (timeImageBase * 4) + minuteImageOffset;
                if (timeImageBase >= 48)
                {
                    timeImageBase -= 48;
                }
                auto imageIndex = timeImageBase + (direction * 12);
                if (imageIndex >= 48)
                {
                    imageIndex -= 48;
                }

                imageIndex = sceneryEntry->image + 68 + imageIndex;
                PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), offset, boxLength, boxOffset);

                imageIndex = gRealTimeOfDay.minute + (direction * 15);
                if (imageIndex >= 60)
                {
                    imageIndex -= 60;
                }
                imageIndex = sceneryEntry->image + 8 + imageIndex;
                PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), offset, boxLength, boxOffset);
            }
            else if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_SWAMP_GOO))
            {
                auto imageIndex = gCurrentTicks;
                imageIndex += session.SpritePosition.x / 4;
                imageIndex += session.SpritePosition.y / 4;
                imageIndex = sceneryEntry->image + ((imageIndex / 4) % 16);
                PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), offset, boxLength, boxOffset);
            }
            else if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_FRAME_OFFSETS))
            {
                auto delay = sceneryEntry->animation_delay & 0xFF;
                auto frame = gCurrentTicks;
                if (!(sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_COG)))
                {
                    frame += ((session.SpritePosition.x / 4) + (session.SpritePosition.y / 4));
                    frame += sceneryElement.GetSceneryQuadrant() << 2;
                }
                frame = (frame >> delay) & sceneryEntry->animation_mask;

                auto imageIndex = 0;
                if (frame < sceneryEntry->num_frames)
                {
                    imageIndex = sceneryEntry->frame_offsets[frame];
                }
                imageIndex = (imageIndex * 4) + direction + sceneryEntry->image;
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED | SMALL_SCENERY_FLAG17))
                {
                    imageIndex += 4;
                }

                auto imageId = imageTemplate.WithIndex(imageIndex);
                if (!imageTemplate.IsRemap())
                {
                    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
                    {
                        imageId = ImageId(imageIndex).WithPrimary(sceneryElement.GetPrimaryColour());
                        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
                        {
                            imageId = imageId.WithSecondary(sceneryElement.GetSecondaryColour());
                        }
                    }
                    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR))
                    {
                        imageId = imageId.WithTertiary(sceneryElement.GetTertiaryColour());
                    }
                }

                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VISIBLE_WHEN_ZOOMED))
                {
                    PaintAddImageAsParent(session, imageId, offset, boxLength, boxOffset);
                }
                else
                {
                    PaintAddImageAsChild(session, imageId, offset, boxLength, boxOffset);
                }
            }
        }
    }
}

void PaintSmallScenery(paint_session& session, uint8_t direction, int32_t height, const SmallSceneryElement& sceneryElement)
{
    PROFILED_FUNCTION();

    if (session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)
    {
        return;
    }

    auto* sceneryEntry = sceneryElement.GetEntry();
    if (sceneryEntry == nullptr)
    {
        return;
    }

    session.InteractionType = ViewportInteractionItem::Scenery;
    ImageId imageTemplate;
    if (gTrackDesignSaveMode)
    {
        if (!track_design_save_contains_tile_element(reinterpret_cast<const TileElement*>(&sceneryElement)))
        {
            imageTemplate = ImageId().WithRemap(FilterPaletteID::Palette46);
        }
    }
    if (sceneryElement.IsGhost())
    {
        session.InteractionType = ViewportInteractionItem::None;
        imageTemplate = ImageId().WithRemap(FilterPaletteID::Palette44);
    }
    else if (OpenRCT2::TileInspector::IsElementSelected(reinterpret_cast<const TileElement*>(&sceneryElement)))
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::Palette44);
    }

    PaintSmallSceneryBody(session, direction, height, sceneryElement, sceneryEntry, imageTemplate);
    PaintSmallScenerySupports(session, *sceneryEntry, sceneryElement, direction, height, imageTemplate);
    SetSupportHeights(session, *sceneryEntry, sceneryElement, height);
}
