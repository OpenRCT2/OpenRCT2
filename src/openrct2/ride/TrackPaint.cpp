/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackPaint.h"

#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../SpriteIds.h"
#include "../config/Config.h"
#include "../drawing/Drawing.h"
#include "../drawing/LightFX.h"
#include "../interface/Viewport.h"
#include "../object/StationObject.h"
#include "../paint/Boundbox.h"
#include "../paint/Paint.SessionFlags.h"
#include "../paint/Paint.h"
#include "../paint/support/MetalSupports.h"
#include "../paint/support/WoodenSupports.h"
#include "../paint/tile_element/Paint.TileElement.h"
#include "../paint/tile_element/Paint.Track.h"
#include "../paint/tile_element/Segment.h"
#include "../paint/track/Segment.h"
#include "../paint/track/Support.h"
#include "../world/tile_element/TrackElement.h"
#include "RideData.h"
#include "Station.h"
#include "TrackData.h"
#include "TrackDesign.h"
#include "TrackStyle.h"
#include "ted/TrackElementDescriptor.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::TrackMetadata;

/* rct2: 0x007667AC */
static constexpr TileCoordsXY EntranceOffsetEdgeNE[] = {
    { -1, 0 },
    { 0, -1 },
    { 1, 0 },
    { 0, 1 },
};

/* rct2: 0x007667AE */
static constexpr TileCoordsXY EntranceOffsetEdgeNW[] = {
    { 0, -1 },
    { 1, 0 },
    { 0, 1 },
    { -1, 0 },
};

static constexpr uint32_t trackSpritesGhostTrainSpinningTunnel[2][2][4] = {
    {
        {
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_SW_NE_FRAME_0,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_SW_NE_FRAME_1,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_SW_NE_FRAME_2,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_SW_NE_FRAME_3,
        },
        {
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_SW_NE_FRAME_0,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_SW_NE_FRAME_1,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_SW_NE_FRAME_2,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_SW_NE_FRAME_3,
        },
    },
    {
        {
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_NW_SE_FRAME_0,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_NW_SE_FRAME_1,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_NW_SE_FRAME_2,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_NW_SE_FRAME_3,
        },
        {
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_NW_SE_FRAME_0,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_NW_SE_FRAME_1,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_NW_SE_FRAME_2,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_NW_SE_FRAME_3,
        },
    },
};

bool TrackPaintUtilHasFence(
    enum edge_t edge, const CoordsXY& position, const TrackElement& trackElement, const Ride& ride, uint8_t rotation)
{
    const auto* stationObject = ride.getStationObject();
    if (stationObject != nullptr && stationObject->Flags & StationObjectFlags::noPlatforms)
        return false;

    TileCoordsXY offset;
    switch (edge)
    {
        case edgeNE:
            offset = EntranceOffsetEdgeNE[rotation];
            break;
        case edgeSE:
            offset = EntranceOffsetEdgeNW[(rotation + 2) & 3];
            break;
        case edgeSW:
            offset = EntranceOffsetEdgeNE[(rotation + 2) & 3];
            break;
        case edgeNW:
            offset = EntranceOffsetEdgeNW[rotation];
            break;
    }

    auto entranceLoc = TileCoordsXY(position) + offset;
    auto entranceId = trackElement.GetStationIndex();
    const auto& station = ride.getStation(entranceId);

    return (entranceLoc != station.Entrance && entranceLoc != station.Exit);
}

