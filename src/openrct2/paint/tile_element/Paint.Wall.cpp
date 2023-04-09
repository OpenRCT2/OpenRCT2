/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Paint.h"

#include "../../Game.h"
#include "../../common.h"
#include "../../config/Config.h"
#include "../../drawing/Drawing.h"
#include "../../interface/Colour.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Formatting.h"
#include "../../localisation/Localisation.h"
#include "../../object/WallSceneryEntry.h"
#include "../../profiling/Profiling.h"
#include "../../ride/Track.h"
#include "../../ride/TrackDesign.h"
#include "../../world/Banner.h"
#include "../../world/Map.h"
#include "../../world/Scenery.h"
#include "../../world/TileInspector.h"
#include "../../world/Wall.h"
#include "Paint.TileElement.h"

static constexpr const uint8_t DirectionToDoorImageOffset0[] = {
    2, 2, 22, 26, 30, 34, 34, 34, 34, 34, 30, 26, 22, 2, 6, 2, 2, 2, 6, 10, 14, 18, 18, 18, 18, 18, 14, 10, 6, 2, 22, 2,
};

static constexpr const uint8_t DirectionToDoorImageOffset1[] = {
    0, 0, 4, 8, 12, 16, 16, 16, 16, 16, 12, 8, 4, 0, 20, 0, 0, 0, 20, 24, 28, 32, 32, 32, 32, 32, 28, 24, 20, 0, 4, 0,
};

static constexpr const uint8_t DirectionToDoorImageOffset2[] = {
    2, 2, 6, 10, 14, 18, 18, 18, 18, 18, 14, 10, 6, 2, 22, 2, 2, 2, 22, 26, 30, 34, 34, 34, 34, 34, 30, 26, 22, 2, 6, 2,
};

static constexpr const uint8_t DirectionToDoorImageOffset3[] = {
    0, 0, 20, 24, 28, 32, 32, 32, 32, 32, 28, 24, 20, 0, 4, 0, 0, 0, 4, 8, 12, 16, 16, 16, 16, 16, 12, 8, 4, 0, 20, 0,
};

static constexpr const uint8_t* DirectionToDoorImageOffset[] = { DirectionToDoorImageOffset0, DirectionToDoorImageOffset1,
                                                                 DirectionToDoorImageOffset2, DirectionToDoorImageOffset3 };

static void PaintWallDoor(
    PaintSession& session, const WallSceneryEntry& wallEntry, ImageId imageId, CoordsXYZ offset, BoundBoxXYZ bbR1,
    BoundBoxXYZ bbR2, BoundBoxXYZ bbL)
{
    PROFILED_FUNCTION();

    auto newImageId0 = imageId;
    auto newImageId1 = imageId.WithIndexOffset(1);
    if (wallEntry.flags & WALL_SCENERY_IS_DOUBLE_SIDED)
    {
        PaintAddImageAsParent(session, newImageId0, offset, bbR1);
        PaintAddImageAsParent(session, newImageId1, offset, bbR2);
    }
    else
    {
        PaintAddImageAsParent(session, newImageId0, offset, bbL);
        PaintAddImageAsChild(session, newImageId1, offset, bbL);
    }
}

