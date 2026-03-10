/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Paint.h"

#include "../../Game.h"
#include "../../GameState.h"
#include "../../core/EnumUtils.hpp"
#include "../../interface/Viewport.h"
#include "../../object/SmallSceneryEntry.h"
#include "../../profiling/Profiling.h"
#include "../../ride/TrackDesign.h"
#include "../../world/Scenery.h"
#include "../../world/TileInspector.h"
#include "../../world/tile_element/SmallSceneryElement.h"
#include "../support/WoodenSupports.h"
#include "Paint.SmallScenery.h"
#include "Paint.TileElement.h"
#include "Segment.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Numerics;

static constexpr CoordsXY kLengths[] = {
    { 12, 26 },
    { 26, 12 },
    { 12, 26 },
    { 26, 12 },
};

static void PaintSmallScenerySupports(
    PaintSession& session, const SmallSceneryEntry& sceneryEntry, const SmallSceneryElement& sceneryElement,
    Direction direction, int32_t height, ImageId imageTemplate)
{
    PROFILED_FUNCTION();

    if (!sceneryElement.NeedsSupports())
        return;

    if (sceneryEntry.flags.has(SmallSceneryFlag::hasNoSupports))
        return;

    auto transitionType = WoodenSupportTransitionType::none;
    auto supportHeight = height;
    if (supportHeight & 0xF)
    {
        supportHeight &= ~0xF;
        transitionType = WoodenSupportTransitionType::scenery;
    }

    auto supportImageTemplate = ImageId().WithRemap(0);
    if (sceneryEntry.flags.has(SmallSceneryFlag::supportsHavePrimaryColour))
    {
        supportImageTemplate = ImageId().WithPrimary(sceneryElement.GetPrimaryColour());
    }
    if (imageTemplate.IsRemap())
    {
        supportImageTemplate = imageTemplate;
    }

    WoodenBSupportsPaintSetupRotated(
        session, WoodenSupportType::truss, WoodenSupportSubType::neSw, direction, supportHeight, supportImageTemplate,
        transitionType);
}

static void SetSupportHeights(
    PaintSession& session, const SmallSceneryEntry& sceneryEntry, const SmallSceneryElement& sceneryElement, int32_t height)
{
    height += sceneryEntry.height;

    PaintUtilSetGeneralSupportHeight(session, ceil2(height, 8));
    if (sceneryEntry.flags.has(SmallSceneryFlag::allowSupportsAbove))
    {
        if (sceneryEntry.flags.has(SmallSceneryFlag::occupiesFullTile))
        {
            PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::centre), height, 0x20);
            if (sceneryEntry.flags.has(SmallSceneryFlag::vOffsetCentre))
            {
                PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~EnumToFlag(PaintSegment::centre), height, 0x20);
            }
        }
        else if (sceneryEntry.flags.has(SmallSceneryFlag::vOffsetCentre))
        {
            auto direction = (sceneryElement.GetSceneryQuadrant() + session.CurrentRotation) % 4;
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
                height, 0x20);
        }
    }
    else if (sceneryEntry.flags.hasAny(SmallSceneryFlag::flag27, SmallSceneryFlag::occupiesFullTile))
    {
        PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::centre), 0xFFFF, 0);
        if (sceneryEntry.flags.has(SmallSceneryFlag::vOffsetCentre))
        {
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~EnumToFlag(PaintSegment::centre), 0xFFFF, 0);
        }
    }
    else if (sceneryEntry.flags.has(SmallSceneryFlag::vOffsetCentre))
    {
        auto direction = (sceneryElement.GetSceneryQuadrant() + session.CurrentRotation) % 4;
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight), direction),
            0xFFFF, 0);
    }
}