void TrackPaintUtilPaintFloor(
    PaintSession& session, uint8_t edges, ImageId colourFlags, uint16_t height, const uint32_t floorSprites[4],
    const StationObject* stationStyle)
{
    if (stationStyle != nullptr && stationStyle->Flags & StationObjectFlags::noPlatforms)
        return;

    uint32_t imageId;

    if (edges & edgeSW && edges & edgeSE)
    {
        imageId = floorSprites[0];
    }
    else if (edges & edgeSW)
    {
        imageId = floorSprites[1];
    }
    else if (edges & edgeSE)
    {
        imageId = floorSprites[2];
    }
    else
    {
        imageId = floorSprites[3];
    }

    paintAddImageAsParent(session, colourFlags.WithIndex(imageId), { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
}

void TrackPaintUtilPaintFences(
    PaintSession& session, uint8_t edges, const CoordsXY& position, const TrackElement& trackElement, const Ride& ride,
    const ImageId colourFlags, uint16_t height, const uint32_t fenceSprites[4], uint8_t rotation)
{
    if (edges & edgeNW && TrackPaintUtilHasFence(edgeNW, position, trackElement, ride, rotation))
    {
        paintAddImageAsChild(
            session, colourFlags.WithIndex(fenceSprites[3]), { 0, 0, height }, { { 0, 2, height + 2 }, { 32, 1, 7 } });
    }
    if (edges & edgeNE && TrackPaintUtilHasFence(edgeNE, position, trackElement, ride, rotation))
    {
        paintAddImageAsChild(
            session, colourFlags.WithIndex(fenceSprites[0]), { 0, 0, height }, { { 2, 0, height + 2 }, { 1, 32, 7 } });
    }
    if (edges & edgeSE && TrackPaintUtilHasFence(edgeSE, position, trackElement, ride, rotation))
    {
        paintAddImageAsParent(
            session, colourFlags.WithIndex(fenceSprites[1]), { 0, 0, height }, { { 0, 30, height + 2 }, { 32, 1, 7 } });
    }
    if (edges & edgeSW && TrackPaintUtilHasFence(edgeSW, position, trackElement, ride, rotation))
    {
        paintAddImageAsParent(
            session, colourFlags.WithIndex(fenceSprites[2]), { 0, 0, height }, { { 30, 0, height + 2 }, { 1, 32, 7 } });
    }
}

/* Supports are only placed every 2 tiles for flat pieces*/
bool TrackPaintUtilShouldPaintSupports(const CoordsXY& position)
{
    if ((position.x & (1 << 5)) == (position.y & (1 << 5)))
        return true;

    if ((!(position.x & (1 << 5))) && (!(position.y & (1 << 5))))
        return true;

    return false;
}

static constexpr ImageIndex kStationBaseTypeSpriteIndexes[][2] = {
    { kImageIndexUndefined, kImageIndexUndefined },
    { SPR_STATION_BASE_TALL_SW_NE, SPR_STATION_BASE_TALL_NW_SE },
    { SPR_STATION_BASE_MEDIUM_SW_NE, SPR_STATION_BASE_MEDIUM_NW_SE },
    { SPR_STATION_BASE_SHORT_SW_NE, SPR_STATION_BASE_SHORT_NW_SE },
};
static_assert(std::size(kStationBaseTypeSpriteIndexes) == kStationBaseTypeCount);

static bool TrackPaintUtilDrawStationImpl(
    PaintSession& session, const Ride& ride, Direction direction, uint16_t height, uint16_t coverHeight,
    const TrackElement& trackElement, StationBaseType baseType, int32_t baseOffsetZ, int32_t fenceOffsetA,
    int32_t fenceOffsetB);

bool TrackPaintUtilDrawStation(
    PaintSession& session, const Ride& ride, Direction direction, uint16_t height, const TrackElement& trackElement,
    const StationBaseType baseType, const int32_t baseOffsetZ)
{
    return TrackPaintUtilDrawStationImpl(session, ride, direction, height, height, trackElement, baseType, baseOffsetZ, 5, 7);
}

bool TrackPaintUtilDrawStation2(
    PaintSession& session, const Ride& ride, Direction direction, uint16_t height, const TrackElement& trackElement,
    const StationBaseType baseType, const int32_t baseOffsetZ, int32_t fenceOffsetA, int32_t fenceOffsetB)
{
    return TrackPaintUtilDrawStationImpl(
        session, ride, direction, height, height, trackElement, baseType, baseOffsetZ, fenceOffsetA, fenceOffsetB);
}

static bool TrackPaintUtilDrawStationImpl(
    PaintSession& session, const Ride& ride, Direction direction, uint16_t height, uint16_t coverHeight,
    const TrackElement& trackElement, const StationBaseType baseType, const int32_t baseOffsetZ, int32_t fenceOffsetA,
    int32_t fenceOffsetB)
{
    CoordsXY position = session.mapPosition;
    const auto* stationObj = ride.getStationObject();
    const bool hasGreenLight = trackElement.HasGreenLight();

    if (stationObj != nullptr && stationObj->Flags & StationObjectFlags::noPlatforms)
        return false;

    bool hasFence;
    ImageId imageId;

    ImageId stationColourScheme = GetStationColourScheme(session, trackElement);

    if (direction == 0 || direction == 2)
    {
        paintAddImageAsParent(
            session, stationColourScheme.WithIndex(kStationBaseTypeSpriteIndexes[EnumValue(baseType)][0]),
            { 0, 0, height + baseOffsetZ }, { { 0, 2, height }, { 32, 28, 1 } });

        // height += 5 (height + 5);
        hasFence = TrackPaintUtilHasFence(edgeNW, position, trackElement, ride, session.currentRotation);

        if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 0)
        {
            if (hasGreenLight)
            {
                imageId = session.supportColours.WithIndex(
                    hasFence ? SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_SW_NE : SPR_STATION_PLATFORM_END_GREEN_LIGHT_SW_NE);
            }
            else
            {
                imageId = session.supportColours.WithIndex(
                    hasFence ? SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_SW_NE : SPR_STATION_PLATFORM_END_RED_LIGHT_SW_NE);
            }
        }
        else if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 2)
        {
            imageId = session.supportColours.WithIndex(
                (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_SW_NE : SPR_STATION_PLATFORM_BEGIN_SW_NE));
        }
        else
        {
            imageId = session.supportColours.WithIndex(
                (hasFence ? SPR_STATION_PLATFORM_FENCED_SW_NE : SPR_STATION_PLATFORM_SW_NE));
        }
        paintAddImageAsParent(session, imageId, { 0, 0, height + fenceOffsetA }, { 32, 8, 1 });
        // height -= 5 (height)
        TrackPaintUtilDrawStationCovers(session, edgeNW, hasFence, stationObj, coverHeight, stationColourScheme);
        // height += 5 (height + 5)

        if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 0)
        {
            imageId = session.supportColours.WithIndex(
                (hasGreenLight ? SPR_STATION_PLATFORM_END_GREEN_LIGHT_SW_NE : SPR_STATION_PLATFORM_END_RED_LIGHT_SW_NE));
        }
        else if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 2)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_PLATFORM_BEGIN_SW_NE);
        }
        else
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_PLATFORM_SW_NE);
        }
        paintAddImageAsParent(session, imageId, { 0, 24, height + fenceOffsetA }, { 32, 8, 1 });
        // height += 2 (height + 7)

        hasFence = TrackPaintUtilHasFence(edgeSE, position, trackElement, ride, session.currentRotation);
        if (hasFence)
        {
            if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 0)
            {
                imageId = session.supportColours.WithIndex(SPR_STATION_BEGIN_ANGLE_FENCE_SW_NE);
            }
            else if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 2)
            {
                imageId = session.supportColours.WithIndex(SPR_STATION_LIGHT_BACK_ANGLE_FENCED_NE_SW);
            }
            else
            {
                imageId = session.supportColours.WithIndex(SPR_STATION_FENCE_SW_NE);
            }
            paintAddImageAsParent(session, imageId, { 0, 31, height + fenceOffsetB }, { 32, 1, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 0)
        {
            // Addition: draw only small fence if there is an entrance/exit at the beginning
            imageId = session.supportColours.WithIndex(SPR_STATION_FENCE_SMALL_NW_SE);
            paintAddImageAsParent(session, imageId, { 31, 23, height + fenceOffsetB }, { 1, 8, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 2)
        {
            // Addition: draw only small fence if there is an entrance/exit at the end
            imageId = session.supportColours.WithIndex(SPR_STATION_LIGHT_BACK_NE_SW);
            paintAddImageAsParent(session, imageId, { 31, 23, height + fenceOffsetB }, { 1, 8, 7 });
        }
        // height -= 7 (height)
        TrackPaintUtilDrawStationCovers(session, edgeSE, hasFence, stationObj, coverHeight, stationColourScheme);
        // height += 7 (height + 7)

        if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 0)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_FENCE_SMALL_NW_SE);
            paintAddImageAsParent(session, imageId, { 31, 0, height + fenceOffsetB }, { 1, 8, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 2)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_LIGHT_BACK_NE_SW);
            paintAddImageAsParent(session, imageId, { 31, 0, height + fenceOffsetB }, { 1, 8, 7 });
        }
    }
    else if (direction == 1 || direction == 3)
    {
        paintAddImageAsParent(
            session, stationColourScheme.WithIndex(kStationBaseTypeSpriteIndexes[EnumValue(baseType)][1]),
            { 0, 0, height + baseOffsetZ }, { { 2, 0, height }, { 28, 32, 1 } });

        // height += 5 (height + 5);
        hasFence = TrackPaintUtilHasFence(edgeNE, position, trackElement, ride, session.currentRotation);

        if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 3)
        {
            if (hasGreenLight)
            {
                imageId = session.supportColours.WithIndex(
                    hasFence ? SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_NW_SE : SPR_STATION_PLATFORM_END_GREEN_LIGHT_NW_SE);
            }
            else
            {
                imageId = session.supportColours.WithIndex(
                    hasFence ? SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_NW_SE : SPR_STATION_PLATFORM_END_RED_LIGHT_NW_SE);
            }
        }
        else if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 1)
        {
            imageId = session.supportColours.WithIndex(
                (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_NW_SE : SPR_STATION_PLATFORM_BEGIN_NW_SE));
        }
        else
        {
            imageId = session.supportColours.WithIndex(
                (hasFence ? SPR_STATION_PLATFORM_FENCED_NW_SE : SPR_STATION_PLATFORM_NW_SE));
        }
        paintAddImageAsParent(session, imageId, { 0, 0, height + fenceOffsetA }, { 8, 32, 1 });
        // height -= 5 (height)
        TrackPaintUtilDrawStationCovers(session, edgeNE, hasFence, stationObj, coverHeight, stationColourScheme);
        // height += 5 (height + 5)

        if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 3)
        {
            imageId = session.supportColours.WithIndex(
                (hasGreenLight ? SPR_STATION_PLATFORM_END_GREEN_LIGHT_NW_SE : SPR_STATION_PLATFORM_END_RED_LIGHT_NW_SE));
        }
        else if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 1)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_PLATFORM_BEGIN_NW_SE);
        }
        else
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_PLATFORM_NW_SE);
        }
        paintAddImageAsParent(session, imageId, { 24, 0, height + fenceOffsetA }, { 8, 32, 1 });
        // height += 2 (height + 7)

        hasFence = TrackPaintUtilHasFence(edgeSW, position, trackElement, ride, session.currentRotation);
        if (hasFence)
        {
            if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 3)
            {
                imageId = session.supportColours.WithIndex(SPR_STATION_BEGIN_ANGLE_FENCE_NW_SE);
            }
            else if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 1)
            {
                imageId = session.supportColours.WithIndex(SPR_STATION_LIGHT_BACK_ANGLE_FENCED_NW_SE);
            }
            else
            {
                imageId = session.supportColours.WithIndex(SPR_STATION_FENCE_NW_SE);
            }
            paintAddImageAsParent(session, imageId, { 31, 0, height + fenceOffsetB }, { 1, 32, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 3)
        {
            // Addition: draw only small fence if there is an entrance/exit at the beginning
            imageId = session.supportColours.WithIndex(SPR_STATION_FENCE_SMALL_SW_NE);
            paintAddImageAsParent(session, imageId, { 23, 31, height + fenceOffsetB }, { 8, 1, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 1)
        {
            // Addition: draw only small fence if there is an entrance/exit at the end
            imageId = session.supportColours.WithIndex(SPR_STATION_LIGHT_BACK_NW_SE);
            paintAddImageAsParent(session, imageId, { 23, 31, height + fenceOffsetB }, { 8, 1, 7 });
        }

        // height -= 7 (height)
        TrackPaintUtilDrawStationCovers(session, edgeSW, hasFence, stationObj, coverHeight, stationColourScheme);
        // height += 7 (height + 7)

        if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 3)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_FENCE_SMALL_SW_NE);
            paintAddImageAsParent(session, imageId, { 0, 31, height + fenceOffsetB }, { 8, 1, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 1)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_LIGHT_BACK_NW_SE);
            paintAddImageAsParent(session, imageId, { 0, 31, height + fenceOffsetB }, { 8, 1, 7 });
        }
    }
    return true;
}

