/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.Path.h"

#include "../../Context.h"
#include "../../GameState.h"
#include "../../config/Config.h"
#include "../../drawing/Drawing.h"
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
#include "../../world/Footpath.h"
#include "../../world/Map.h"
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
using namespace OpenRCT2::Drawing;

struct FootpathPaintInfo
{
    PathSurfaceDescriptor surface{};
    PathRailingsDescriptor railings{};
};

bool gPaintWidePathsAsGhost = false;

const uint8_t kPathSlopeToLandSlope[] = {
    kTileSlopeSWSideUp,
    kTileSlopeNWSideUp,
    kTileSlopeNESideUp,
    kTileSlopeSESideUp,
};

static constexpr uint8_t kPathEdgesAndCornersToSurfaceImageIndexOffset[] = {
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
static constexpr BoundBoxXY kPathBoundingBoxes[] = {
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
    WoodenSupportSubType::neSw, 
    WoodenSupportSubType::neSw, 
    WoodenSupportSubType::nwSe, 
    WoodenSupportSubType::neSw,
    WoodenSupportSubType::neSw, 
    WoodenSupportSubType::neSw, 
    WoodenSupportSubType::nwSe, 
    WoodenSupportSubType::neSw,
    WoodenSupportSubType::nwSe, 
    WoodenSupportSubType::nwSe, 
    WoodenSupportSubType::nwSe, 
    WoodenSupportSubType::nwSe,
    WoodenSupportSubType::neSw, 
    WoodenSupportSubType::neSw, 
    WoodenSupportSubType::nwSe, 
    WoodenSupportSubType::neSw,
};
// clang-format on

static void PathPaintBoxSupport(
    PaintSession& session, const PathElement& pathElement, int32_t height, const FootpathPaintInfo& pathPaintInfo,
    bool hasSupports, ImageId imageTemplate, ImageId sceneryImageTemplate);
static void PathPaintPoleSupport(
    PaintSession& session, const PathElement& pathElement, int16_t height, const FootpathPaintInfo& pathPaintInfo,
    bool hasSupports, ImageId imageTemplate, ImageId sceneryImageTemplate);

static void PathPaintQueueBanner(
    PaintSession& session, const PathElement& pathElement, uint16_t height, const PathRailingsDescriptor& railings,
    ImageId imageTemplate)
{
    auto imageId = imageTemplate.WithIndex(railings.railingsImage);

    uint8_t direction = pathElement.GetQueueBannerDirection();
    // Draw ride sign
    session.InteractionType = ViewportInteractionItem::ride;
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
        uint16_t scrollingMode = railings.scrollingMode;
        scrollingMode += direction;

        auto ft = Formatter();

        if (ride->status == RideStatus::open && !(ride->lifecycleFlags & RIDE_LIFECYCLE_BROKEN_DOWN))
        {
            ft.Add<StringId>(STR_RIDE_ENTRANCE_NAME);
            ride->formatNameTo(ft);
        }
        else
        {
            ft.Add<StringId>(STR_RIDE_ENTRANCE_CLOSED);
        }

        utf8 bannerBuffer[512]{};
        if (Config::Get().general.upperCaseBanners)
        {
            FormatStringToUpper(bannerBuffer, sizeof(bannerBuffer), STR_BANNER_TEXT_FORMAT, ft.Data());
        }
        else
        {
            FormatStringLegacy(bannerBuffer, sizeof(bannerBuffer), STR_BANNER_TEXT_FORMAT, ft.Data());
        }

        uint16_t stringWidth = GfxGetStringWidth(bannerBuffer, FontStyle::tiny);
        uint16_t scroll = stringWidth > 0 ? (getGameState().currentTicks / 2) % stringWidth : 0;

        PaintAddImageAsChild(
            session,
            ScrollingText::setup(session, STR_BANNER_TEXT_FORMAT, ft, scroll, scrollingMode, PaletteIndex::transparent),
            { 0, 0, height + 7 }, { boundBoxOffsets, { 1, 1, 21 } });
    }

    session.InteractionType = ViewportInteractionItem::footpath;
    if (imageTemplate.IsRemap())
    {
        session.InteractionType = ViewportInteractionItem::none;
    }
}

