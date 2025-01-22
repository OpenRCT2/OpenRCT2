/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Paint.h"

#include "../../Context.h"
#include "../../Game.h"
#include "../../GameState.h"
#include "../../config/Config.h"
#include "../../core/Numerics.hpp"
#include "../../entity/PatrolArea.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Formatter.h"
#include "../../localisation/Formatting.h"
#include "../../localisation/StringIds.h"
#include "../../object/FootpathObject.h"
#include "../../object/FootpathRailingsObject.h"
#include "../../object/FootpathSurfaceObject.h"
#include "../../object/PathAdditionEntry.h"
#include "../../profiling/Profiling.h"
#include "../../ride/Ride.h"
#include "../../ride/Track.h"
#include "../../ride/TrackDesign.h"
#include "../../ride/TrackPaint.h"
#include "../../world/Footpath.h"
#include "../../world/Map.h"
#include "../../world/Scenery.h"
#include "../../world/TileInspector.h"
#include "../../world/tile_element/PathElement.h"
#include "../../world/tile_element/Slope.h"
#include "../../world/tile_element/SurfaceElement.h"
#include "../../world/tile_element/TileElement.h"
#include "../../world/tile_element/TrackElement.h"
#include "../Boundbox.h"
#include "../Paint.SessionFlags.h"
#include "../support/MetalSupports.h"
#include "../support/WoodenSupports.h"
#include "Paint.PathAddition.h"
#include "Paint.Surface.h"
#include "Paint.TileElement.h"
#include "Segment.h"

using namespace OpenRCT2;

bool gPaintWidePathsAsGhost = false;

const uint8_t kPathSlopeToLandSlope[] = {
    kTileSlopeSWSideUp,
    kTileSlopeNWSideUp,
    kTileSlopeNESideUp,
    kTileSlopeSESideUp,
};

static constexpr uint8_t Byte98D6E0[] = {
    0, 1, 2, 3, 4, 5, 6,  7,  8, 9,  10, 11, 12, 13, 14, 15, 0, 1, 2, 20, 4, 5, 6, 22, 8, 9, 10, 26, 12, 13, 14, 36,
    0, 1, 2, 3, 4, 5, 21, 23, 8, 9,  10, 11, 12, 13, 33, 37, 0, 1, 2, 3,  4, 5, 6, 24, 8, 9, 10, 11, 12, 13, 14, 38,
    0, 1, 2, 3, 4, 5, 6,  7,  8, 9,  10, 11, 29, 30, 34, 39, 0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 40,
    0, 1, 2, 3, 4, 5, 6,  7,  8, 9,  10, 11, 12, 13, 35, 41, 0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 42,
    0, 1, 2, 3, 4, 5, 6,  7,  8, 25, 10, 27, 12, 31, 14, 43, 0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 28, 12, 13, 14, 44,
    0, 1, 2, 3, 4, 5, 6,  7,  8, 9,  10, 11, 12, 13, 14, 45, 0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 46,
    0, 1, 2, 3, 4, 5, 6,  7,  8, 9,  10, 11, 12, 32, 14, 47, 0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 48,
    0, 1, 2, 3, 4, 5, 6,  7,  8, 9,  10, 11, 12, 13, 14, 49, 0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 50,
};

// clang-format off
static constexpr BoundBoxXY stru_98D804[] = {
    { { 3, 3 }, { 26, 26 } },
    { { 0, 3 }, { 29, 26 } },
    { { 3, 3 }, { 26, 29 } },
    { { 0, 3 }, { 29, 29 } },
    { { 3, 3 }, { 29, 26 } },
    { { 0, 3 }, { 32, 26 } },
    { { 3, 3 }, { 29, 29 } },
    { { 0, 3 }, { 32, 29 } },
    { { 3, 0 }, { 26, 29 } },
    { { 0, 0 }, { 29, 29 } },
    { { 3, 0 }, { 26, 32 } },
    { { 0, 0 }, { 29, 32 } },
    { { 3, 0 }, { 29, 29 } },
    { { 0, 0 }, { 32, 29 } },
    { { 3, 0 }, { 29, 32 } },
    { { 0, 0 }, { 32, 32 } },
};

static constexpr WoodenSupportSubType PathSupportOrientation[] = {
    WoodenSupportSubType::NeSw, 
    WoodenSupportSubType::NeSw, 
    WoodenSupportSubType::NwSe, 
    WoodenSupportSubType::NeSw,
    WoodenSupportSubType::NeSw, 
    WoodenSupportSubType::NeSw, 
    WoodenSupportSubType::NwSe, 
    WoodenSupportSubType::NeSw,
    WoodenSupportSubType::NwSe, 
    WoodenSupportSubType::NwSe, 
    WoodenSupportSubType::NwSe, 
    WoodenSupportSubType::NwSe,
    WoodenSupportSubType::NeSw, 
    WoodenSupportSubType::NeSw, 
    WoodenSupportSubType::NwSe, 
    WoodenSupportSubType::NeSw,
};
// clang-format on