static void PaintSmallSceneryBody(
    PaintSession& session, uint8_t direction, int32_t height, const SmallSceneryElement& sceneryElement,
    const SmallSceneryEntry* sceneryEntry, ImageId imageTemplate)
{
    PROFILED_FUNCTION();

    BoundBoxXYZ boundBox = { { 0, 0, height }, { 2, 2, 0 } };

    CoordsXYZ offset = { 0, 0, height };
    if (sceneryEntry->flags.has(SmallSceneryFlag::occupiesFullTile))
    {
        if (sceneryEntry->flags.has(SmallSceneryFlag::occupiesHalfTile))
        {
            static constexpr CoordsXY kSceneryHalfTileOffsets[] = {
                { 3, 3 },
                { 3, 17 },
                { 17, 3 },
                { 3, 3 },
            };
            boundBox.offset.x = kSceneryHalfTileOffsets[direction].x;
            boundBox.offset.y = kSceneryHalfTileOffsets[direction].y;
            boundBox.length.x = kLengths[direction].x;
            boundBox.length.y = kLengths[direction].y;
            offset.x = 3;
            offset.y = 3;
        }
        else
        {
            offset.x = 15;
            offset.y = 15;
            if (sceneryEntry->flags.has(SmallSceneryFlag::vOffsetCentre))
            {
                offset.x = 3;
                offset.y = 3;
                boundBox.length.x = 26;
                boundBox.length.y = 26;
                if (sceneryEntry->flags.has(SmallSceneryFlag::prohibitWalls))
                {
                    offset.x = 1;
                    offset.y = 1;
                    boundBox.length.x = 30;
                    boundBox.length.y = 30;
                }
            }
            boundBox.offset.x = offset.x;
            boundBox.offset.y = offset.y;
        }
    }
    else
    {
        uint8_t quadrant = (sceneryElement.GetSceneryQuadrant() + session.CurrentRotation) & 3;
        // -1 to maintain compatibility with existing CSOs in context of issue #17616
        offset.x = SceneryQuadrantOffsets[quadrant].x - 1;
        offset.y = SceneryQuadrantOffsets[quadrant].y - 1;
        boundBox.offset.x = offset.x;
        boundBox.offset.y = offset.y;
    }
    boundBox.length.z = sceneryEntry->height - 4;
    if (boundBox.length.z > 128 || boundBox.length.z < 0)
    {
        boundBox.length.z = 128;
    }
    boundBox.length.z--;

    ImageIndex baseImageIndex = sceneryEntry->image + direction;
    if (sceneryEntry->flags.has(SmallSceneryFlag::canWither))
    {
        if (sceneryElement.GetAge() >= kSceneryWitherAgeThreshold1)
        {
            baseImageIndex += 4;
        }
        if (sceneryElement.GetAge() >= kSceneryWitherAgeThreshold2)
        {
            baseImageIndex += 4;
        }
    }
    if (!sceneryEntry->flags.has(SmallSceneryFlag::isVisibleWhenZoomed))
    {
        auto imageId = imageTemplate.WithIndex(baseImageIndex);
        if (!imageTemplate.IsRemap())
        {
            if (sceneryEntry->flags.has(SmallSceneryFlag::hasPrimaryColour))
            {
                imageId = imageId.WithPrimary(sceneryElement.GetPrimaryColour());
                if (sceneryEntry->flags.has(SmallSceneryFlag::hasSecondaryColour))
                {
                    imageId = imageId.WithSecondary(sceneryElement.GetSecondaryColour());
                }
            }
            if (sceneryEntry->flags.has(SmallSceneryFlag::hasTertiaryColour))
            {
                imageId = imageId.WithTertiary(sceneryElement.GetTertiaryColour());
            }
        }
        PaintAddImageAsParent(session, imageId, offset, boundBox);
    }

    if (sceneryEntry->flags.has(SmallSceneryFlag::hasGlass) && !imageTemplate.IsRemap())
    {
        auto imageId = ImageId(baseImageIndex + 4).WithTransparency(sceneryElement.GetPrimaryColour());
        PaintAddImageAsChild(session, imageId, offset, boundBox);
    }

    if (sceneryEntry->flags.has(SmallSceneryFlag::isAnimated))
    {
        const auto currentTicks = getGameState().currentTicks;

        if (sceneryEntry->flags.has(SmallSceneryFlag::isVisibleWhenZoomed) || (session.rt.zoom_level <= ZoomLevel{ 1 }))
        {
            if (sceneryEntry->flags.has(SmallSceneryFlag::isFountain))
            {
                auto imageIndex = sceneryEntry->image + 4 + ((currentTicks / 2) & 0xF);
                auto imageId = imageTemplate.WithIndex(imageIndex);
                PaintAddImageAsChild(session, imageId, offset, boundBox);
            }
            else if (sceneryEntry->flags.has(SmallSceneryFlag::isCupidFountain))
            {
                auto imageIndex = sceneryEntry->image + 8 + ((currentTicks / 2) & 0xF);
                PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), offset, boundBox);

                imageIndex = direction + sceneryEntry->image + 4;
                PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), offset, boundBox);

                imageIndex = sceneryEntry->image + 24 + ((currentTicks / 2) & 0xF);
                PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), offset, boundBox);
            }
            else if (sceneryEntry->flags.has(SmallSceneryFlag::isClock))
            {
                auto minuteImageOffset = ((gRealTimeOfDay.minute + 6) * 17) / 256;
                auto timeImageBase = gRealTimeOfDay.hour % 12;
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
                PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), offset, boundBox);

                imageIndex = gRealTimeOfDay.minute + (direction * 15);
                if (imageIndex >= 60)
                {
                    imageIndex -= 60;
                }
                imageIndex = sceneryEntry->image + 8 + imageIndex;
                PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), offset, boundBox);
            }
            else if (sceneryEntry->flags.has(SmallSceneryFlag::isSwampGoo))
            {
                auto imageIndex = currentTicks;
                imageIndex += session.SpritePosition.x / 4;
                imageIndex += session.SpritePosition.y / 4;
                imageIndex = sceneryEntry->image + ((imageIndex / 4) % 16);
                PaintAddImageAsChild(session, imageTemplate.WithIndex(imageIndex), offset, boundBox);
            }
            else if (sceneryEntry->flags.has(SmallSceneryFlag::hasFrameOffsets))
            {
                auto delay = sceneryEntry->animation_delay & 0xFF;
                auto frame = currentTicks;
                if (!sceneryEntry->flags.has(SmallSceneryFlag::isCogwheel))
                {
                    frame += ((session.SpritePosition.x / 4) + (session.SpritePosition.y / 4));
                    frame += sceneryElement.GetSceneryQuadrant() << 2;
                }
                frame = (frame >> delay) & sceneryEntry->animation_mask;

                auto imageIndex = 0;
                if (frame < sceneryEntry->FrameOffsetCount)
                {
                    imageIndex = sceneryEntry->frame_offsets[frame];
                }
                imageIndex = (imageIndex * 4) + direction + sceneryEntry->image;
                if (sceneryEntry->flags.hasAny(SmallSceneryFlag::isVisibleWhenZoomed, SmallSceneryFlag::flag17))
                {
                    imageIndex += 4;
                }

                auto imageId = imageTemplate.WithIndex(imageIndex);
                if (!imageTemplate.IsRemap())
                {
                    if (sceneryEntry->flags.has(SmallSceneryFlag::hasPrimaryColour))
                    {
                        imageId = ImageId(imageIndex).WithPrimary(sceneryElement.GetPrimaryColour());
                        if (sceneryEntry->flags.has(SmallSceneryFlag::hasSecondaryColour))
                        {
                            imageId = imageId.WithSecondary(sceneryElement.GetSecondaryColour());
                        }
                    }
                    if (sceneryEntry->flags.has(SmallSceneryFlag::hasTertiaryColour))
                    {
                        imageId = imageId.WithTertiary(sceneryElement.GetTertiaryColour());
                    }
                }

                if (sceneryEntry->flags.has(SmallSceneryFlag::isVisibleWhenZoomed))
                {
                    PaintAddImageAsParent(session, imageId, offset, boundBox);
                }
                else
                {
                    PaintAddImageAsChild(session, imageId, offset, boundBox);
                }
            }
        }
    }
}

void PaintSmallScenery(PaintSession& session, uint8_t direction, int32_t height, const SmallSceneryElement& sceneryElement)
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

    session.InteractionType = ViewportInteractionItem::scenery;
    ImageId imageTemplate;
    if (gTrackDesignSaveMode)
    {
        if (!TrackDesignSaveContainsTileElement(reinterpret_cast<const TileElement*>(&sceneryElement)))
        {
            imageTemplate = ImageId().WithRemap(FilterPaletteID::palette46);
        }
    }
    if (sceneryElement.IsGhost())
    {
        session.InteractionType = ViewportInteractionItem::none;
        imageTemplate = ImageId().WithRemap(FilterPaletteID::paletteGhost);
    }
    else if (session.SelectedElement == reinterpret_cast<const TileElement*>(&sceneryElement))
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::paletteGhost);
    }

    PaintSmallSceneryBody(session, direction, height, sceneryElement, sceneryEntry, imageTemplate);
    PaintSmallScenerySupports(session, *sceneryEntry, sceneryElement, direction, height, imageTemplate);
    SetSupportHeights(session, *sceneryEntry, sceneryElement, height);
}