static void PathPaintSlopedFences(
    PaintSession& session, const PathElement& pathElement, uint16_t height, ImageId imageId, bool isQueue)
{
    auto queueOffset = isQueue ? 14 : 0;
    switch ((pathElement.GetSlopeDirection() + session.CurrentRotation) % kNumOrthogonalDirections)
    {
        case 0:
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(8 + queueOffset), { 0, 4, height }, { { 0, 4, height + 2 }, { 32, 1, 23 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(8 + queueOffset), { 0, 28, height }, { { 0, 27, height + 2 }, { 32, 1, 23 } });
            break;
        case 1:
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(7 + queueOffset), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 32, 23 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(7 + queueOffset), { 28, 0, height }, { { 27, 0, height + 2 }, { 1, 32, 23 } });
            break;
        case 2:
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(9 + queueOffset), { 0, 4, height }, { { 0, 4, height + 2 }, { 32, 1, 23 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(9 + queueOffset), { 0, 28, height }, { { 0, 27, height + 2 }, { 32, 1, 23 } });
            break;
        case 3:
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(6 + queueOffset), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 32, 23 } });
            PaintAddImageAsParent(
                session, imageId.WithIndexOffset(6 + queueOffset), { 28, 0, height }, { { 27, 0, height + 2 }, { 1, 32, 23 } });
            break;
    }
}