bool TrackPaintUtilDrawStationInverted(
    PaintSession& session, const Ride& ride, Direction direction, int32_t height, const TrackElement& trackElement,
    uint8_t stationVariant)
{
    CoordsXY position = session.mapPosition;
    const auto* stationObj = ride.getStationObject();
    const bool hasGreenLight = trackElement.HasGreenLight();

    if (stationObj != nullptr && stationObj->Flags & StationObjectFlags::noPlatforms)
        return false;

    auto colour = GetStationColourScheme(session, trackElement);
    bool hasFence;
    ImageId imageId;

    if (direction == 0 || direction == 2)
    {
        paintAddImageAsParent(
            session, colour.WithIndex(kStationBaseTypeSpriteIndexes[EnumValue(StationBaseType::c)][0]), { 0, 0, height },
            { { 0, 2, height }, { 32, 28, 1 } });

        // height += 5 (height + 5);
        hasFence = TrackPaintUtilHasFence(edgeNW, position, trackElement, ride, session.currentRotation);

        if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 0)
        {
            if (hasGreenLight)
            {
                imageId = session.supportColours.WithIndex(
                    hasFence ? SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_SW_NE : SPR_STATION_PLATFORM_END_GREEN_LIGHT_SW_NE);
            }
            else
            {
                imageId = session.supportColours.WithIndex(
                    hasFence ? SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_SW_NE : SPR_STATION_PLATFORM_END_RED_LIGHT_SW_NE);
            }
        }
        else if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 2)
        {
            imageId = session.supportColours.WithIndex(
                (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_SW_NE : SPR_STATION_PLATFORM_BEGIN_SW_NE));
        }
        else
        {
            imageId = session.supportColours.WithIndex(
                (hasFence ? SPR_STATION_PLATFORM_FENCED_SW_NE : SPR_STATION_PLATFORM_SW_NE));
        }
        paintAddImageAsParent(session, imageId, { 0, 0, height + 6 }, { 32, 8, 1 });
        // height -= 5 (height)
        TrackPaintUtilDrawStationCovers2(session, edgeNW, hasFence, stationObj, height, stationVariant, colour);
        // height += 5 (height + 5)

        if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 0)
        {
            imageId = session.supportColours.WithIndex(
                (hasGreenLight ? SPR_STATION_PLATFORM_END_GREEN_LIGHT_SW_NE : SPR_STATION_PLATFORM_END_RED_LIGHT_SW_NE));
        }
        else if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 2)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_PLATFORM_BEGIN_SW_NE);
        }
        else
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_PLATFORM_SW_NE);
        }
        paintAddImageAsParent(session, imageId, { 0, 24, height + 6 }, { 32, 8, 1 });
        // height += 2 (height + 7)

        hasFence = TrackPaintUtilHasFence(edgeSE, position, trackElement, ride, session.currentRotation);
        if (hasFence)
        {
            if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 0)
            {
                imageId = session.supportColours.WithIndex(SPR_STATION_INVERTED_BEGIN_ANGLE_FENCE_SW_NE);
            }
            else if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 2)
            {
                imageId = session.supportColours.WithIndex(SPR_STATION_INVERTED_LIGHT_BACK_ANGLE_FENCED_NE_SW);
            }
            else
            {
                imageId = session.supportColours.WithIndex(SPR_STATION_INVERTED_FENCE_SW_NE);
            }
            paintAddImageAsParent(session, imageId, { 0, 31, height + 8 }, { 32, 1, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 0)
        {
            // Addition: draw only small fence if there is an entrance/exit at the beginning
            imageId = session.supportColours.WithIndex(SPR_STATION_FENCE_SMALL_NW_SE);
            paintAddImageAsParent(session, imageId, { 31, 23, height + 8 }, { 1, 8, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 2)
        {
            // Addition: draw only small fence if there is an entrance/exit at the end
            imageId = session.supportColours.WithIndex(SPR_STATION_LIGHT_BACK_NE_SW);
            paintAddImageAsParent(session, imageId, { 31, 23, height + 8 }, { 1, 8, 7 });
        }
        // height -= 7 (height)
        TrackPaintUtilDrawStationCovers2(session, edgeSE, hasFence, stationObj, height, stationVariant, colour);
        // height += 7 (height + 7)

        if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 0)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_FENCE_SMALL_NW_SE);
            paintAddImageAsParent(session, imageId, { 31, 0, height + 8 }, { 1, 8, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 2)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_LIGHT_BACK_NE_SW);
            paintAddImageAsParent(session, imageId, { 31, 0, height + 8 }, { 1, 8, 7 });
        }
    }
    else if (direction == 1 || direction == 3)
    {
        paintAddImageAsParent(
            session, colour.WithIndex(kStationBaseTypeSpriteIndexes[EnumValue(StationBaseType::c)][1]), { 0, 0, height },
            { { 2, 0, height }, { 28, 32, 1 } });

        // height += 5 (height + 5);
        hasFence = TrackPaintUtilHasFence(edgeNE, position, trackElement, ride, session.currentRotation);

        if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 3)
        {
            if (hasGreenLight)
            {
                imageId = session.supportColours.WithIndex(
                    hasFence ? SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_NW_SE : SPR_STATION_PLATFORM_END_GREEN_LIGHT_NW_SE);
            }
            else
            {
                imageId = session.supportColours.WithIndex(
                    hasFence ? SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_NW_SE : SPR_STATION_PLATFORM_END_RED_LIGHT_NW_SE);
            }
        }
        else if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 1)
        {
            imageId = session.supportColours.WithIndex(
                (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_NW_SE : SPR_STATION_PLATFORM_BEGIN_NW_SE));
        }
        else
        {
            imageId = session.supportColours.WithIndex(
                (hasFence ? SPR_STATION_PLATFORM_FENCED_NW_SE : SPR_STATION_PLATFORM_NW_SE));
        }
        paintAddImageAsParent(session, imageId, { 0, 0, height + 6 }, { 8, 32, 1 });
        // height -= 5 (height)
        TrackPaintUtilDrawStationCovers2(session, edgeNE, hasFence, stationObj, height, stationVariant, colour);
        // height += 5 (height + 5)

        if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 3)
        {
            imageId = session.supportColours.WithIndex(
                (hasGreenLight ? SPR_STATION_PLATFORM_END_GREEN_LIGHT_NW_SE : SPR_STATION_PLATFORM_END_RED_LIGHT_NW_SE));
        }
        else if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 1)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_PLATFORM_BEGIN_NW_SE);
        }
        else
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_PLATFORM_NW_SE);
        }
        paintAddImageAsParent(session, imageId, { 24, 0, height + 6 }, { 8, 32, 1 });
        // height += 2 (height + 7)

        hasFence = TrackPaintUtilHasFence(edgeSW, position, trackElement, ride, session.currentRotation);
        if (hasFence)
        {
            if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 3)
            {
                imageId = session.supportColours.WithIndex(SPR_STATION_INVERTED_BEGIN_ANGLE_FENCE_NW_SE);
            }
            else if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 1)
            {
                imageId = session.supportColours.WithIndex(SPR_STATION_INVERTED_LIGHT_BACK_ANGLE_FENCED_NW_SE);
            }
            else
            {
                imageId = session.supportColours.WithIndex(SPR_STATION_INVERTED_FENCE_NW_SE);
            }
            paintAddImageAsParent(session, imageId, { 31, 0, height + 8 }, { 1, 32, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 3)
        {
            // Addition: draw only small fence if there is an entrance/exit at the beginning
            imageId = session.supportColours.WithIndex(SPR_STATION_FENCE_SMALL_SW_NE);
            paintAddImageAsParent(session, imageId, { 23, 31, height + 8 }, { 8, 1, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 1)
        {
            // Addition: draw only small fence if there is an entrance/exit at the end
            imageId = session.supportColours.WithIndex(SPR_STATION_LIGHT_BACK_NW_SE);
            paintAddImageAsParent(session, imageId, { 23, 31, height + 8 }, { 8, 1, 7 });
        }

        // height -= 7 (height)
        TrackPaintUtilDrawStationCovers2(session, edgeSW, hasFence, stationObj, height, stationVariant, colour);
        // height += 7 (height + 7)

        if (trackElement.GetTrackType() == TrackElemType::beginStation && direction == 3)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_FENCE_SMALL_SW_NE);
            paintAddImageAsParent(session, imageId, { 0, 31, height + 8 }, { 8, 1, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::endStation && direction == 1)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_LIGHT_BACK_NW_SE);
            paintAddImageAsParent(session, imageId, { 0, 31, height + 8 }, { 8, 1, 7 });
        }
    }
    return true;
}

bool TrackPaintUtilDrawStationCovers(
    PaintSession& session, enum edge_t edge, bool hasFence, const StationObject* stationObject, uint16_t height, ImageId colour)
{
    return TrackPaintUtilDrawStationCovers2(session, edge, hasFence, stationObject, height, STATION_VARIANT_BASIC, colour);
}

bool TrackPaintUtilDrawStationCovers2(
    PaintSession& session, enum edge_t edge, bool hasFence, const StationObject* stationObject, uint16_t height,
    uint8_t stationVariant, ImageId colour)
{
    if (stationObject == nullptr)
    {
        return false;
    }

    if (!(session.flags & (PaintSessionFlags::PassedSurface | PaintSessionFlags::IsTrackPiecePreview)))
    {
        return false;
    }

    auto shelterImageIndex = stationObject->shelterIndex;
    if (shelterImageIndex == kImageIndexUndefined)
        return false;

    static constexpr int16_t heights[][2] = {
        { 22, 0 },
        { 30, 0 },
        { 46, 0 },
    };

    int32_t imageOffset = 0;
    BoundBoxXYZ boundBox;
    CoordsXYZ offset = CoordsXYZ(0, 0, height);
    switch (edge)
    {
        case edgeNE:
            boundBox = { { 0, 1, height + 1 }, { 1, 30, heights[stationVariant][0] } };
            imageOffset = hasFence ? SPR_STATION_COVER_OFFSET_SE_NW_BACK_1 : SPR_STATION_COVER_OFFSET_SE_NW_BACK_0;
            break;
        case edgeSE:
            boundBox = { { 0, 0, height + 1 + heights[stationVariant][0] }, { 32, 32, 0 } };
            imageOffset = SPR_STATION_COVER_OFFSET_NE_SW_FRONT;
            break;
        case edgeSW:
            boundBox = { { 0, 0, height + 1 + heights[stationVariant][0] }, { 32, 32, 0 } };
            imageOffset = SPR_STATION_COVER_OFFSET_SE_NW_FRONT;
            break;
        case edgeNW:
            boundBox = { { 1, 0, height + 1 }, { 30, 1, heights[stationVariant][0] } };
            imageOffset = hasFence ? SPR_STATION_COVER_OFFSET_NE_SW_BACK_1 : SPR_STATION_COVER_OFFSET_NE_SW_BACK_0;
            break;
    }

    if (stationVariant == STATION_VARIANT_TALL)
    {
        imageOffset += SPR_STATION_COVER_OFFSET_TALL;
    }

    auto imageId = session.trackColours.WithIndex(shelterImageIndex + imageOffset);
    if (!session.trackColours.IsRemap())
    {
        imageId = ImageId(shelterImageIndex + imageOffset);
        if (stationObject->Flags & StationObjectFlags::hasPrimaryColour)
            imageId = imageId.WithPrimary(session.trackColours.GetPrimary());
        if (stationObject->Flags & StationObjectFlags::hasSecondaryColour)
            imageId = imageId.WithSecondary(session.trackColours.GetSecondary());
    }

    paintAddImageAsParent(session, imageId, offset, boundBox);

    // Glass
    if (colour == TrackStationColour && (stationObject->Flags & StationObjectFlags::isTransparent))
    {
        auto shelterGlassImageIndex = stationObject->shelterGlassIndex;
        imageId = ImageId(shelterGlassImageIndex + imageOffset).WithTransparency(session.trackColours.GetPrimary());
        paintAddImageAsChild(session, imageId, offset, boundBox);
    }
    return true;
}