static void PaintWallDoor(
    PaintSession& session, const WallSceneryEntry& wallEntry, const WallElement& wallElement, ImageId imageTemplate,
    Direction direction, int32_t height)
{
    PROFILED_FUNCTION();

    auto bbHeight = wallEntry.height * 8 - 2;
    auto animationFrame = wallElement.GetAnimationFrame();

    // Add the direction as well
    if (wallElement.AnimationIsBackwards())
        animationFrame |= (1 << 4);

    auto imageId = wallEntry.image + DirectionToDoorImageOffset[direction & 3][animationFrame];
    switch (direction)
    {
        case 0:
        {
            BoundBoxXYZ bbR1 = { { 1, 1, height + 1 }, { 1, 3, bbHeight - 5 } };
            BoundBoxXYZ bbR2 = { { 1, 1, height + bbHeight - 9 }, { 1, 28, 3 } };

            BoundBoxXYZ bbL = { { 1, 1, height + 1 }, { 1, 28, bbHeight } };

            CoordsXYZ offset = { 0, 0, height };

            PaintWallDoor(session, wallEntry, imageTemplate.WithIndex(imageId), offset, bbR1, bbR2, bbL);
            break;
        }
        case 1:
        {
            BoundBoxXYZ bbR1 = { { 1, 30, height + 1 }, { 3, 3, bbHeight - 5 } };
            BoundBoxXYZ bbR2 = { { 1, 30, height + bbHeight - 8 }, { 29, 3, 2 } };
            BoundBoxXYZ bbL = { { 2, 30, height + 1 }, { 29, 1, bbHeight } };

            CoordsXYZ offset = { 1, 31, height };

            PaintWallDoor(session, wallEntry, imageTemplate.WithIndex(imageId), offset, bbR1, bbR2, bbL);
            break;
        }
        case 2:
        {
            BoundBoxXYZ bbR1 = { { 30, 1, height + 1 }, { 3, 3, bbHeight - 5 } };
            BoundBoxXYZ bbR2 = { { 30, 1, height + bbHeight - 8 }, { 3, 29, 2 } };
            BoundBoxXYZ bbL = { { 30, 2, height + 1 }, { 1, 29, bbHeight } };

            CoordsXYZ offset = { 31, 0, height };

            PaintWallDoor(session, wallEntry, imageTemplate.WithIndex(imageId), offset, bbR1, bbR2, bbL);
            break;
        }
        case 3:
        {
            BoundBoxXYZ bbR1 = { { 1, 1, height + 1 }, { 3, 1, bbHeight - 5 } };
            BoundBoxXYZ bbR2 = { { 1, 1, height + bbHeight - 9 }, { 28, 1, 3 } };
            BoundBoxXYZ bbL = { { 1, 1, height + 1 }, { 28, 1, bbHeight } };

            CoordsXYZ offset = { 2, 1, height };

            PaintWallDoor(session, wallEntry, imageTemplate.WithIndex(imageId), offset, bbR1, bbR2, bbL);
            break;
        }
    }
}

static void PaintWallWall(
    PaintSession& session, const WallSceneryEntry& wallEntry, ImageId imageTemplate, uint32_t imageOffset, CoordsXYZ offset,
    BoundBoxXYZ boundBox, bool isGhost)
{
    PROFILED_FUNCTION();

    auto frameNum = (wallEntry.flags2 & WALL_SCENERY_2_ANIMATED) ? (gCurrentTicks & 7) * 2 : 0;
    auto imageIndex = wallEntry.image + imageOffset + frameNum;
    PaintAddImageAsParent(session, imageTemplate.WithIndex(imageIndex), offset, boundBox);
    if ((wallEntry.flags & WALL_SCENERY_HAS_GLASS) && !isGhost)
    {
        auto glassImageId = ImageId(imageIndex + 6).WithTransparency(imageTemplate.GetPrimary());
        PaintAddImageAsChild(session, glassImageId, offset, boundBox);
    }
}

static void PaintWallScrollingText(
    PaintSession& session, const WallSceneryEntry& wallEntry, const WallElement& wallElement, Direction direction,
    int32_t height, const CoordsXYZ& boundsOffset, bool isGhost)
{
    PROFILED_FUNCTION();

    if (direction != 0 && direction != 3)
        return;

    auto scrollingMode = wallEntry.scrolling_mode;
    if (scrollingMode == SCROLLING_MODE_NONE)
        return;

    scrollingMode = wallEntry.scrolling_mode + ((direction + 1) & 3);
    if (scrollingMode >= MAX_SCROLLING_TEXT_MODES)
        return;

    auto banner = wallElement.GetBanner();
    if (banner == nullptr)
        return;

    auto textColour = isGhost ? static_cast<colour_t>(COLOUR_GREY) : wallElement.GetSecondaryColour();
    auto textPaletteIndex = direction == 0 ? ColourMapA[textColour].mid_dark : ColourMapA[textColour].light;

    auto ft = Formatter();
    banner->FormatTextTo(ft);
    char signString[256];
    if (gConfigGeneral.UpperCaseBanners)
    {
        FormatStringToUpper(signString, sizeof(signString), STR_SCROLLING_SIGN_TEXT, ft.Data());
    }
    else
    {
        OpenRCT2::FormatStringLegacy(signString, sizeof(signString), STR_SCROLLING_SIGN_TEXT, ft.Data());
    }

    auto stringWidth = GfxGetStringWidth(signString, FontStyle::Tiny);
    auto scroll = stringWidth > 0 ? (gCurrentTicks / 2) % stringWidth : 0;
    auto imageId = ScrollingTextSetup(session, STR_SCROLLING_SIGN_TEXT, ft, scroll, scrollingMode, textPaletteIndex);
    PaintAddImageAsChild(session, imageId, { 0, 0, height + 8 }, { boundsOffset, { 1, 1, 13 } });
}