static void PathPaintFencesAndQueueBannersQueue(
    PaintSession& session, const PathElement& pathElement, uint16_t height, uint32_t connectedEdges, bool hasSupports,
    const PathRailingsDescriptor& railings, ImageId imageTemplate)
{
    auto imageId = imageTemplate.WithIndex(railings.railingsImage);

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
                    session, imageId.WithIndexOffset(17), { 0, 4, height }, { { 0, 4, height + 2 }, { 27, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(17), { 0, 28, height }, { { 0, 27, height + 2 }, { 27, 1, 7 } });
                break;
            case 0b0010:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(18), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 27, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(18), { 28, 0, height }, { { 27, 0, height + 2 }, { 1, 27, 7 } });
                break;
            case 0b0011:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(17), { 0, 4, height }, { { 0, 4, height + 2 }, { 26, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(18), { 28, 0, height }, { { 27, 4, height + 2 }, { 1, 27, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(25), { 0, 0, height }, { { 0, 27, height + 2 }, { 4, 4, 7 } });
                break;
            case 0b0100:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(19), { 0, 4, height }, { { 0, 4, height + 2 }, { 27, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(19), { 0, 28, height }, { { 0, 27, height + 2 }, { 27, 1, 7 } });
                break;
            case 0b0101:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(15), { 0, 4, height }, { { 0, 4, height + 2 }, { 32, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(15), { 0, 28, height }, { { 0, 27, height + 2 }, { 32, 1, 7 } });
                break;
            case 0b0110:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(18), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 27, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(19), { 0, 4, height }, { { 0, 4, height + 2 }, { 27, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(26), { 0, 0, height }, { { 27, 27, height + 2 }, { 4, 4, 7 } });
                break;
            case 0b0111:
                if (pathElement.HasJunctionRailings())
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(15), { 0, 4, height }, { { 0, 4, height + 2 }, { 32, 1, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(25), { 0, 0, height }, { { 0, 27, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(26), { 0, 0, height }, { { 27, 27, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 0b1000:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(16), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 27, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(16), { 28, 0, height }, { { 27, 0, height + 2 }, { 1, 27, 7 } });
                break;
            case 0b1001:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(16), { 28, 0, height }, { { 27, 0, height + 2 }, { 1, 27, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(17), { 0, 28, height }, { { 0, 27, height + 2 }, { 27, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(24), { 0, 0, height }, { { 0, 0, height + 2 }, { 4, 4, 7 } });
                break;
            case 0b1010:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(14), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 32, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(14), { 28, 0, height }, { { 27, 0, height + 2 }, { 1, 32, 7 } });
                break;
            case 0b1011:
                if (pathElement.HasJunctionRailings())
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(14), { 28, 0, height }, { { 27, 0, height + 2 }, { 1, 32, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(24), { 0, 0, height }, { { 0, 0, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(25), { 0, 0, height }, { { 0, 27, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 0b1100:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(16), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 26, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(19), { 0, 28, height }, { { 4, 27, height + 2 }, { 27, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(27), { 0, 0, height }, { { 27, 0, height + 2 }, { 4, 4, 7 } });
                break;
            case 0b1101:
                if (pathElement.HasJunctionRailings())
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(15), { 0, 28, height }, { { 0, 27, height + 2 }, { 32, 1, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(24), { 0, 0, height }, { { 0, 0, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(27), { 0, 0, height }, { { 27, 0, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 0b1110:
                if (pathElement.HasJunctionRailings())
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(14), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 32, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(26), { 0, 0, height }, { { 27, 27, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(27), { 0, 0, height }, { { 27, 0, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 0b1111:
                if (pathElement.HasJunctionRailings())
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(24), { 0, 0, height }, { { 0, 0, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(25), { 0, 0, height }, { { 0, 27, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(26), { 0, 0, height }, { { 27, 27, height + 2 }, { 4, 4, 7 } });
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(27), { 0, 0, height }, { { 27, 0, height + 2 }, { 4, 4, 7 } });
                }
        }
    }

    if (pathElement.HasQueueBanner() && !(railings.flags & RAILING_ENTRY_FLAG_NO_QUEUE_BANNER))
    {
        PathPaintQueueBanner(session, pathElement, height, railings, imageTemplate);
    }
}

static void PathPaintFencesAndQueueBannersNonQueue(
    PaintSession& session, const PathElement& pathElement, uint16_t height, uint32_t connectedEdges, bool hasSupports,
    const FootpathPaintInfo& pathPaintInfo, ImageId imageTemplate)
{
    auto imageId = imageTemplate.WithIndex(pathPaintInfo.railings.railingsImage);

    uint32_t drawnCorners = 0;
    // If the path is not drawn over the supports, then no corner sprites will be drawn (making double-width paths
    // look like connected series of intersections).
    if (pathPaintInfo.railings.flags & RAILING_ENTRY_FLAG_DRAW_PATH_OVER_SUPPORTS)
    {
        drawnCorners = (connectedEdges & FOOTPATH_PROPERTIES_EDGES_CORNERS_MASK) >> 4;
    }

    auto slopeRailingsSupported = !(pathPaintInfo.surface.flags & FOOTPATH_ENTRY_FLAG_NO_SLOPE_RAILINGS);
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
                    session, imageId.WithIndexOffset(3), { 0, 4, height }, { { 0, 4, height + 2 }, { 27, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(3), { 0, 28, height }, { { 0, 27, height + 2 }, { 27, 1, 7 } });
                break;
            case 2:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(4), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 27, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(4), { 28, 0, height }, { { 27, 0, height + 2 }, { 1, 27, 7 } });
                break;
            case 4:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(5), { 0, 4, height }, { { 0, 4, height + 2 }, { 27, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(5), { 0, 28, height }, { { 0, 27, height + 2 }, { 27, 1, 7 } });
                break;
            case 5:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(1), { 0, 4, height }, { { 0, 4, height + 2 }, { 32, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(1), { 0, 28, height }, { { 0, 27, height + 2 }, { 32, 1, 7 } });
                break;
            case 8:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(2), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 27, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(2), { 28, 0, height }, { { 27, 0, height + 2 }, { 1, 27, 7 } });
                break;
            case 10:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(0), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 32, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(0), { 28, 0, height }, { { 27, 0, height + 2 }, { 1, 32, 7 } });
                break;

            case 3:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(3), { 0, 4, height }, { { 0, 4, height + 2 }, { 26, 1, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(4), { 28, 0, height }, { { 27, 4, height + 2 }, { 1, 27, 7 } });
                if (!(drawnCorners & FOOTPATH_CORNER_0))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(11), { 0, 0, height }, { { 0, 27, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 6:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(4), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 27, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(5), { 0, 4, height }, { { 0, 4, height + 2 }, { 27, 1, 7 } });
                if (!(drawnCorners & FOOTPATH_CORNER_1))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(12), { 0, 0, height }, { { 27, 27, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 9:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(2), { 28, 0, height }, { { 27, 0, height + 2 }, { 1, 27, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(3), { 0, 28, height }, { { 0, 27, height + 2 }, { 27, 1, 7 } });
                if (!(drawnCorners & FOOTPATH_CORNER_3))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(10), { 0, 0, height }, { { 0, 0, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 12:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(2), { 4, 0, height }, { { 4, 0, height + 2 }, { 1, 26, 7 } });
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(5), { 0, 28, height }, { { 4, 27, height + 2 }, { 27, 1, 7 } });
                if (!(drawnCorners & FOOTPATH_CORNER_2))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(13), { 0, 0, height }, { { 27, 0, height + 2 }, { 4, 4, 7 } });
                }
                break;

            case 7:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(1), { 0, 4, height }, { { 0, 4, height + 2 }, { 32, 1, 7 } });
                if (!(drawnCorners & FOOTPATH_CORNER_0))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(11), { 0, 0, height }, { { 0, 27, height + 2 }, { 4, 4, 7 } });
                }
                if (!(drawnCorners & FOOTPATH_CORNER_1))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(12), { 0, 0, height }, { { 27, 27, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 13:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(1), { 0, 28, height }, { { 0, 27, height + 2 }, { 32, 1, 7 } });
                if (!(drawnCorners & FOOTPATH_CORNER_2))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(13), { 0, 0, height }, { { 27, 0, height + 2 }, { 4, 4, 7 } });
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
                        session, imageId.WithIndexOffset(12), { 0, 0, height }, { { 27, 27, height + 2 }, { 4, 4, 7 } });
                }
                if (!(drawnCorners & FOOTPATH_CORNER_2))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(13), { 0, 0, height }, { { 27, 0, height + 2 }, { 4, 4, 7 } });
                }
                break;
            case 11:
                PaintAddImageAsParent(
                    session, imageId.WithIndexOffset(0), { 28, 0, height }, { { 27, 0, height + 2 }, { 1, 32, 7 } });
                if (!(drawnCorners & FOOTPATH_CORNER_0))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(11), { 0, 0, height }, { { 0, 27, height + 2 }, { 4, 4, 7 } });
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
                        session, imageId.WithIndexOffset(11), { 0, 0, height }, { { 0, 27, height + 2 }, { 4, 4, 7 } });
                }
                if (!(drawnCorners & FOOTPATH_CORNER_1))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(12), { 0, 0, height }, { { 27, 27, height + 2 }, { 4, 4, 7 } });
                }
                if (!(drawnCorners & FOOTPATH_CORNER_2))
                {
                    PaintAddImageAsParent(
                        session, imageId.WithIndexOffset(13), { 0, 0, height }, { { 27, 0, height + 2 }, { 4, 4, 7 } });
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
            session, pathElement, height, connectedEdges, hasSupports, pathPaintInfo.railings, imageTemplate);
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
static void PathPaintFencesAdditionsTunnels(
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

    if (session.rt.zoom_level <= ZoomLevel{ 1 })
    {
        if (!gTrackDesignSaveMode)
        {
            if (pathElement.HasAddition())
            {
                paintPathAddition(session, pathElement, height, sceneryImageTemplate);
            }
        }

        // Redundant zoom-level check removed

        PathPaintFencesAndQueueBanners(session, pathElement, height, connectedEdges, hasSupports, pathPaintInfo, imageTemplate);
    }

    // This is about tunnel drawing
    uint8_t direction = (pathElement.GetSlopeDirection() + session.CurrentRotation) % kNumOrthogonalDirections;
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
    pathPaintInfo.surface = pathEl.GetSurfaceDescriptor();
    pathPaintInfo.railings = pathEl.GetRailingsDescriptor();

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
        auto imageId = ImageId(baseImageIndex, OpenRCT2::Drawing::Colour::grey);
        PaintAddImageAsParent(session, imageId, { 16, 16, heightMarkerBaseZ }, { 1, 1, 0 });
    }
}

/**
 * rct2: 0x0006A3590
 */
void PaintPath(PaintSession& session, uint16_t height, const PathElement& tileElement)
{
    PROFILED_FUNCTION();

    session.InteractionType = ViewportInteractionItem::footpath;

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
            imageTemplate = ImageId().WithRemap(FilterPaletteID::palette46);
        }
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::palette46);
    }

    if (tileElement.AdditionIsGhost())
    {
        sceneryImageTemplate = ImageId().WithRemap(FilterPaletteID::paletteGhost);
    }

    if (tileElement.IsGhost())
    {
        session.InteractionType = ViewportInteractionItem::none;
        imageTemplate = ImageId().WithRemap(FilterPaletteID::paletteGhost);
    }
    else if (session.SelectedElement == reinterpret_cast<const TileElement*>(&tileElement))
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::paletteGhost);
        sceneryImageTemplate = ImageId().WithRemap(FilterPaletteID::paletteGhost);
    }

    // For debugging purpose, show blocked tiles with a colour
    if (gPaintBlockedTiles && tileElement.IsBlockedByVehicle())
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::palette46);
    }

    // Draw wide flags as ghosts, leaving only the "walkable" paths to be drawn normally
    if (gPaintWidePathsAsGhost && tileElement.IsWide())
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::paletteGhost);
    }

    PaintPatrolAreas(session, tileElement);
    PaintHeightMarkers(session, tileElement);

    auto hasSupports = ShouldDrawSupports(session, tileElement, height);
    auto pathPaintInfo = GetFootpathPaintInfo(tileElement);
    if (pathPaintInfo.railings.supportType == RailingEntrySupportType::pole)
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
    const uint8_t rotatedEdgesAndCorners)
{
    ImageIndex surfaceBaseImageIndex = pathPaintInfo.surface.image;
    if (pathElement.IsSloped())
    {
        auto directionOffset = (pathElement.GetSlopeDirection() + session.CurrentRotation) % kNumOrthogonalDirections;
        surfaceBaseImageIndex += 16 + directionOffset;
    }
    else
    {
        surfaceBaseImageIndex += kPathEdgesAndCornersToSurfaceImageIndexOffset[rotatedEdgesAndCorners];
    }

    return surfaceBaseImageIndex;
}

static BoundBoxXYZ PathPaintGetBoundbox(const PaintSession& session, int32_t height, uint8_t edges)
{
    CoordsXY boundBoxOffset = kPathBoundingBoxes[edges].offset;
    CoordsXY boundBoxSize = kPathBoundingBoxes[edges].length;

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
        && session.TrackElementOnSameHeight->AsTrack()->GetTrackType() == TrackElemType::flat)
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
            EnumsToFlags(PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight),
            0xFFFF, 0);
        return;
    }

    PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::centre), 0xFFFF, 0);

    if (edges & EDGE_NE)
    {
        PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::topRight), 0xFFFF, 0);
    }

    if (edges & EDGE_SE)
    {
        PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottomRight), 0xFFFF, 0);
    }

    if (edges & EDGE_SW)
    {
        PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::bottomLeft), 0xFFFF, 0);
    }

    if (edges & EDGE_NW)
    {
        PaintUtilSetSegmentSupportHeight(session, EnumToFlag(PaintSegment::topLeft), 0xFFFF, 0);
    }
}