bool TrackPaintUtilDrawNarrowStationPlatform(
    PaintSession& session, const Ride& ride, Direction direction, int32_t height, int32_t zOffset,
    const TrackElement& trackElement, const StationBaseType baseType, const int32_t baseOffsetZ)
{
    CoordsXY position = session.mapPosition;
    const auto* stationObj = ride.getStationObject();
    if (stationObj != nullptr && stationObj->Flags & StationObjectFlags::noPlatforms)
        return false;

    auto colour = GetStationColourScheme(session, trackElement);
    if (direction & 1)
    {
        paintAddImageAsParent(
            session, colour.WithIndex(kStationBaseTypeSpriteIndexes[EnumValue(baseType)][1]), { 0, 0, height + baseOffsetZ },
            { { 0, 2, height }, { 32, 28, 1 } });

        bool hasFence = TrackPaintUtilHasFence(edgeNE, position, trackElement, ride, session.currentRotation);
        ImageId imageId = session.supportColours.WithIndex(
            (hasFence ? SPR_STATION_NARROW_EDGE_FENCED_NE : SPR_STATION_NARROW_EDGE_NE));
        paintAddImageAsParent(session, imageId, { 0, 0, height + zOffset }, { 8, 32, 1 });
        TrackPaintUtilDrawStationCovers(session, edgeNE, hasFence, stationObj, height, colour);

        imageId = session.supportColours.WithIndex(SPR_STATION_NARROW_EDGE_SW);
        paintAddImageAsParent(session, imageId, { 24, 0, height + zOffset }, { 8, 32, 1 });

        hasFence = TrackPaintUtilHasFence(edgeSW, position, trackElement, ride, session.currentRotation);
        if (hasFence)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_FENCE_NW_SE);
            paintAddImageAsParent(session, imageId, { 31, 0, height + zOffset + 2 }, { 1, 32, 7 });
        }
        TrackPaintUtilDrawStationCovers(session, edgeSW, hasFence, stationObj, height, colour);
    }
    else
    {
        paintAddImageAsParent(
            session, colour.WithIndex(kStationBaseTypeSpriteIndexes[EnumValue(baseType)][0]), { 0, 0, height + baseOffsetZ },
            { { 2, 0, height }, { 28, 32, 1 } });

        bool hasFence = TrackPaintUtilHasFence(edgeNW, position, trackElement, ride, session.currentRotation);
        ImageId imageId = session.supportColours.WithIndex(
            (hasFence ? SPR_STATION_NARROW_EDGE_FENCED_NW : SPR_STATION_NARROW_EDGE_NW));
        paintAddImageAsParent(session, imageId, { 0, 0, height + zOffset }, { 32, 8, 1 });
        TrackPaintUtilDrawStationCovers(session, edgeNW, hasFence, stationObj, height, colour);

        imageId = session.supportColours.WithIndex(SPR_STATION_NARROW_EDGE_SE);
        paintAddImageAsParent(session, imageId, { 0, 24, height + zOffset }, { 32, 8, 1 });

        hasFence = TrackPaintUtilHasFence(edgeSE, position, trackElement, ride, session.currentRotation);
        if (hasFence)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_FENCE_SW_NE);
            paintAddImageAsParent(session, imageId, { 0, 31, height + zOffset + 2 }, { 32, 1, 7 });
        }
        TrackPaintUtilDrawStationCovers(session, edgeSE, hasFence, stationObj, height, colour);
    }
    return true;
}

void TrackPaintUtilDrawPier(
    PaintSession& session, const Ride& ride, const StationObject* stationObj, const CoordsXY& position, Direction direction,
    int32_t height, const TrackElement& trackElement, uint8_t rotation)
{
    if (stationObj != nullptr && stationObj->Flags & StationObjectFlags::noPlatforms)
        return;
    auto colour = GetStationColourScheme(session, trackElement);
    bool hasFence;
    ImageId imageId;

    if (direction & 1)
    {
        hasFence = TrackPaintUtilHasFence(edgeNE, position, trackElement, ride, session.currentRotation);
        imageId = session.supportColours.WithIndex((hasFence ? SPR_STATION_PIER_EDGE_NE_FENCED : SPR_STATION_PIER_EDGE_NE));
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 6, 32, 1 } });
        TrackPaintUtilDrawStationCovers(session, edgeNE, hasFence, stationObj, height, colour);

        imageId = session.supportColours.WithIndex(SPR_STATION_PIER_EDGE_SW);
        paintAddImageAsParent(session, imageId, { 24, 0, height }, { 8, 32, 1 });

        hasFence = TrackPaintUtilHasFence(edgeSW, position, trackElement, ride, session.currentRotation);
        if (hasFence)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_PIER_FENCE_SW);
            paintAddImageAsParent(session, imageId, { 31, 0, height + 2 }, { 1, 32, 7 });
        }
        TrackPaintUtilDrawStationCovers(session, edgeSW, hasFence, stationObj, height, colour);
    }
    else
    {
        hasFence = TrackPaintUtilHasFence(edgeNW, position, trackElement, ride, rotation);
        imageId = session.supportColours.WithIndex((hasFence ? SPR_STATION_PIER_EDGE_NW_FENCED : SPR_STATION_PIER_EDGE_NW));
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 6, 1 } });
        TrackPaintUtilDrawStationCovers(session, edgeNW, hasFence, stationObj, height, colour);

        imageId = session.supportColours.WithIndex(SPR_STATION_PIER_EDGE_SE);
        paintAddImageAsParent(session, imageId, { 0, 24, height }, { 32, 8, 1 });

        hasFence = TrackPaintUtilHasFence(edgeSE, position, trackElement, ride, rotation);
        if (hasFence)
        {
            imageId = session.supportColours.WithIndex(SPR_STATION_PIER_FENCE_SE);
            paintAddImageAsParent(session, imageId, { 0, 31, height + 2 }, { 32, 1, 7 });
        }
        TrackPaintUtilDrawStationCovers(session, edgeSE, hasFence, stationObj, height, colour);
    }
}

constexpr CoordsXY defaultRightHelixUpSmallQuarterBoundLengths[4][3][2] = {
    {
        { { 32, 20 }, { 0, 0 } },
        { { 16, 16 }, { 0, 0 } },
        { { 20, 32 }, { 0, 0 } },
    },
    {
        { { 20, 32 }, { 0, 0 } },
        { { 16, 16 }, { 0, 0 } },
        { { 32, 20 }, { 32, 1 } },
    },
    {
        { { 0, 0 }, { 32, 1 } },
        { { 16, 16 }, { 0, 0 } },
        { { 0, 0 }, { 1, 32 } },
    },
    {
        { { 20, 32 }, { 1, 32 } },
        { { 16, 16 }, { 0, 0 } },
        { { 32, 20 }, { 0, 0 } },
    },
};

constexpr CoordsXYZ defaultRightHelixUpSmallQuarterBoundOffsets[4][3][2] = {
    {
        { { 0, 6, 0 }, { 0, 0, 0 } },
        { { 16, 16, 0 }, { 0, 0, 0 } },
        { { 6, 0, 0 }, { 0, 0, 0 } },
    },
    {
        { { 6, 0, 0 }, { 0, 0, 0 } },
        { { 16, 0, 0 }, { 0, 0, 0 } },
        { { 0, 6, 0 }, { 0, 27, 0 } },
    },
    {
        { { 0, 0, 0 }, { 0, 27, 0 } },
        { { 0, 0, 27 }, { 0, 0, 0 } },
        { { 0, 0, 0 }, { 27, 0, 0 } },
    },
    {
        { { 6, 0, 0 }, { 27, 0, 0 } },
        { { 0, 16, 0 }, { 0, 0, 0 } },
        { { 0, 6, 0 }, { 0, 0, 0 } },
    },
};

static constexpr int8_t right_helix_up_small_quarter_tiles_sprite_map[] = {
    0,
    -1,
    1,
    2,
};

void TrackPaintUtilRightHelixUpSmallQuarterTilesPaint(
    PaintSession& session, const int8_t thickness[2], int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][3][2], const CoordsXY offsets[4][3][2],
    const CoordsXY boundsLengths[4][3][2], const CoordsXYZ boundsOffsets[4][3][2])
{
    int32_t index = right_helix_up_small_quarter_tiles_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    if (sprites[direction][index][0] != 0)
    {
        auto imageId = colourFlags.WithIndex(sprites[direction][index][0]);
        CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index][0]);
        CoordsXY boundsLength = boundsLengths[direction][index][0];
        CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index][0]);

        paintAddImageAsParent(
            session, imageId, { offset, height },
            { { boundsOffset.x, boundsOffset.y, height + boundsOffset.z }, { boundsLength, thickness[0] } });
    }
    if (sprites[direction][index][1] != 0)
    {
        auto imageId = colourFlags.WithIndex(sprites[direction][index][1]);
        CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index][1]);
        CoordsXY boundsLength = boundsLengths[direction][index][1];
        CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index][1]);

        paintAddImageAsParent(
            session, imageId, { offset, height },
            { { boundsOffset.x, boundsOffset.y, height + boundsOffset.z }, { boundsLength, thickness[1] } });
    }
}

constexpr CoordsXYZ defaultRightHelixUpLargeQuarterBoundOffsets[4][5][2] = {
    {
        { { 0, 6, 0 }, { 0, 0, 0 } },
        { { 0, 16, 0 }, { 0, 0, 0 } },
        { { 0, 0, 0 }, { 0, 0, 0 } },
        { { 16, 0, 0 }, { 0, 0, 0 } },
        { { 6, 0, 0 }, { 0, 0, 0 } },
    },
    {
        { { 6, 0, 0 }, { 0, 0, 0 } },
        { { 16, 0, 0 }, { 0, 0, 0 } },
        { { 0, 16, 0 }, { 0, 0, 0 } },
        { { 0, 0, 0 }, { 0, 0, 0 } },
        { { 0, 6, 0 }, { 0, 27, 0 } },
    },
    {
        { { 0, 0, 0 }, { 0, 27, 0 } },
        { { 0, 0, 27 }, { 0, 0, 0 } },
        { { 16, 16, 27 }, { 0, 0, 0 } },
        { { 0, 0, 27 }, { 0, 0, 0 } },
        { { 0, 0, 0 }, { 27, 0, 0 } },
    },
    {
        { { 6, 0, 0 }, { 27, 0, 0 } },
        { { 0, 0, 0 }, { 0, 0, 0 } },
        { { 16, 0, 0 }, { 0, 0, 0 } },
        { { 0, 16, 0 }, { 0, 0, 0 } },
        { { 0, 6, 0 }, { 0, 0, 0 } },
    },
};