void PathPaintBoxSupport(
    PaintSession& session, const PathElement& pathElement, int32_t height, const FootpathPaintInfo& pathPaintInfo,
    bool hasSupports, ImageId imageTemplate, ImageId sceneryImageTemplate);
void PathPaintPoleSupport(
    PaintSession& session, const PathElement& pathElement, int16_t height, const FootpathPaintInfo& pathPaintInfo,
    bool hasSupports, ImageId imageTemplate, ImageId sceneryImageTemplate);

static void PathPaintQueueBanner(
    PaintSession& session, const PathElement& pathElement, uint16_t height, const FootpathPaintInfo& pathPaintInfo,
    ImageId imageTemplate)
{
    auto imageId = imageTemplate.WithIndex(pathPaintInfo.RailingsImageId);

    uint8_t direction = pathElement.GetQueueBannerDirection();
    // Draw ride sign
    session.InteractionType = ViewportInteractionItem::Ride;
    if (pathElement.IsSloped())
    {
        if (pathElement.GetSlopeDirection() == direction)
            height += kCoordsZStep * 2;
    }
    direction += session.CurrentRotation;
    direction &= 3;

    CoordsXYZ boundBoxOffsets = CoordsXYZ(kBannerBoundBoxes[direction][0], height + 2);

    imageId = imageId.WithIndexOffset(28 + (direction << 1));

    // Draw pole in the back
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { boundBoxOffsets, { 1, 1, 21 } });

    // Draw pole in the front and banner
    boundBoxOffsets.x = kBannerBoundBoxes[direction][1].x;
    boundBoxOffsets.y = kBannerBoundBoxes[direction][1].y;
    imageId = imageId.WithIndexOffset(1);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { boundBoxOffsets, { 1, 1, 21 } });

    direction--;
    // If text shown
    auto ride = GetRide(pathElement.GetRideIndex());
    if (direction < 2 && ride != nullptr && !imageTemplate.IsRemap())
    {
        uint16_t scrollingMode = pathPaintInfo.ScrollingMode;
        scrollingMode += direction;

        auto ft = Formatter();

        if (ride->status == RideStatus::Open && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
        {
            ft.Add<StringId>(STR_RIDE_ENTRANCE_NAME);
            ride->FormatNameTo(ft);
        }
        else
        {
            ft.Add<StringId>(STR_RIDE_ENTRANCE_CLOSED);
        }

        utf8 bannerBuffer[512]{};
        if (Config::Get().general.UpperCaseBanners)
        {
            FormatStringToUpper(bannerBuffer, sizeof(bannerBuffer), STR_BANNER_TEXT_FORMAT, ft.Data());
        }
        else
        {
            FormatStringLegacy(bannerBuffer, sizeof(bannerBuffer), STR_BANNER_TEXT_FORMAT, ft.Data());
        }

        uint16_t stringWidth = GfxGetStringWidth(bannerBuffer, FontStyle::Tiny);
        uint16_t scroll = stringWidth > 0 ? (GetGameState().CurrentTicks / 2) % stringWidth : 0;

        PaintAddImageAsChild(
            session, ScrollingTextSetup(session, STR_BANNER_TEXT_FORMAT, ft, scroll, scrollingMode, COLOUR_BLACK),
            { 0, 0, height + 7 }, { boundBoxOffsets, { 1, 1, 21 } });
    }

    session.InteractionType = ViewportInteractionItem::Footpath;
    if (imageTemplate.IsRemap())
    {
        session.InteractionType = ViewportInteractionItem::None;
    }
}

static void PathPaintSlopedFences(
    PaintSession& session, const PathElement& pathElement, uint16_t height, ImageId imageId, bool isQueue)
{
    auto queueOffset = isQueue ? 14 : 0;
    switch ((pathElement.GetSlopeDirection() + session.CurrentRotation) & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK)
    {
        case 0:
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(8 + queueOffset), { 0, 4, height }, { { 0, 4, height + 2 }, { 32, 1, 23 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(8 + queueOffset), { 0, 28, height }, { { 0, 28, height + 2 }, { 32, 1, 23 } });
            break;
        case 1:
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(7 + queueOffset), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 32, 23 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(7 + queueOffset), { 28, 0, height }, { { 28, 0, height + 2 }, { 1, 32, 23 } });
            break;
        case 2:
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(9 + queueOffset), { 0, 4, height }, { { 0, 4, height + 2 }, { 32, 1, 23 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(9 + queueOffset), { 0, 28, height }, { { 0, 28, height + 2 }, { 32, 1, 23 } });
            break;
        case 3:
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(6 + queueOffset), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 32, 23 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(6 + queueOffset), { 28, 0, height }, { { 28, 0, height + 2 }, { 1, 32, 23 } });
            break;
    }
}