static void PathPaintBoxSupport(
    PaintSession& session, const PathElement& pathElement, int32_t height, const FootpathPaintInfo& pathPaintInfo,
    bool hasSupports, ImageId imageTemplate, ImageId sceneryImageTemplate)
{
    PROFILED_FUNCTION();

    auto [edges, corners] = PathPaintGetRotatedEdgesAndCorners(session, pathElement);
    const uint8_t edgesAndCorners = pathElement.IsQueue() ? edges : edges | (corners << 4);

    const auto surfaceBaseImageIndex = PathPaintGetBaseImage(session, pathElement, pathPaintInfo, edgesAndCorners);
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
            auto directionOffset = (pathElement.GetSlopeDirection() + session.CurrentRotation) % kNumOrthogonalDirections;
            bridgeBaseImageIndex = pathPaintInfo.railings.bridgeImage + 51 + directionOffset;
        }
        else
        {
            bridgeBaseImageIndex = EnumValue(PathSupportOrientation[edges]) + pathPaintInfo.railings.bridgeImage + 49;
        }

        PaintAddImageAsParent(session, imageTemplate.WithIndex(bridgeBaseImageIndex), { 0, 0, height }, boundbox);

        if (pathElement.IsQueue() || (pathPaintInfo.railings.flags & RAILING_ENTRY_FLAG_DRAW_PATH_OVER_SUPPORTS))
        {
            PaintAddImageAsChild(session, imageTemplate.WithIndex(surfaceBaseImageIndex), { 0, 0, height }, boundbox);
        }
    }

    PathPaintFencesAdditionsTunnels(
        session, pathElement, edgesAndCorners, height, pathPaintInfo, imageTemplate, sceneryImageTemplate, hasSupports);

    Direction slopeDirection{};
    if (pathElement.IsSloped())
    {
        slopeDirection = ((pathElement.GetSlopeDirection() + session.CurrentRotation) & 0x3);
    }

    PathBoxSupportsPaintSetup(
        session, PathSupportOrientation[edges], pathElement.IsSloped(), slopeDirection, height, imageTemplate,
        pathPaintInfo.railings);

    PathPaintSegmentSupportHeight(session, pathElement, height, edges, hasSupports);
}