constexpr CoordsXY defaultRightHelixUpLargeQuarterBoundLengths[4][5][2] = {
    {
        { { 32, 20 }, { 0, 0 } },
        { { 32, 16 }, { 0, 0 } },
        { { 16, 16 }, { 0, 0 } },
        { { 16, 32 }, { 0, 0 } },
        { { 20, 32 }, { 0, 0 } },
    },
    {
        { { 20, 32 }, { 0, 0 } },
        { { 16, 32 }, { 0, 0 } },
        { { 16, 16 }, { 0, 0 } },
        { { 32, 16 }, { 0, 0 } },
        { { 32, 20 }, { 32, 1 } },
    },
    {
        { { 0, 0 }, { 32, 1 } },
        { { 32, 16 }, { 0, 0 } },
        { { 16, 16 }, { 0, 0 } },
        { { 16, 32 }, { 0, 0 } },
        { { 0, 0 }, { 1, 32 } },
    },
    {
        { { 20, 32 }, { 1, 32 } },
        { { 16, 32 }, { 0, 0 } },
        { { 16, 16 }, { 0, 0 } },
        { { 32, 16 }, { 0, 0 } },
        { { 32, 20 }, { 0, 0 } },
    },
};

static constexpr int8_t right_helix_up_large_quarter_sprite_map[] = {
    0, -1, 1, 2, -1, 3, 4,
};
void TrackPaintUtilRightHelixUpLargeQuarterTilesPaint(
    PaintSession& session, const int8_t thickness[2], int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][5][2], const CoordsXY offsets[4][5][2],
    const CoordsXY boundsLengths[4][5][2], const CoordsXYZ boundsOffsets[4][5][2])
{
    int32_t index = right_helix_up_large_quarter_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    if (sprites[direction][index][0] != 0)
    {
        auto imageId = colourFlags.WithIndex(sprites[direction][index][0]);
        CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index][0]);
        CoordsXY boundsLength = boundsLengths[direction][index][0];
        CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index][0]);

        paintAddImageAsParent(
            session, imageId, { offset, height },
            { { boundsOffset.x, boundsOffset.y, height + boundsOffset.z }, { boundsLength, thickness[0] } });
    }
    if (sprites[direction][index][1] != 0)
    {
        auto imageId = colourFlags.WithIndex(sprites[direction][index][1]);
        CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index][1]);
        CoordsXY boundsLength = boundsLengths[direction][index][1];
        CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index][1]);

        paintAddImageAsParent(
            session, imageId, { offset, height },
            { { boundsOffset.x, boundsOffset.y, height + boundsOffset.z }, { boundsLength, thickness[1] } });
    }
}

constexpr CoordsXY defaultLeftEighthToDiagBoundLengths[4][4] = {
    {
        { 32, 20 },
        { 32, 16 },
        { 16, 16 },
        { 16, 16 },
    },
    {
        { 20, 32 },
        { 16, 34 },
        { 16, 16 },
        { 18, 16 },
    },
    {
        { 32, 20 },
        { 32, 16 },
        { 16, 16 },
        { 16, 16 },
    },
    {
        { 20, 32 },
        { 16, 32 },
        { 16, 16 },
        { 16, 16 },
    },
};

constexpr CoordsXYZ defaultLeftEighthToDiagBoundOffsets[4][4] = {
    {
        { 0, 6, 0 },
        { 0, 0, 0 },
        { 0, 16, 0 },
        { 16, 16, 0 },
    },
    {
        { 6, 0, 0 },
        { 0, 0, 0 },
        { 16, 16, 0 },
        { 16, 0, 0 },
    },
    {
        { 0, 6, 0 },
        { 0, 16, 0 },
        { 16, 0, 0 },
        { 0, 0, 0 },
    },
    {
        { 6, 0, 0 },
        { 16, 0, 0 },
        { 0, 0, 0 },
        { 0, 16, 0 },
    },
};

constexpr CoordsXY defaultRightEighthToDiagBoundLengths[4][4] = {
    {
        { 32, 20 },
        { 32, 16 },
        { 16, 16 },
        { 16, 16 },
    },
    {
        { 20, 32 },
        { 16, 32 },
        { 16, 16 },
        { 16, 16 },
    },
    {
        { 32, 20 },
        { 34, 16 },
        { 16, 16 },
        { 16, 18 },
    },
    {
        { 20, 32 },
        { 16, 32 },
        { 16, 16 },
        { 16, 16 },
    },
};

constexpr CoordsXYZ defaultRightEighthToDiagBoundOffsets[4][4] = {
    {
        { 0, 6, 0 },
        { 0, 16, 0 },
        { 0, 0, 0 },
        { 16, 0, 0 },
    },
    {
        { 6, 0, 0 },
        { 16, 0, 0 },
        { 0, 16, 0 },
        { 0, 0, 0 },
    },
    {
        { 0, 6, 0 },
        { 0, 0, 0 },
        { 16, 16, 0 },
        { 0, 16, 0 },
    },
    {
        { 6, 0, 0 },
        { 0, 0, 0 },
        { 16, 0, 0 },
        { 16, 16, 0 },
    },
};

const int8_t defaultEighthToDiagThickness[4][4] = {
    {
        1,
        1,
        1,
        1,
    },
    {
        1,
        1,
        1,
        1,
    },
    {
        1,
        1,
        1,
        1,
    },
    {
        1,
        1,
        1,
        1,
    },
};

const uint8_t mapLeftEighthTurnToOrthogonal[] = {
    4, 2, 3, 1, 0,
};

static constexpr int8_t eighth_to_diag_sprite_map[] = {
    0, 1, 2, -1, 3,
};
void TrackPaintUtilEighthToDiagTilesPaint(
    PaintSession& session, const int8_t thickness[4][4], int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][4], const CoordsXY offsets[4][4], const CoordsXY boundsLengths[4][4],
    const CoordsXYZ boundsOffsets[4][4])
{
    int32_t index = eighth_to_diag_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    auto imageId = colourFlags.WithIndex(sprites[direction][index]);
    CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index]);
    CoordsXY boundsLength = boundsLengths[direction][index];
    CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index]);

    paintAddImageAsParent(
        session, imageId, { offset.x, offset.y, height },
        { { boundsOffset.x, boundsOffset.y, height + boundsOffset.z },
          { boundsLength.x, boundsLength.y, thickness[direction][index] } });
}

constexpr CoordsXY defaultDiagTileOffsets[4] = {
    { -16, -16 },
    { -16, -16 },
    { -16, -16 },
    { -16, -16 },
};

constexpr CoordsXY defaultDiagBoundLengths[4] = {
    { 32, 32 },
    { 32, 32 },
    { 32, 32 },
    { 32, 32 },
};

void TrackPaintUtilDiagTilesPaint(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const uint32_t sprites[4], const CoordsXY offsets[4], const CoordsXY boundsLengths[4], const CoordsXYZ boundsOffsets[4],
    int8_t additionalBoundsHeight, const ImageId colourFlags)
{
    auto shouldDraw = kDiagSpriteMap[direction][trackSequence];
    if (!shouldDraw)
    {
        return;
    }

    auto imageId = colourFlags.WithIndex(sprites[direction]);
    CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction]);
    CoordsXY boundsLength = boundsLengths[direction];
    CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, additionalBoundsHeight) : boundsOffsets[direction]);

    paintAddImageAsParent(
        session, imageId, { offset, height },
        { { boundsOffset.x, boundsOffset.y, height + boundsOffset.z }, { boundsLength, thickness } });
}