static void PathPaintFencesAndQueueBannersQueue(
    PaintSession& session, const PathElement& pathElement, uint16_t height, uint32_t connectedEdges, bool hasSupports,
    const FootpathPaintInfo& pathPaintInfo, ImageId imageTemplate)
{
    auto imageId = imageTemplate.WithIndex(pathPaintInfo.RailingsImageId);

    if (pathElement.IsSloped())
    {
        PathPaintSlopedFences(session, pathElement, height, imageId, true);
    }
    else
    {
        const auto pathEdges = connectedEdges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK;
        switch (pathEdges)
        {
            case 0b0001:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(17), { 0, 4, height }, { { 0, 4, height + 2 }, { 28, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(17), { 0, 28, height }, { { 0, 28, height + 2 }, { 28, 1, 7 } });
                break;
            case 0b0010:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(18), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 28, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(18), { 28, 0, height }, { { 28, 0, height + 2 }, { 1, 28, 7 } });
                break;
            case 0b0011:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(17), { 0, 4, height }, { { 0, 4, height + 2 }, { 28, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(18), { 28, 0, height },
                    { { 28, 4, height + 2 }, { 1, 28, 7 } }); // bound_box_offset_y seems to be a bug
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(25), { 0, 0, height }, { { 0, 28, height + 2 }, { 4, 4, 7 } });
                break;
            case 0b0100:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(19), { 0, 4, height }, { { 0, 4, height + 2 }, { 28, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(19), { 0, 28, height }, { { 0, 28, height + 2 }, { 28, 1, 7 } });
                break;
            case 0b0101:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(15), { 0, 4, height }, { { 0, 4, height + 2 }, { 32, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(15), { 0, 28, height }, { { 0, 28, height + 2 }, { 32, 1, 7 } });
                break;
            case 0b0110:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(18), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 28, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(19), { 0, 4, height }, { { 0, 4, height + 2 }, { 28, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(26), { 0, 0, height }, { { 28, 28, height + 2 }, { 4, 4, 7 } });
                break;
            case 0b0111:
                if (pathElement.HasJunctionRailings())
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(15), { 0, 4, height }, { { 0, 4, height + 2 }, { 32, 1, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(25), { 0, 0, height }, { { 0, 28, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(26), { 0, 0, height }, { { 28, 28, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 0b1000:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(16), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 28, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(16), { 28, 0, height }, { { 28, 0, height + 2 }, { 1, 28, 7 } });
                break;
            case 0b1001:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(16), { 28, 0, height }, { { 28, 0, height + 2 }, { 1, 28, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(17), { 0, 28, height }, { { 0, 28, height + 2 }, { 28, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(24), { 0, 0, height }, { { 0, 0, height + 2 }, { 4, 4, 7 } });
                break;
            case 0b1010:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(14), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 32, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(14), { 28, 0, height }, { { 28, 0, height + 2 }, { 1, 32, 7 } });
                break;
            case 0b1011:
                if (pathElement.HasJunctionRailings())
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(14), { 28, 0, height }, { { 28, 0, height + 2 }, { 1, 32, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(24), { 0, 0, height }, { { 0, 0, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(25), { 0, 0, height }, { { 0, 28, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 0b1100:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(16), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 28, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(19), { 0, 28, height },
                    { { 4, 28, height + 2 }, { 28, 1, 7 } }); // bound_box_offset_x seems to be a bug
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(27), { 0, 0, height }, { { 28, 0, height + 2 }, { 4, 4, 7 } });
                break;
            case 0b1101:
                if (pathElement.HasJunctionRailings())
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(15), { 0, 28, height }, { { 0, 28, height + 2 }, { 32, 1, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(24), { 0, 0, height }, { { 0, 0, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(27), { 0, 0, height }, { { 28, 0, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 0b1110:
                if (pathElement.HasJunctionRailings())
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(14), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 32, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(26), { 0, 0, height }, { { 28, 28, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(27), { 0, 0, height }, { { 28, 0, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 0b1111:
                if (pathElement.HasJunctionRailings())
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(24), { 0, 0, height }, { { 0, 0, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(25), { 0, 0, height }, { { 0, 28, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(26), { 0, 0, height }, { { 28, 28, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(27), { 0, 0, height }, { { 28, 0, height + 2 }, { 4, 4, 7 } });
                }
        }
    }

    if (pathElement.HasQueueBanner() && !(pathPaintInfo.RailingFlags & RAILING_ENTRY_FLAG_NO_QUEUE_BANNER))
    {
        PathPaintQueueBanner(session, pathElement, height, pathPaintInfo, imageTemplate);
    }
}

static void PathPaintFencesAndQueueBannersNonQueue(
    PaintSession& session, const PathElement& pathElement, uint16_t height, uint32_t connectedEdges, bool hasSupports,
    const FootpathPaintInfo& pathPaintInfo, ImageId imageTemplate)
{
    auto imageId = imageTemplate.WithIndex(pathPaintInfo.RailingsImageId);

    uint32_t drawnCorners = 0;
    // If the path is not drawn over the supports, then no corner sprites will be drawn (making double-width paths
    // look like connected series of intersections).
    if (pathPaintInfo.RailingFlags & RAILING_ENTRY_FLAG_DRAW_PATH_OVER_SUPPORTS)
    {
        drawnCorners = (connectedEdges & FOOTPATH_PROPERTIES_EDGES_CORNERS_MASK) >> 4;
    }

    auto slopeRailingsSupported = !(pathPaintInfo.SurfaceFlags & FOOTPATH_ENTRY_FLAG_NO_SLOPE_RAILINGS);
    if ((hasSupports || slopeRailingsSupported) && pathElement.IsSloped())
    {
        PathPaintSlopedFences(session, pathElement, height, imageId, false);
    }
    else
    {
        if (!hasSupports)
        {
            return;
        }

        switch (connectedEdges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK)
        {
            case 0:
                // purposely left empty
                break;
            case 1:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(3), { 0, 4, height }, { { 0, 4, height + 2 }, { 28, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(3), { 0, 28, height }, { { 0, 28, height + 2 }, { 28, 1, 7 } });
                break;
            case 2:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(4), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 28, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(4), { 28, 0, height }, { { 28, 0, height + 2 }, { 1, 28, 7 } });
                break;
            case 4:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(5), { 0, 4, height }, { { 0, 4, height + 2 }, { 28, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(5), { 0, 28, height }, { { 0, 28, height + 2 }, { 28, 1, 7 } });
                break;
            case 5:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(1), { 0, 4, height }, { { 0, 4, height + 2 }, { 32, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(1), { 0, 28, height }, { { 0, 28, height + 2 }, { 32, 1, 7 } });
                break;
            case 8:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(2), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 28, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(2), { 28, 0, height }, { { 28, 0, height + 2 }, { 1, 28, 7 } });
                break;
            case 10:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(0), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 32, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(0), { 28, 0, height }, { { 28, 0, height + 2 }, { 1, 32, 7 } });
                break;

            case 3:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(3), { 0, 4, height }, { { 0, 4, height + 2 }, { 28, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(4), { 28, 0, height },
                    { { 28, 4, height + 2 }, { 1, 28, 7 } }); // bound_box_offset_y seems to be a bug
                if (!(drawnCorners & FOOTPATH_CORNER_0))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(11), { 0, 0, height }, { { 0, 28, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 6:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(4), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 28, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(5), { 0, 4, height }, { { 0, 4, height + 2 }, { 28, 1, 7 } });
                if (!(drawnCorners & FOOTPATH_CORNER_1))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(12), { 0, 0, height }, { { 28, 28, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 9:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(2), { 28, 0, height }, { { 28, 0, height + 2 }, { 1, 28, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(3), { 0, 28, height }, { { 0, 28, height + 2 }, { 28, 1, 7 } });
                if (!(drawnCorners & FOOTPATH_CORNER_3))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(10), { 0, 0, height }, { { 0, 0, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 12:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(2), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 28, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(5), { 0, 28, height },
                    { { 4, 28, height + 2 }, { 28, 1, 7 } }); // bound_box_offset_x seems to be a bug
                if (!(drawnCorners & FOOTPATH_CORNER_2))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(13), { 0, 0, height }, { { 28, 0, height + 2 }, { 4, 4, 7 } });
                }
                break;

            case 7:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(1), { 0, 4, height }, { { 0, 4, height + 2 }, { 32, 1, 7 } });
                if (!(drawnCorners & FOOTPATH_CORNER_0))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(11), { 0, 0, height }, { { 0, 28, height + 2 }, { 4, 4, 7 } });
                }
                if (!(drawnCorners & FOOTPATH_CORNER_1))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(12), { 0, 0, height }, { { 28, 28, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 13:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(1), { 0, 28, height }, { { 0, 28, height + 2 }, { 32, 1, 7 } });
                if (!(drawnCorners & FOOTPATH_CORNER_2))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(13), { 0, 0, height }, { { 28, 0, height + 2 }, { 4, 4, 7 } });
                }
                if (!(drawnCorners & FOOTPATH_CORNER_3))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(10), { 0, 0, height }, { { 0, 0, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 14:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(0), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 32, 7 } });
                if (!(drawnCorners & FOOTPATH_CORNER_1))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(12), { 0, 0, height }, { { 28, 28, height + 2 }, { 4, 4, 7 } });
                }
                if (!(drawnCorners & FOOTPATH_CORNER_2))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(13), { 0, 0, height }, { { 28, 0, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 11:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(0), { 28, 0, height }, { { 28, 0, height + 2 }, { 1, 32, 7 } });
                if (!(drawnCorners & FOOTPATH_CORNER_0))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(11), { 0, 0, height }, { { 0, 28, height + 2 }, { 4, 4, 7 } });
                }
                if (!(drawnCorners & FOOTPATH_CORNER_3))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(10), { 0, 0, height }, { { 0, 0, height + 2 }, { 4, 4, 7 } });
                }
                break;

            case 15:
                if (!(drawnCorners & FOOTPATH_CORNER_0))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(11), { 0, 0, height }, { { 0, 28, height + 2 }, { 4, 4, 7 } });
                }
                if (!(drawnCorners & FOOTPATH_CORNER_1))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(12), { 0, 0, height }, { { 28, 28, height + 2 }, { 4, 4, 7 } });
                }
                if (!(drawnCorners & FOOTPATH_CORNER_2))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(13), { 0, 0, height }, { { 28, 0, height + 2 }, { 4, 4, 7 } });
                }
                if (!(drawnCorners & FOOTPATH_CORNER_3))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(10), { 0, 0, height }, { { 0, 0, height + 2 }, { 4, 4, 7 } });
                }
                break;
        }
    }
}

/**
 * rct2: 0x006A4101
 * @param tile_element (esi)
 */
static void PathPaintFencesAndQueueBanners(
    PaintSession& session, const PathElement& pathElement, uint16_t height, uint32_t connectedEdges, bool hasSupports,
    const FootpathPaintInfo& pathPaintInfo, ImageId imageTemplate)
{
    PROFILED_FUNCTION();

    if (pathElement.IsQueue())
    {
        PathPaintFencesAndQueueBannersQueue(
            session, pathElement, height, connectedEdges, hasSupports, pathPaintInfo, imageTemplate);
    }
    else
    {
        PathPaintFencesAndQueueBannersNonQueue(
            session, pathElement, height, connectedEdges, hasSupports, pathPaintInfo, imageTemplate);
    }
}

/**
 * rct2: 0x006A3F61
 * @param pathElement (esp[0])
 * @param connectedEdges (bp) (relative to the camera's rotation)
 * @param height (dx)
 * @param pathPaintInfo (0x00F3EF6C)
 * @param imageFlags (0x00F3EF70)
 * @param sceneryImageFlags (0x00F3EF74)
 */
static void Sub6A3F61(
    PaintSession& session, const PathElement& pathElement, uint16_t connectedEdges, uint16_t height,
    const FootpathPaintInfo& pathPaintInfo, ImageId imageTemplate, ImageId sceneryImageTemplate, bool hasSupports)
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
    PROFILED_FUNCTION();

    if (session.DPI.zoom_level <= ZoomLevel{ 1 })
    {
        if (!gTrackDesignSaveMode)
        {
            if (pathElement.HasAddition())
            {
                Sub6A3F61PathAddition(session, pathElement, height, sceneryImageTemplate);
            }
        }

        // Redundant zoom-level check removed

        PathPaintFencesAndQueueBanners(session, pathElement, height, connectedEdges, hasSupports, pathPaintInfo, imageTemplate);
    }

    // This is about tunnel drawing
    uint8_t direction = (pathElement.GetSlopeDirection() + session.CurrentRotation) & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
    bool sloped = pathElement.IsSloped();

    if (connectedEdges & EDGE_SE)
    {
        // Bottom right of tile is a tunnel
        if (sloped && direction == EDGE_NE)
        {
            // Path going down into the tunnel
            PaintUtilPushTunnelRight(session, height + 16, TunnelType::PathAndMiniGolf);
        }
        else if (connectedEdges & EDGE_NE)
        {
            // Flat path with edge to the right (north-east)
            PaintUtilPushTunnelRight(session, height, TunnelType::Path11);
        }
        else
        {
            // Path going up, or flat and not connected to the right
            PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
        }
    }

    if (!(connectedEdges & EDGE_SW))
    {
        return;
    }

    // Bottom left of the tile is a tunnel
    if (sloped && direction == EDGE_SE)
    {
        // Path going down into the tunnel
        PaintUtilPushTunnelLeft(session, height + 16, TunnelType::PathAndMiniGolf);
    }
    else if (connectedEdges & EDGE_NW)
    {
        // Flat path with edge to the left (north-west)
        PaintUtilPushTunnelLeft(session, height, TunnelType::Path11);
    }
    else
    {
        // Path going up, or flat and not connected to the left
        PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
    }
}

static FootpathPaintInfo GetFootpathPaintInfo(const PathElement& pathEl)
{
    FootpathPaintInfo pathPaintInfo;

    const auto* surfaceDescriptor = pathEl.GetSurfaceDescriptor();
    if (surfaceDescriptor != nullptr)
    {
        pathPaintInfo.SurfaceImageId = surfaceDescriptor->Image;
        pathPaintInfo.SurfaceFlags = surfaceDescriptor->Flags;
    }

    const auto* railingsDescriptor = pathEl.GetRailingsDescriptor();
    if (railingsDescriptor != nullptr)
    {
        pathPaintInfo.BridgeImageId = railingsDescriptor->BridgeImage;
        pathPaintInfo.RailingsImageId = railingsDescriptor->RailingsImage;
        pathPaintInfo.RailingFlags = railingsDescriptor->Flags;
        pathPaintInfo.ScrollingMode = railingsDescriptor->ScrollingMode;
        pathPaintInfo.SupportType = railingsDescriptor->SupportType;
        pathPaintInfo.SupportColour = railingsDescriptor->SupportColour;
    }

    return pathPaintInfo;
}

static bool ShouldDrawSupports(PaintSession& session, const PathElement& pathEl, uint16_t height)
{
    auto surface = MapGetSurfaceElementAt(session.MapPosition);
    if (surface == nullptr)
    {
        return true;
    }
    else if (surface->GetBaseZ() != height)
    {
        const auto* surfaceEntry = pathEl.GetSurfaceEntry();
        const bool showUndergroundRailings = surfaceEntry == nullptr
            || !(surfaceEntry->Flags & FOOTPATH_ENTRY_FLAG_NO_SLOPE_RAILINGS);
        if (surface->GetBaseZ() < height || showUndergroundRailings)
            return true;
    }
    else if (pathEl.IsSloped())
    {
        // Diagonal path
        if (surface->GetSlope() != kPathSlopeToLandSlope[pathEl.GetSlopeDirection()])
        {
            return true;
        }
    }
    else if (surface->GetSlope() != kTileSlopeFlat)
    {
        return true;
    }
    return false;
}

static void PaintPatrolAreas(PaintSession& session, const PathElement& pathEl)
{
    auto colour = GetPatrolAreaTileColour(session.MapPosition);
    if (colour)
    {
        uint32_t baseImageIndex = SPR_TERRAIN_STAFF;
        auto patrolAreaBaseZ = pathEl.GetBaseZ();
        if (pathEl.IsSloped())
        {
            baseImageIndex = SPR_TERRAIN_STAFF_SLOPED + ((pathEl.GetSlopeDirection() + session.CurrentRotation) & 3);
            patrolAreaBaseZ += 16;
        }

        auto imageId = ImageId(baseImageIndex, *colour);
        PaintAddImageAsParent(session, imageId, { 16, 16, patrolAreaBaseZ + 2 }, { 1, 1, 0 });
    }
}

static void PaintHeightMarkers(PaintSession& session, const PathElement& pathEl)
{
    PROFILED_FUNCTION();

    if (PaintShouldShowHeightMarkers(session, VIEWPORT_FLAG_PATH_HEIGHTS))
    {
        uint16_t heightMarkerBaseZ = pathEl.GetBaseZ() + 3;
        if (pathEl.IsSloped())
        {
            heightMarkerBaseZ += 8;
        }

        uint32_t baseImageIndex = SPR_HEIGHT_MARKER_BASE;
        baseImageIndex += heightMarkerBaseZ / 16;
        baseImageIndex += GetHeightMarkerOffset();
        baseImageIndex -= kMapBaseZ;
        auto imageId = ImageId(baseImageIndex, COLOUR_GREY);
        PaintAddImageAsParent(session, imageId, { 16, 16, heightMarkerBaseZ }, { 1, 1, 0 });
    }
}

/**
 * rct2: 0x0006A3590
 */
void PaintPath(PaintSession& session, uint16_t height, const PathElement& tileElement)
{
    PROFILED_FUNCTION();

    session.InteractionType = ViewportInteractionItem::Footpath;

    ImageId imageTemplate, sceneryImageTemplate;
    if (gTrackDesignSaveMode)
    {
        // Do not display queues for other rides
        if (tileElement.IsQueue() && tileElement.GetRideIndex() != gTrackDesignSaveRideIndex)
        {
            return;
        }

        if (!TrackDesignSaveContainsTileElement(reinterpret_cast<const TileElement*>(&tileElement)))
        {
            imageTemplate = ImageId().WithRemap(FilterPaletteID::Palette46);
        }
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::Palette46);
    }

    if (tileElement.AdditionIsGhost())
    {
        sceneryImageTemplate = ImageId().WithRemap(FilterPaletteID::PaletteGhost);
    }

    if (tileElement.IsGhost())
    {
        session.InteractionType = ViewportInteractionItem::None;
        imageTemplate = ImageId().WithRemap(FilterPaletteID::PaletteGhost);
    }
    else if (session.SelectedElement == reinterpret_cast<const TileElement*>(&tileElement))
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::PaletteGhost);
        sceneryImageTemplate = ImageId().WithRemap(FilterPaletteID::PaletteGhost);
    }

    // For debugging purpose, show blocked tiles with a colour
    if (gPaintBlockedTiles && tileElement.IsBlockedByVehicle())
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::Palette46);
    }

    // Draw wide flags as ghosts, leaving only the "walkable" paths to be drawn normally
    if (gPaintWidePathsAsGhost && tileElement.IsWide())
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::PaletteGhost);
    }

    PaintPatrolAreas(session, tileElement);
    PaintHeightMarkers(session, tileElement);

    auto hasSupports = ShouldDrawSupports(session, tileElement, height);
    auto pathPaintInfo = GetFootpathPaintInfo(tileElement);
    if (pathPaintInfo.SupportType == RailingEntrySupportType::Pole)
    {
        PathPaintPoleSupport(session, tileElement, height, pathPaintInfo, hasSupports, imageTemplate, sceneryImageTemplate);
    }
    else
    {
        PathPaintBoxSupport(session, tileElement, height, pathPaintInfo, hasSupports, imageTemplate, sceneryImageTemplate);
    }

    PaintLampLightEffects(session, tileElement, height);
}

static std::pair<uint8_t, uint8_t> PathPaintGetRotatedEdgesAndCorners(
    const PaintSession& session, const PathElement& pathElement)
{
    // Rol edges around rotation
    uint8_t edges = ((pathElement.GetEdges() << session.CurrentRotation) & 0xF)
        | (((pathElement.GetEdges()) << session.CurrentRotation) >> 4);

    uint8_t corners = (((pathElement.GetCorners()) << session.CurrentRotation) & 0xF)
        | (((pathElement.GetCorners()) << session.CurrentRotation) >> 4);

    return std::make_pair(edges, corners);
}

static ImageIndex PathPaintGetBaseImage(
    const PaintSession& session, const PathElement& pathElement, const FootpathPaintInfo& pathPaintInfo,
    uint16_t rotatedEdgesAndCorners)
{
    ImageIndex surfaceBaseImageIndex = pathPaintInfo.SurfaceImageId;
    if (pathElement.IsSloped())
    {
        auto directionOffset = (pathElement.GetSlopeDirection() + session.CurrentRotation)
            & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
        surfaceBaseImageIndex += 16 + directionOffset;
    }
    else
    {
        surfaceBaseImageIndex += Byte98D6E0[rotatedEdgesAndCorners];
    }

    return surfaceBaseImageIndex;
}

static BoundBoxXYZ PathPaintGetBoundbox(const PaintSession& session, int32_t height, uint8_t edges)
{
    CoordsXY boundBoxOffset = stru_98D804[edges].offset;
    CoordsXY boundBoxSize = stru_98D804[edges].length;

    const bool hasPassedSurface = (session.Flags & PaintSessionFlags::PassedSurface) != 0;
    if (!hasPassedSurface)
    {
        boundBoxOffset = { 3, 3 };
        boundBoxSize = { 26, 26 };
    }

    // By default, add 1 to the z bounding box to always clip above the surface
    uint8_t boundingBoxZOffset = 1;

    // If we are on the same tile as a straight track, add the offset 2 so we
    //  can clip above gravel part of the track sprite
    if (session.TrackElementOnSameHeight != nullptr
        && session.TrackElementOnSameHeight->AsTrack()->GetTrackType() == TrackElemType::Flat)
    {
        boundingBoxZOffset = 2;
    }

    return BoundBoxXYZ({ boundBoxOffset, height + boundingBoxZOffset }, { boundBoxSize, 0 });
}

static void PathPaintSegmentSupportHeight(
    PaintSession& session, const PathElement& pathElement, int32_t height, uint8_t edges, bool hasSupports)
{
    height += 32;
    if (pathElement.IsSloped())
    {
        height += 16;
    }

    PaintUtilSetGeneralSupportHeight(session, height);

    if (pathElement.IsQueue() || (pathElement.GetEdgesAndCorners() != 0xFF && hasSupports))
    {
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        return;
    }

    if (pathElement.GetEdgesAndCorners() == 0xFF)
    {
        PaintUtilSetSegmentSupportHeight(
            session,
            EnumsToFlags(
                PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::bottomLeftSide,
                PaintSegment::bottomRightSide),
            0xFFFF, 0);
        return;
    }

    PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::centre), 0xFFFF, 0);

    if (edges & EDGE_NE)
    {
        PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::topRightSide), 0xFFFF, 0);
    }

    if (edges & EDGE_SE)
    {
        PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottomRightSide), 0xFFFF, 0);
    }

    if (edges & EDGE_SW)
    {
        PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottomLeftSide), 0xFFFF, 0);
    }

    if (edges & EDGE_NW)
    {
        PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::topLeftSide), 0xFFFF, 0);
    }
}