static void PaintWallWall(
    PaintSession& session, const WallSceneryEntry& wallEntry, const WallElement& wallElement, ImageId imageTemplate,
    Direction direction, int32_t height, bool isGhost)
{
    PROFILED_FUNCTION();

    uint8_t bbHeight = wallEntry.height * 8 - 2;
    ImageIndex imageOffset = 0;
    CoordsXYZ offset;
    BoundBoxXYZ boundBox;
    switch (direction)
    {
        case 0:
            if (wallElement.GetSlope() == 2)
            {
                imageOffset = 3;
            }
            else if (wallElement.GetSlope() == 1)
            {
                imageOffset = 5;
            }
            else
            {
                imageOffset = 1;
            }

            offset = { 0, 0, height };
            boundBox = { { 1, 1, height + 1 }, { 1, 28, bbHeight } };
            break;

        case 1:
            if (wallElement.GetSlope() == 2)
            {
                imageOffset = 2;
            }
            else if (wallElement.GetSlope() == 1)
            {
                imageOffset = 4;
            }
            else
            {
                imageOffset = 0;
            }

            if (wallEntry.flags & WALL_SCENERY_HAS_GLASS)
            {
                if (wallEntry.flags & WALL_SCENERY_IS_DOUBLE_SIDED)
                {
                    imageOffset += 12;
                }
            }
            else
            {
                if (wallEntry.flags & WALL_SCENERY_IS_DOUBLE_SIDED)
                {
                    imageOffset += 6;
                }
            }

            offset = { 1, 31, height };
            boundBox = { { 2, 30, height + 1 }, { 29, 1, bbHeight } };
            break;

        case 2:
            if (wallElement.GetSlope() == 2)
            {
                imageOffset = 5;
            }
            else if (wallElement.GetSlope() == 1)
            {
                imageOffset = 3;
            }
            else
            {
                imageOffset = 1;
            }

            if (wallEntry.flags & WALL_SCENERY_IS_DOUBLE_SIDED)
            {
                imageOffset += 6;
            }

            offset = { 31, 0, height };
            boundBox = { { 30, 2, height + 1 }, { 1, 29, bbHeight } };
            break;

        case 3:
            if (wallElement.GetSlope() == 2)
            {
                imageOffset = 4;
            }
            else if (wallElement.GetSlope() == 1)
            {
                imageOffset = 2;
            }
            else
            {
                imageOffset = 0;
            }

            offset = { 2, 1, height };
            boundBox = { { 1, 1, height + 1 }, { 28, 1, bbHeight } };
            break;
    }

    PaintWallWall(session, wallEntry, imageTemplate, imageOffset, offset, boundBox, isGhost);
    PaintWallScrollingText(session, wallEntry, wallElement, direction, height, boundBox.offset, isGhost);
}

void PaintWall(PaintSession& session, uint8_t direction, int32_t height, const WallElement& wallElement)
{
    PROFILED_FUNCTION();

    auto* wallEntry = wallElement.GetEntry();
    if (wallEntry == nullptr)
    {
        return;
    }

    session.InteractionType = ViewportInteractionItem::Wall;

    ImageId imageTemplate;
    if (wallEntry->flags & WALL_SCENERY_HAS_PRIMARY_COLOUR)
    {
        imageTemplate = imageTemplate.WithPrimary(wallElement.GetPrimaryColour());
    }
    if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
    {
        imageTemplate = imageTemplate.WithSecondary(wallElement.GetSecondaryColour());
    }
    if (wallEntry->flags & WALL_SCENERY_HAS_TERTIARY_COLOUR)
    {
        imageTemplate = imageTemplate.WithTertiary(wallElement.GetTertiaryColour());
    }

    PaintUtilSetGeneralSupportHeight(session, 8 * wallElement.ClearanceHeight, 0x20);

    auto isGhost = false;
    if (gTrackDesignSaveMode || (session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
    {
        if (!TrackDesignSaveContainsTileElement(reinterpret_cast<const TileElement*>(&wallElement)))
        {
            imageTemplate = ImageId().WithRemap(FilterPaletteID::Palette46);
            isGhost = true;
        }
    }

    if (wallElement.IsGhost())
    {
        session.InteractionType = ViewportInteractionItem::None;
        imageTemplate = ImageId().WithRemap(FilterPaletteID::PaletteGhost);
        isGhost = true;
    }
    else if (OpenRCT2::TileInspector::IsElementSelected(reinterpret_cast<const TileElement*>(&wallElement)))
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::PaletteGhost);
        isGhost = true;
    }

    if (wallEntry->flags & WALL_SCENERY_IS_DOOR)
    {
        PaintWallDoor(session, *wallEntry, wallElement, imageTemplate, direction, height);
    }
    else
    {
        PaintWallWall(session, *wallEntry, wallElement, imageTemplate, direction, height, isGhost);
    }
}