void TrackPaintUtilDiagTilesPaintExtra(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const uint32_t sprites[4], MetalSupportType supportType)
{
    TrackPaintUtilDiagTilesPaint(
        session, thickness, height, direction, trackSequence, sprites, defaultDiagTileOffsets, defaultDiagBoundLengths,
        nullptr);

    if (SupportedSequences::kDiagStraightFlat[trackSequence] != MetalSupportPlace::none)
    {
        MetalASupportsPaintSetupRotated(
            session, supportType, SupportedSequences::kDiagStraightFlat[trackSequence], direction, 0, height,
            session.supportColours);
    }

    paintUtilSetSegmentSupportHeight(
        session, paintUtilRotateSegments(BlockedSegments::kDiagStraightFlat[trackSequence], direction), 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

const uint8_t kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[] = {
    6, 4, 5, 3, 1, 2, 0,
};

const uint8_t kMapReversedDiagonalStraight[4] = {
    3,
    2,
    1,
    0,
};

constexpr CoordsXY kDefaultRightQuarterTurn5TilesOffsets[4][5] = {
    {
        { 0, 6 },
        { 0, 16 },
        { 0, 0 },
        { 16, 0 },
        { 6, 0 },
    },
    {
        { 6, 0 },
        { 16, 0 },
        { 0, 16 },
        { 0, 0 },
        { 0, 6 },
    },
    {
        { 0, 6 },
        { 0, 0 },
        { 16, 16 },
        { 0, 0 },
        { 6, 0 },
    },
    {
        { 6, 0 },
        { 0, 0 },
        { 16, 0 },
        { 0, 16 },
        { 0, 6 },
    },
};

constexpr CoordsXYZ kDefaultRightQuarterTurn5TilesBoundOffsets[4][5] = {
    {
        { 0, 6, 0 },
        { 0, 16, 0 },
        { 0, 0, 0 },
        { 16, 0, 0 },
        { 6, 0, 0 },
    },
    {
        { 6, 0, 0 },
        { 16, 0, 0 },
        { 0, 16, 0 },
        { 0, 0, 0 },
        { 0, 6, 0 },
    },
    {
        { 0, 6, 0 },
        { 0, 0, 0 },
        { 16, 16, 0 },
        { 0, 0, 0 },
        { 6, 0, 0 },
    },
    {
        { 6, 0, 0 },
        { 0, 0, 0 },
        { 16, 0, 0 },
        { 0, 16, 0 },
        { 0, 6, 0 },
    },
};

constexpr CoordsXY kDefaultRightQuarterTurn5TilesBoundLengths[4][5] = {
    {
        { 32, 20 },
        { 32, 16 },
        { 16, 16 },
        { 16, 32 },
        { 20, 32 },
    },
    {
        { 20, 32 },
        { 16, 32 },
        { 16, 16 },
        { 32, 16 },
        { 32, 20 },
    },
    {
        { 32, 20 },
        { 32, 16 },
        { 16, 16 },
        { 16, 32 },
        { 20, 32 },
    },
    {
        { 20, 32 },
        { 16, 32 },
        { 16, 16 },
        { 32, 16 },
        { 32, 20 },
    },
};

static constexpr int8_t right_quarter_turn_5_tiles_sprite_map[] = {
    0, -1, 1, 2, -1, 3, 4,
};

void TrackPaintUtilRightQuarterTurn5TilesPaint(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][5], const CoordsXY offsets[4][5], const CoordsXY boundsLengths[4][5],
    const CoordsXYZ boundsOffsets[4][5])
{
    int32_t index = right_quarter_turn_5_tiles_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    auto imageId = colourFlags.WithIndex(sprites[direction][index]);
    CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index]);
    CoordsXY boundsLength = boundsLengths[direction][index];
    CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index]);

    paintAddImageAsParent(
        session, imageId, { static_cast<int8_t>(offset.x), static_cast<int8_t>(offset.y), height },
        { { boundsOffset.x, boundsOffset.y, height + boundsOffset.z }, { boundsLength.x, boundsLength.y, thickness } });
}

void TrackPaintUtilRightQuarterTurn5TilesPaint2(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, const ImageId colourFlags,
    const SpriteBb sprites[][5])
{
    int8_t sprite = right_quarter_turn_5_tiles_sprite_map[trackSequence];
    if (sprite < 0)
    {
        return;
    }

    const SpriteBb* spriteBB = &sprites[direction][sprite];
    const auto imageId = colourFlags.WithIndex(spriteBB->spriteId);
    const auto& offset = spriteBB->offset;
    const auto& bbOffset = spriteBB->bbOffset;
    paintAddImageAsParent(
        session, imageId, { offset.x, offset.y, height + offset.z },
        { { bbOffset.x, bbOffset.y, height + bbOffset.z }, spriteBB->bbSize });
}

void TrackPaintUtilRightQuarterTurn5TilesPaint3(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, const ImageId colourFlags,
    const SpriteBb sprites[][5])
{
    int8_t sprite = right_quarter_turn_5_tiles_sprite_map[trackSequence];
    if (sprite < 0)
    {
        return;
    }

    const SpriteBb* spriteBB = &sprites[direction][sprite];
    const auto imageId = colourFlags.WithIndex(spriteBB->spriteId);
    const auto& offset = spriteBB->offset;
    paintAddImageAsParent(session, imageId, { offset.x, offset.y, height + offset.z }, spriteBB->bbSize);
}

const uint8_t kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[] = {
    3,
    1,
    2,
    0,
};

constexpr CoordsXY defaultRightQuarterTurn3TilesOffsets[4][3] = {
    {
        { 0, 6 },
        { 16, 16 },
        { 6, 0 },
    },
    {
        { 6, 0 },
        { 16, 0 },
        { 0, 6 },
    },
    {
        { 0, 6 },
        { 0, 0 },
        { 6, 0 },
    },
    {
        { 6, 0 },
        { 0, 16 },
        { 0, 6 },
    },
};

constexpr CoordsXYZ defaultRightQuarterTurn3TilesBoundOffsets[4][3] = {
    {
        { 0, 6, 0 },
        { 16, 16, 0 },
        { 6, 0, 0 },
    },
    {
        { 6, 0, 0 },
        { 16, 0, 0 },
        { 0, 6, 0 },
    },
    {
        { 0, 6, 0 },
        { 0, 0, 0 },
        { 6, 0, 0 },
    },
    {
        { 6, 0, 0 },
        { 0, 16, 0 },
        { 0, 6, 0 },
    },
};

constexpr CoordsXY defaultRightQuarterTurn3TilesBoundLengths[4][3] = {
    {
        { 32, 20 },
        { 16, 16 },
        { 20, 32 },
    },
    {
        { 20, 32 },
        { 16, 16 },
        { 32, 20 },
    },
    {
        { 32, 20 },
        { 16, 16 },
        { 20, 32 },
    },
    {
        { 20, 32 },
        { 16, 16 },
        { 32, 20 },
    },
};

void TrackPaintUtilRightQuarterTurn3TilesPaint(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][3], const CoordsXY offsets[4][3], const CoordsXY boundsLengths[4][3],
    const CoordsXYZ boundsOffsets[4][3])
{
    int32_t index = kRightQuarterTurn3TilesSpriteMap[trackSequence];
    if (index < 0)
    {
        return;
    }

    auto imageId = colourFlags.WithIndex(sprites[direction][index]);
    CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index]);
    CoordsXY boundsLength = boundsLengths[direction][index];
    CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index]);

    paintAddImageAsParent(
        session, imageId, { offset, height },
        { { boundsOffset.x, boundsOffset.y, height + boundsOffset.z }, { boundsLength.x, boundsLength.y, thickness } });
}

void TrackPaintUtilRightQuarterTurn3TilesPaint2(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][3])
{
    TrackPaintUtilRightQuarterTurn3TilesPaint2WithHeightOffset(
        session, thickness, height, direction, trackSequence, colourFlags, sprites, 0);
}

void TrackPaintUtilRightQuarterTurn3TilesPaint2WithHeightOffset(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][3], int32_t heightOffset)
{
    int8_t sprite = kRightQuarterTurn3TilesSpriteMap[trackSequence];
    if (sprite < 0)
    {
        return;
    }

    auto imageId = colourFlags.WithIndex(sprites[(direction + 1) % 4][sprite]);

    switch (direction)
    {
        case 0:
            switch (trackSequence)
            {
                case 0:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 0, 6, height + heightOffset }, { 32, 20, thickness } });
                    break;
                case 2:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 16, 16, height + heightOffset }, { 16, 16, thickness } });
                    break;
                case 3:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 6, 0, height + heightOffset }, { 20, 32, thickness } });
                    break;
            }
            break;

        case 1:
            switch (trackSequence)
            {
                case 0:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 6, 0, height + heightOffset }, { 20, 32, thickness } });
                    break;
                case 2:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 16, 0, height + heightOffset }, { 16, 16, thickness } });
                    break;
                case 3:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 0, 6, height + heightOffset }, { 32, 20, thickness } });
                    break;
            }
            break;

        case 2:
            switch (trackSequence)
            {
                case 0:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 0, 6, height + heightOffset }, { 32, 20, thickness } });
                    break;
                case 2:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 0, 0, height + heightOffset }, { 16, 16, thickness } });
                    break;
                case 3:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 6, 0, height + heightOffset }, { 20, 32, thickness } });
                    break;
            }
            break;

        case 3:
            switch (trackSequence)
            {
                case 0:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 6, 0, height + heightOffset }, { 20, 32, thickness } });
                    break;
                case 2:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 0, 16, height + heightOffset }, { 16, 16, thickness } });
                    break;
                case 3:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 0, 6, height + heightOffset }, { 32, 20, thickness } });
                    break;
            }
            break;
    }
}

void TrackPaintUtilRightQuarterTurn3TilesPaint3(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, const ImageId colourFlags,
    const SpriteBb sprites[4][3])
{
    int8_t sprite = kRightQuarterTurn3TilesSpriteMap[trackSequence];
    if (sprite < 0)
    {
        return;
    }
    const SpriteBb* spriteBB = &sprites[direction][sprite];
    const auto& offset = spriteBB->offset;
    const auto& bbOffset = spriteBB->bbOffset;
    paintAddImageAsParent(
        session, colourFlags.WithIndex(spriteBB->spriteId), { offset.x, offset.y, offset.z + height },
        { { bbOffset.x, bbOffset.y, height + bbOffset.z }, spriteBB->bbSize });
}

void TrackPaintUtilRightQuarterTurn3TilesPaint4(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, const ImageId colourFlags,
    const SpriteBb sprites[4][3])
{
    int8_t sprite = kRightQuarterTurn3TilesSpriteMap[trackSequence];
    if (sprite < 0)
    {
        return;
    }

    const SpriteBb* spriteBB = &sprites[direction][sprite];
    const auto imageId = colourFlags.WithIndex(spriteBB->spriteId);
    const auto& offset = spriteBB->offset;
    paintAddImageAsParent(session, imageId, { offset.x, offset.y, height + offset.z }, spriteBB->bbSize);
}

static constexpr int8_t left_quarter_turn_3_tiles_sprite_map[] = {
    2,
    -1,
    1,
    0,
};

void TrackPaintUtilLeftQuarterTurn3TilesPaint(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][3])
{
    TrackPaintUtilLeftQuarterTurn3TilesPaintWithHeightOffset(
        session, thickness, height, direction, trackSequence, colourFlags, sprites, 0);
}