void PathPaintBoxSupport(
    PaintSession& session, const PathElement& pathElement, int32_t height, const FootpathPaintInfo& pathPaintInfo,
    bool hasSupports, ImageId imageTemplate, ImageId sceneryImageTemplate)
{
    PROFILED_FUNCTION();

    auto [edges, corners] = PathPaintGetRotatedEdgesAndCorners(session, pathElement);
    uint16_t edi = edges | (corners << 4);

    auto surfaceBaseImageIndex = PathPaintGetBaseImage(session, pathElement, pathPaintInfo, edi);
    auto boundbox = PathPaintGetBoundbox(session, height, edges);

    const bool hasPassedSurface = (session.Flags & PaintSessionFlags::PassedSurface) != 0;
    if (!hasSupports || !hasPassedSurface)
    {
        PaintAddImageAsParent(session, imageTemplate.WithIndex(surfaceBaseImageIndex), { 0, 0, height }, boundbox);
    }
    else
    {
        ImageIndex bridgeBaseImageIndex;
        if (pathElement.IsSloped())
        {
            auto directionOffset = (pathElement.GetSlopeDirection() + session.CurrentRotation)
                & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
            bridgeBaseImageIndex = pathPaintInfo.BridgeImageId + 51 + directionOffset;
        }
        else
        {
            bridgeBaseImageIndex = EnumValue(PathSupportOrientation[edges]) + pathPaintInfo.BridgeImageId + 49;
        }

        PaintAddImageAsParent(session, imageTemplate.WithIndex(bridgeBaseImageIndex), { 0, 0, height }, boundbox);

        if (pathElement.IsQueue() || (pathPaintInfo.RailingFlags & RAILING_ENTRY_FLAG_DRAW_PATH_OVER_SUPPORTS))
        {
            PaintAddImageAsChild(session, imageTemplate.WithIndex(surfaceBaseImageIndex), { 0, 0, height }, boundbox);
        }
    }

    Sub6A3F61(session, pathElement, edi, height, pathPaintInfo, imageTemplate, sceneryImageTemplate, hasSupports);

    Direction slopeDirection{};
    if (pathElement.IsSloped())
    {
        slopeDirection = ((pathElement.GetSlopeDirection() + session.CurrentRotation) & 0x3);
    }

    PathBoxSupportsPaintSetup(
        session, PathSupportOrientation[edges], pathElement.IsSloped(), slopeDirection, height, imageTemplate, pathPaintInfo);

    PathPaintSegmentSupportHeight(session, pathElement, height, edges, hasSupports);
}