static void PathPaintPoleSupport(
    PaintSession& session, const PathElement& pathElement, int16_t height, const FootpathPaintInfo& pathPaintInfo,
    bool hasSupports, ImageId imageTemplate, ImageId sceneryImageTemplate)
{
    PROFILED_FUNCTION();

    auto [edges, corners] = PathPaintGetRotatedEdgesAndCorners(session, pathElement);
    const uint8_t edgesAndCorners = pathElement.IsQueue() ? edges : edges | (corners << 4);

    const auto surfaceBaseImageIndex = PathPaintGetBaseImage(session, pathElement, pathPaintInfo, edgesAndCorners);
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
            bridgeBaseImageIndex = ((pathElement.GetSlopeDirection() + session.CurrentRotation) % kNumOrthogonalDirections)
                + pathPaintInfo.railings.bridgeImage + 16;
        }
        else
        {
            bridgeBaseImageIndex = edges + pathPaintInfo.railings.bridgeImage;
        }

        PaintAddImageAsParent(session, imageTemplate.WithIndex(bridgeBaseImageIndex), { 0, 0, height }, boundbox);

        if (pathElement.IsQueue() || (pathPaintInfo.railings.flags & RAILING_ENTRY_FLAG_DRAW_PATH_OVER_SUPPORTS))
        {
            PaintAddImageAsChild(session, imageTemplate.WithIndex(surfaceBaseImageIndex), { 0, 0, height }, boundbox);
        }
    }

    PathPaintFencesAdditionsTunnels(
        session, pathElement, edgesAndCorners, height, pathPaintInfo, imageTemplate, sceneryImageTemplate,
        hasSupports); // TODO: arguments

    MetalSupportPlace supports[] = {
        MetalSupportPlace::topRightSide,
        MetalSupportPlace::bottomRightSide,
        MetalSupportPlace::bottomLeftSide,
        MetalSupportPlace::topLeftSide,
    };

    for (int8_t i = 3; i > -1; --i)
    {
        if (!(edges & (1 << i)))
        {
            // Only colour the supports if not already remapped (e.g. ghost remap)
            auto supportColour = pathPaintInfo.railings.supportColour;
            if (supportColour != OpenRCT2::Drawing::kColourNull && !imageTemplate.IsRemap())
            {
                imageTemplate = ImageId().WithPrimary(supportColour);
            }
            PathPoleSupportsPaintSetup(
                session, supports[i], pathElement.IsSloped(), height, imageTemplate, pathPaintInfo.railings);
        }
    }

    PathPaintSegmentSupportHeight(session, pathElement, height, edges, hasSupports);
}