void TrackPaintUtilLeftQuarterTurn3TilesPaintWithHeightOffset(
    PaintSession& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence,
    const ImageId colourFlags, const uint32_t sprites[4][3], int32_t heightOffset)
{
    int8_t sprite = left_quarter_turn_3_tiles_sprite_map[trackSequence];
    if (sprite < 0)
    {
        return;
    }

    auto imageId = colourFlags.WithIndex(sprites[(direction + 1) % 4][sprite]);

    switch (direction)
    {
        case 0:
            switch (trackSequence)
            {
                case 0:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 0, 6, height + heightOffset }, { 32, 20, thickness } });
                    break;
                case 2:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 16, 0, height + heightOffset }, { 16, 16, thickness } });
                    break;
                case 3:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 6, 0, height + heightOffset }, { 20, 32, thickness } });
                    break;
            }
            break;

        case 1:
            switch (trackSequence)
            {
                case 0:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 6, 0, height + heightOffset }, { 20, 32, thickness } });
                    break;
                case 2:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 0, 0, height + heightOffset }, { 16, 16, thickness } });
                    break;
                case 3:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 0, 6, height + heightOffset }, { 32, 20, thickness } });
                    break;
            }
            break;

        case 2:
            switch (trackSequence)
            {
                case 0:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 0, 6, height + heightOffset }, { 32, 20, thickness } });
                    break;
                case 2:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 0, 16, height + heightOffset }, { 16, 16, thickness } });
                    break;
                case 3:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 6, 0, height + heightOffset }, { 20, 32, thickness } });
                    break;
            }
            break;

        case 3:
            switch (trackSequence)
            {
                case 0:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 6, 0, height + heightOffset }, { 20, 32, thickness } });
                    break;
                case 2:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 16, 16, height + heightOffset }, { 16, 16, thickness } });
                    break;
                case 3:
                    paintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { { 0, 6, height + heightOffset }, { 32, 20, thickness } });
                    break;
            }
            break;
    }
}

void TrackPaintUtilLeftQuarterTurn1TilePaint(
    PaintSession& session, int8_t thickness, int16_t height, int16_t boundBoxZOffset, Direction direction,
    const ImageId colourFlags, const uint32_t* sprites)
{
    auto imageId = colourFlags.WithIndex(sprites[direction]);

    switch (direction)
    {
        case 0:
            paintAddImageAsParent(
                session, imageId, { 0, 0, height }, { { 6, 2, height + boundBoxZOffset }, { 26, 24, thickness } });
            break;
        case 1:
            paintAddImageAsParent(
                session, imageId, { 0, 0, height }, { { 0, 0, height + boundBoxZOffset }, { 26, 26, thickness } });
            break;
        case 2:
            paintAddImageAsParent(
                session, imageId, { 0, 0, height }, { { 2, 6, height + boundBoxZOffset }, { 24, 26, thickness } });
            break;
        case 3:
            paintAddImageAsParent(
                session, imageId, { 0, 0, height }, { { 6, 6, height + boundBoxZOffset }, { 24, 24, thickness } });
            break;
    }
}

void TrackPaintUtilSpinningTunnelPaint(PaintSession& session, int8_t thickness, int16_t height, Direction direction)
{
    int32_t frame = (getGameState().currentTicks >> 2) & 3;
    auto colourFlags = session.supportColours;

    auto colourFlags2 = session.trackColours;
    if (colourFlags2.HasSecondary())
    {
        colourFlags = colourFlags.WithSecondary(colourFlags2.GetSecondary());
    }

    auto imageId = colourFlags.WithIndex(trackSpritesGhostTrainSpinningTunnel[direction & 1][0][frame]);
    if (direction == 0 || direction == 2)
    {
        paintAddImageAsChild(session, imageId, { 0, 0, height }, { { 2, 6, height }, { 28, 20, thickness } });
    }
    else
    {
        paintAddImageAsChild(session, imageId, { 0, 0, height }, { { 6, 2, height }, { 20, 28, thickness } });
    }

    imageId = colourFlags.WithIndex(trackSpritesGhostTrainSpinningTunnel[direction & 1][1][frame]);
    if (direction == 0 || direction == 2)
    {
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 28, height }, { 28, 1, 23 } });
    }
    else
    {
        paintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 2, height }, { 1, 28, 23 } });
    }
}

void TrackPaintUtilOnridePhotoPlatformPaintBase(PaintSession& session, int32_t height)
{
    auto image = ImageId(SPR_STATION_BASE_BORDERLESS, OpenRCT2::Drawing::Colour::black);
    if (session.trackColours == ConstructionMarker)
        image = session.trackColours.WithIndex(SPR_STATION_BASE_BORDERLESS);
    paintAddImageAsParent(session, image, { 0, 0, height }, { 32, 32, 1 });
}

void TrackPaintUtilOnridePhotoPlatformPaint(
    PaintSession& session, Direction direction, int32_t height, MetalSupportType supportType)
{
    TrackPaintUtilOnridePhotoPlatformPaintBase(session, height);
    DrawSupportsSideBySide(session, direction, height, session.supportColours, supportType);
}

void TrackPaintUtilOnridePhotoSmallPaint(
    PaintSession& session, Direction direction, int32_t height, const TrackElement& trackElement)
{
    static constexpr uint32_t imageIds[4][3] = {
        { SPR_ON_RIDE_PHOTO_SIGN_SMALL_SW_NE, SPR_ON_RIDE_PHOTO_CAMERA_SMALL_S, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_S },
        { SPR_ON_RIDE_PHOTO_SIGN_SMALL_NW_SE, SPR_ON_RIDE_PHOTO_CAMERA_SMALL_W, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_W },
        { SPR_ON_RIDE_PHOTO_SIGN_SMALL_NE_SW, SPR_ON_RIDE_PHOTO_CAMERA_SMALL_N, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_N },
        { SPR_ON_RIDE_PHOTO_SIGN_SMALL_SE_NW, SPR_ON_RIDE_PHOTO_CAMERA_SMALL_E, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_E },
    };

    bool takingPhoto = trackElement.IsTakingPhoto();
    ImageId imageId = GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction][0]);
    ImageId flashImageId = GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction][takingPhoto ? 2 : 1]);
    switch (direction)
    {
        case 0:
            paintAddImageAsParent(session, imageId, { 26, 0, height }, { 1, 1, 19 });
            paintAddImageAsParent(session, imageId, { 26, 28, height - 3 }, { 1, 1, 19 });
            paintAddImageAsParent(session, flashImageId, { 6, 0, height }, { 1, 1, 19 });
            break;
        case 1:
            paintAddImageAsParent(session, imageId, { 0, 6, height }, { 1, 1, 19 });
            paintAddImageAsParent(session, imageId, { 28, 6, height - 3 }, { 1, 1, 19 });
            paintAddImageAsParent(session, flashImageId, { 0, 26, height }, { 1, 1, 19 });
            break;
        case 2:
            paintAddImageAsParent(session, imageId, { 6, 0, height }, { 1, 1, 19 });
            paintAddImageAsParent(session, imageId, { 6, 28, height - 3 }, { 1, 1, 19 });
            paintAddImageAsParent(session, flashImageId, { 26, 28, height - 3 }, { 1, 1, 19 });
            break;
        case 3:
            paintAddImageAsParent(session, imageId, { 0, 26, height }, { 1, 1, 19 });
            paintAddImageAsParent(session, imageId, { 28, 26, height - 3 }, { 1, 1, 19 });
            paintAddImageAsParent(session, flashImageId, { 28, 6, height - 3 }, { 1, 1, 19 });
            break;
    }
}

void TrackPaintUtilOnridePhotoPaint(
    PaintSession& session, Direction direction, int32_t height, const TrackElement& trackElement)
{
    static constexpr uint32_t imageIds[4][3] = {
        { SPR_ON_RIDE_PHOTO_SIGN_SW_NE, SPR_ON_RIDE_PHOTO_CAMERA_S, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_S },
        { SPR_ON_RIDE_PHOTO_SIGN_NW_SE, SPR_ON_RIDE_PHOTO_CAMERA_W, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_W },
        { SPR_ON_RIDE_PHOTO_SIGN_NE_SW, SPR_ON_RIDE_PHOTO_CAMERA_N, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_N },
        { SPR_ON_RIDE_PHOTO_SIGN_SE_NW, SPR_ON_RIDE_PHOTO_CAMERA_E, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_E },
    };

    bool takingPhoto = trackElement.IsTakingPhoto();
    ImageId imageId = GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction][0]);
    ImageId flashImageId = GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction][takingPhoto ? 2 : 1]);
    switch (direction)
    {
        case 0:
            paintAddImageAsParent(session, imageId, { 26, 0, height }, { 1, 1, 19 });
            paintAddImageAsParent(session, imageId, { 26, 28, height - 3 }, { 1, 1, 19 });
            paintAddImageAsParent(session, flashImageId, { 6, 0, height }, { 1, 1, 19 });
            break;
        case 1:
            paintAddImageAsParent(session, imageId, { 0, 6, height }, { 1, 1, 19 });
            paintAddImageAsParent(session, imageId, { 28, 6, height - 3 }, { 1, 1, 19 });
            paintAddImageAsParent(session, flashImageId, { 0, 26, height }, { 1, 1, 19 });
            break;
        case 2:
            paintAddImageAsParent(session, imageId, { 6, 0, height }, { 1, 1, 19 });
            paintAddImageAsParent(session, imageId, { 6, 28, height - 3 }, { 1, 1, 19 });
            paintAddImageAsParent(session, flashImageId, { 26, 28, height - 3 }, { 1, 1, 19 });
            break;
        case 3:
            paintAddImageAsParent(session, imageId, { 0, 26, height }, { 1, 1, 19 });
            paintAddImageAsParent(session, imageId, { 28, 26, height - 3 }, { 1, 1, 19 });
            paintAddImageAsParent(session, flashImageId, { 28, 6, height - 3 }, { 1, 1, 19 });
            break;
    }
}