void PathPaintPoleSupport(
    PaintSession& session, const PathElement& pathElement, int16_t height, const FootpathPaintInfo& pathPaintInfo,
    bool hasSupports, ImageId imageTemplate, ImageId sceneryImageTemplate)
{
    PROFILED_FUNCTION();

    auto [edges, corners] = PathPaintGetRotatedEdgesAndCorners(session, pathElement);
    uint16_t edi = edges | (corners << 4);

    auto surfaceBaseImageIndex = PathPaintGetBaseImage(session, pathElement, pathPaintInfo, edi);
    auto boundbox = PathPaintGetBoundbox(session, height, edges);

    // Below Surface
    const bool hasPassedSurface = (session.Flags & PaintSessionFlags::PassedSurface) != 0;
    if (!hasSupports || !hasPassedSurface)
    {
        PaintAddImageAsParent(session, imageTemplate.WithIndex(surfaceBaseImageIndex), { 0, 0, height }, boundbox);
    }
    else
    {
        ImageIndex bridgeBaseImageIndex;
        if (pathElement.IsSloped())
        {
            bridgeBaseImageIndex = ((pathElement.GetSlopeDirection() + session.CurrentRotation)
                                    & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK)
                + pathPaintInfo.BridgeImageId + 16;
        }
        else
        {
            bridgeBaseImageIndex = edges + pathPaintInfo.BridgeImageId;
        }

        PaintAddImageAsParent(session, imageTemplate.WithIndex(bridgeBaseImageIndex), { 0, 0, height }, boundbox);

        if (pathElement.IsQueue() || (pathPaintInfo.RailingFlags & RAILING_ENTRY_FLAG_DRAW_PATH_OVER_SUPPORTS))
        {
            PaintAddImageAsChild(session, imageTemplate.WithIndex(surfaceBaseImageIndex), { 0, 0, height }, boundbox);
        }
    }

    Sub6A3F61(
        session, pathElement, edi, height, pathPaintInfo, imageTemplate, sceneryImageTemplate,
        hasSupports); // TODO: arguments

    MetalSupportPlace supports[] = {
        MetalSupportPlace::TopRightSide,
        MetalSupportPlace::BottomRightSide,
        MetalSupportPlace::BottomLeftSide,
        MetalSupportPlace::TopLeftSide,
    };

    for (int8_t i = 3; i > -1; --i)
    {
        if (!(edges & (1 << i)))
        {
            // Only colour the supports if not already remapped (e.g. ghost remap)
            auto supportColour = pathPaintInfo.SupportColour;
            if (supportColour != COLOUR_NULL && !imageTemplate.IsRemap())
            {
                imageTemplate = ImageId().WithPrimary(supportColour);
            }
            PathPoleSupportsPaintSetup(session, supports[i], pathElement.IsSloped(), height, imageTemplate, pathPaintInfo);
        }
    }

    PathPaintSegmentSupportHeight(session, pathElement, height, edges, hasSupports);
}