static constexpr uint16_t RightVerticalLoopSegments[] = {
    EnumsToFlags(
        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
        PaintSegment::bottomRight),
    EnumsToFlags(
        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
        PaintSegment::bottomRight),
    EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
    EnumsToFlags(
        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft,
        PaintSegment::bottomRight),
    0,
    0,
    EnumsToFlags(
        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
        PaintSegment::bottomLeft),
    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
    EnumsToFlags(
        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
        PaintSegment::bottomLeft),
    EnumsToFlags(
        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
        PaintSegment::bottomLeft),
};

void TrackPaintUtilRightVerticalLoopSegments(PaintSession& session, Direction direction, uint8_t trackSequence)
{
    if (trackSequence > 9)
    {
        // P
        return;
    }

    paintUtilSetSegmentSupportHeight(
        session, paintUtilRotateSegments(RightVerticalLoopSegments[trackSequence], direction), 0xFFFF, 0);
}

void TrackPaintUtilLeftCorkscrewUpSupports(PaintSession& session, Direction direction, uint16_t height)
{
    MetalASupportsPaintSetupRotated(
        session, MetalSupportType::tubes, MetalSupportPlace::centre, direction, 0, height, session.supportColours);
    paintUtilSetSegmentSupportHeight(
        session,
        paintUtilRotateSegments(
            EnumsToFlags(
                PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                PaintSegment::bottomLeft),
            direction),
        0xFFFF, 0);
}

ImageId GetStationColourScheme(PaintSession& session, const TrackElement& trackElement)
{
    if (trackElement.isGhost())
    {
        return ConstructionMarker;
    }
    if (trackElement.IsHighlighted() || session.selectedElement == reinterpret_cast<const TileElement*>(&trackElement))
    {
        return HighlightMarker;
    }
    return TrackStationColour;
}

ImageId GetShopSupportColourScheme(PaintSession& session, const TrackElement& trackElement)
{
    if (trackElement.isGhost())
    {
        return ConstructionMarker;
    }
    if (trackElement.IsHighlighted() || session.selectedElement == reinterpret_cast<const TileElement*>(&trackElement))
    {
        return HighlightMarker;
    }
    return ShopSupportColour;
}

/**
 *
 *  rct2: 0x006C4794
 */
void PaintTrack(PaintSession& session, Direction direction, int32_t height, const TrackElement& trackElement)
{
    RideId rideIndex = trackElement.GetRideIndex();
    auto ride = GetRide(rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Attempted to paint invalid ride: %d", rideIndex);
        return;
    }

    if ((!gTrackDesignSaveMode || rideIndex == gTrackDesignSaveRideIndex)
        && !(session.viewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
    {
        auto trackType = trackElement.GetTrackType();
        int32_t trackSequence = trackElement.GetSequenceIndex();
        int32_t trackColourScheme = trackElement.GetColourScheme();

        if (paintShouldShowHeightMarkers(session, VIEWPORT_FLAG_TRACK_HEIGHTS))
        {
            session.interactionType = ViewportInteractionItem::none;
            const auto& ted = GetTrackElementDescriptor(trackType);
            if ((trackType == TrackElemType::maze)
                || (trackSequence < ted.sequenceData.numSequences
                    && ted.sequenceData.sequences[trackSequence].flags.has(SequenceFlag::hasHeightMarker)))
            {
                uint16_t ax = ride->getRideTypeDescriptor().Heights.VehicleZOffset;
                // 0x1689 represents 0 height there are -127 to 128 heights above and below it
                // There are 3 arrays of 256 heights (units, m, ft) chosen with the GetHeightMarkerOffset()
                auto heightNum = (height + 8) / 16 - kMapBaseZ;
                auto imageId = ImageId(
                    SPR_HEIGHT_MARKER_BASE + GetHeightMarkerOffset() + heightNum, OpenRCT2::Drawing::Colour::lightBlue);

                paintAddImageAsParent(session, imageId, { 16, 16, height + ax + 3 }, { { 1000, 1000, 2047 }, { 1, 1, 0 } });
            }
        }

        if (LightFx::IsAvailable())
        {
            uint8_t zOffset = 16;
            const auto& rtd = ride->getRideTypeDescriptor();
            if (rtd.specialType == RtdSpecialType::toilet || rtd.specialType == RtdSpecialType::firstAid
                || rtd.specialType == RtdSpecialType::cashMachine)
                zOffset = 23;

            const auto* originElement = ride->getOriginElement(StationIndex::FromUnderlying(0));
            if (originElement != nullptr && originElement->GetTrackType() == TrackElemType::flatTrack1x1B)
                LightFx::AddKioskLights(session.mapPosition, height, zOffset);
            else if (kRideTypeDescriptors[ride->type].flags.has(RtdFlag::isShopOrFacility))
                LightFx::AddShopLights(session.mapPosition, trackElement.getDirection(), height, zOffset);
        }

        session.interactionType = ViewportInteractionItem::ride;
        session.trackColours = ImageId(
            0, ride->trackColours[trackColourScheme].main, ride->trackColours[trackColourScheme].additional);
        session.supportColours = ImageId(
            0, ride->trackColours[trackColourScheme].supports, ride->trackColours[trackColourScheme].additional);
        if (trackElement.IsHighlighted() || session.selectedElement == reinterpret_cast<const TileElement*>(&trackElement))
        {
            session.trackColours = HighlightMarker;
            session.supportColours = HighlightMarker;
        }
        if (trackElement.isGhost())
        {
            session.interactionType = ViewportInteractionItem::none;
            session.trackColours = ConstructionMarker;
            session.supportColours = ConstructionMarker;
        }

        const auto& rtd = GetRideTypeDescriptor(trackElement.GetRideType());
        bool isInverted = trackElement.IsInverted() && rtd.flags.has(RtdFlag::hasInvertedVariant);
        const auto trackDrawerEntry = getTrackDrawerEntry(rtd, isInverted, trackTypeIsCovered(trackType));

        trackType = uncoverTrackType(trackType);
        TrackPaintFunction paintFunction = GetTrackPaintFunction(trackDrawerEntry.trackStyle, trackType);
        paintFunction(session, *ride, trackSequence, direction, height, trackElement, trackDrawerEntry.supportType);
    }
}

void TrackPaintUtilOnridePhotoPaint2(
    PaintSession& session, Direction direction, const TrackElement& trackElement, int32_t height,
    int32_t supportsAboveHeightOffset, int32_t trackHeightOffset)
{
    TrackPaintUtilOnridePhotoPaint(session, direction, height + trackHeightOffset, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TunnelGroup::Square, TunnelSubType::Flat);
    paintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    paintUtilSetGeneralSupportHeight(session, height + supportsAboveHeightOffset);
}

void DrawSBendLeftSupports(
    PaintSession& session, MetalSupportType supportType, uint8_t sequence, Direction direction, int32_t height,
    int32_t specialA, int32_t specialB)
{
    switch (sequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType, MetalSupportPlace::centre, direction, specialA, height, session.supportColours);
            break;
        case 1:
            if (direction == 0)
                MetalASupportsPaintSetupRotated(
                    session, supportType, MetalSupportPlace::topLeftSide, direction, specialA, height, session.supportColours);
            if (direction == 1)
                MetalASupportsPaintSetupRotated(
                    session, supportType, MetalSupportPlace::topLeftSide, direction, specialB, height, session.supportColours);
            break;
        case 2:
            if (direction == 2)
                MetalASupportsPaintSetupRotated(
                    session, supportType, MetalSupportPlace::bottomRightSide, direction, specialA, height,
                    session.supportColours);
            if (direction == 3)
                MetalASupportsPaintSetupRotated(
                    session, supportType, MetalSupportPlace::bottomRightSide, direction, specialB, height,
                    session.supportColours);
            break;
        case 3:
            MetalASupportsPaintSetupRotated(
                session, supportType, MetalSupportPlace::centre, direction, specialA, height, session.supportColours);
            break;
    }
}

void DrawSBendRightSupports(
    PaintSession& session, MetalSupportType supportType, uint8_t sequence, Direction direction, int32_t height,
    int32_t specialA, int32_t specialB)
{
    switch (sequence)
    {
        case 0:
            MetalASupportsPaintSetupRotated(
                session, supportType, MetalSupportPlace::centre, direction, specialA, height, session.supportColours);
            break;
        case 1:
            if (direction == 0)
                MetalASupportsPaintSetupRotated(
                    session, supportType, MetalSupportPlace::bottomRightSide, direction, specialA, height,
                    session.supportColours);
            if (direction == 1)
                MetalASupportsPaintSetupRotated(
                    session, supportType, MetalSupportPlace::bottomRightSide, direction, specialB, height,
                    session.supportColours);
            break;
        case 2:
            if (direction == 2)
                MetalASupportsPaintSetupRotated(
                    session, supportType, MetalSupportPlace::topLeftSide, direction, specialA, height, session.supportColours);
            if (direction == 3)
                MetalASupportsPaintSetupRotated(
                    session, supportType, MetalSupportPlace::topLeftSide, direction, specialB, height, session.supportColours);
            break;
        case 3:
            MetalASupportsPaintSetupRotated(
                session, supportType, MetalSupportPlace::centre, direction, specialA, height, session.supportColours);
            break;
    }
}

void TrackPaintFunctionDummy(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
}
