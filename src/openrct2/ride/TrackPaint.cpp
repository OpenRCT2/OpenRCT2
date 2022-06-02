/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackPaint.h"

#include "../Game.h"
#include "../config/Config.h"
#include "../drawing/Drawing.h"
#include "../drawing/LightFX.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../object/StationObject.h"
#include "../paint/Paint.h"
#include "../paint/Supports.h"
#include "../paint/tile_element/Paint.TileElement.h"
#include "../scenario/Scenario.h"
#include "../sprites.h"
#include "../world/Map.h"
#include "RideData.h"
#include "Station.h"
#include "TrackData.h"
#include "TrackDesign.h"

using namespace OpenRCT2::TrackMetaData;

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

const uint8_t track_map_2x2[][4] = {
    { 0, 1, 2, 3 },
    { 1, 3, 0, 2 },
    { 3, 2, 1, 0 },
    { 2, 0, 3, 1 },
};

const uint8_t edges_2x2[] = {
    EDGE_NE | EDGE_NW,
    EDGE_NE | EDGE_SE,
    EDGE_SW | EDGE_NW,
    EDGE_SW | EDGE_SE,
};

const uint8_t track_map_3x3[][9] = {
    { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
    { 0, 3, 5, 7, 2, 8, 1, 6, 4 },
    { 0, 7, 8, 6, 5, 4, 3, 1, 2 },
    { 0, 6, 4, 1, 8, 2, 7, 3, 5 },
};

// clang-format off
const uint8_t edges_3x3[] = {
    0,
    EDGE_NE | EDGE_NW,
    EDGE_NE,
    EDGE_NE | EDGE_SE,
    EDGE_NW,
    EDGE_SE,
    EDGE_SW | EDGE_NW,
    EDGE_SW | EDGE_SE,
    EDGE_SW,
};
// clang-format on

const uint8_t track_map_4x4[][16] = {
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
    { 3, 7, 11, 15, 2, 6, 10, 14, 1, 5, 9, 13, 0, 4, 8, 12 },
    { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 },
    { 12, 8, 4, 0, 13, 9, 5, 1, 14, 10, 6, 2, 15, 11, 7, 3 },
};

// clang-format off
const uint8_t edges_4x4[] = {
    EDGE_NE | EDGE_NW,
    EDGE_NE,
    EDGE_NE,
    EDGE_NE | EDGE_SE,
    EDGE_NW,
    0,
    0,
    EDGE_SE,
    EDGE_NW,
    0,
    0,
    EDGE_SE,
    EDGE_NW | EDGE_SW,
    EDGE_SW,
    EDGE_SW,
    EDGE_SW | EDGE_SE,
};
// clang-format on

const uint8_t track_map_1x4[][4] = {
    { 0, 1, 2, 3 },
    { 2, 3, 0, 1 },
    { 2, 3, 0, 1 },
    { 0, 1, 2, 3 },
};

const uint32_t floorSpritesCork[] = {
    SPR_FLOOR_CORK_SE_SW,
    SPR_FLOOR_CORK_SW,
    SPR_FLOOR_CORK_SE,
    SPR_FLOOR_CORK,
};

const uint32_t floorSpritesMetal[] = {
    SPR_FLOOR_METAL,
    SPR_FLOOR_METAL,
    SPR_FLOOR_METAL,
    SPR_FLOOR_METAL,
};

const uint32_t floorSpritesMetalB[] = {
    SPR_FLOOR_METAL_B,
    SPR_FLOOR_METAL_B,
    SPR_FLOOR_METAL_B,
    SPR_FLOOR_METAL_B,
};

const uint32_t fenceSpritesRope[] = {
    SPR_FENCE_ROPE_NE,
    SPR_FENCE_ROPE_SE,
    SPR_FENCE_ROPE_SW,
    SPR_FENCE_ROPE_NW,
};

const uint32_t fenceSpritesMetal[] = {
    SPR_FENCE_METAL_NE,
    SPR_FENCE_METAL_SE,
    SPR_FENCE_METAL_SW,
    SPR_FENCE_METAL_NW,
};

const uint32_t fenceSpritesMetalB[] = {
    SPR_FENCE_METAL_B_NE,
    SPR_FENCE_METAL_B_SE,
    SPR_FENCE_METAL_B_SW,
    SPR_FENCE_METAL_B_NW,
};

const uint32_t trackSpritesSubmarineRideMiniHelicoptersQuarterTurn3Tiles[4][3] = {
    {
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_2,
    },
    {
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_2,
    },
    {
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_2,
    },
    {
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_2,
    }
};

const uint32_t trackSpritesSubmarineRideMiniHelicoptersQuarterTurn1Tile[4] = {
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_1_TILE_SW_NW,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_1_TILE_NW_NE,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_1_TILE_NE_SE,
    SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_1_TILE_SE_SW,
};

static constexpr const uint32_t trackSpritesGhostTrainSpinningTunnel[2][2][4] = {
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

enum
{
    SPR_STATION_COVER_OFFSET_NE_SW_BACK_0 = 0,
    SPR_STATION_COVER_OFFSET_NE_SW_BACK_1,
    SPR_STATION_COVER_OFFSET_NE_SW_FRONT,
    SPR_STATION_COVER_OFFSET_SE_NW_BACK_0,
    SPR_STATION_COVER_OFFSET_SE_NW_BACK_1,
    SPR_STATION_COVER_OFFSET_SE_NW_FRONT,
    SPR_STATION_COVER_OFFSET_TALL
};

bool track_paint_util_has_fence(
    enum edge_t edge, const CoordsXY& position, const TrackElement& trackElement, const Ride& ride, uint8_t rotation)
{
    const auto* stationObject = ride.GetStationObject();
    if (stationObject != nullptr && stationObject->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS)
        return false;

    TileCoordsXY offset;
    switch (edge)
    {
        case EDGE_NE:
            offset = EntranceOffsetEdgeNE[rotation];
            break;
        case EDGE_SE:
            offset = EntranceOffsetEdgeNW[(rotation + 2) & 3];
            break;
        case EDGE_SW:
            offset = EntranceOffsetEdgeNE[(rotation + 2) & 3];
            break;
        case EDGE_NW:
            offset = EntranceOffsetEdgeNW[rotation];
            break;
    }

    auto entranceLoc = TileCoordsXY(position) + offset;
    auto entranceId = trackElement.GetStationIndex();
    const auto& station = ride.GetStation(entranceId);

    return (entranceLoc != station.Entrance && entranceLoc != station.Exit);
}

void track_paint_util_paint_floor(
    paint_session& session, uint8_t edges, uint32_t colourFlags, uint16_t height, const uint32_t floorSprites[4],
    const StationObject* stationStyle)
{
    if (stationStyle != nullptr && stationStyle->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS)
        return;

    uint32_t imageId;

    if (edges & EDGE_SW && edges & EDGE_SE)
    {
        imageId = floorSprites[0];
    }
    else if (edges & EDGE_SW)
    {
        imageId = floorSprites[1];
    }
    else if (edges & EDGE_SE)
    {
        imageId = floorSprites[2];
    }
    else
    {
        imageId = floorSprites[3];
    }

    PaintAddImageAsParent(session, imageId | colourFlags, { 0, 0, height }, { 32, 32, 1 }, { 0, 0, height });
}

void track_paint_util_paint_fences(
    paint_session& session, uint8_t edges, const CoordsXY& position, const TrackElement& trackElement, const Ride& ride,
    uint32_t colourFlags, uint16_t height, const uint32_t fenceSprites[4], uint8_t rotation)
{
    uint32_t imageId;

    if (edges & EDGE_NW && track_paint_util_has_fence(EDGE_NW, position, trackElement, ride, rotation))
    {
        imageId = fenceSprites[3] | colourFlags;
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 32, 1, 7 }, { 0, 2, height + 2 });
    }
    if (edges & EDGE_NE && track_paint_util_has_fence(EDGE_NE, position, trackElement, ride, rotation))
    {
        imageId = fenceSprites[0] | colourFlags;
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 1, 32, 7 }, { 2, 0, height + 2 });
    }
    if (edges & EDGE_SE && track_paint_util_has_fence(EDGE_SE, position, trackElement, ride, rotation))
    {
        imageId = fenceSprites[1] | colourFlags;
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 1, 7 }, { 0, 30, height + 2 });
    }
    if (edges & EDGE_SW && track_paint_util_has_fence(EDGE_SW, position, trackElement, ride, rotation))
    {
        imageId = fenceSprites[2] | colourFlags;
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 32, 7 }, { 30, 0, height + 2 });
    }
}

/* Supports are only placed every 2 tiles for flat pieces*/
bool track_paint_util_should_paint_supports(const CoordsXY& position)
{
    if ((position.x & (1 << 5)) == (position.y & (1 << 5)))
        return true;

    if ((!(position.x & (1 << 5))) && (!(position.y & (1 << 5))))
        return true;

    return false;
}

static void track_paint_util_draw_station_impl(
    paint_session& session, const Ride& ride, Direction direction, uint16_t height, uint16_t coverHeight,
    const TrackElement& trackElement, int32_t fenceOffsetA, int32_t fenceOffsetB);

void track_paint_util_draw_station(
    paint_session& session, const Ride& ride, Direction direction, uint16_t height, const TrackElement& trackElement)
{
    track_paint_util_draw_station_impl(session, ride, direction, height, height, trackElement, 5, 7);
}

void track_paint_util_draw_station_2(
    paint_session& session, const Ride& ride, Direction direction, uint16_t height, const TrackElement& trackElement,
    int32_t fenceOffsetA, int32_t fenceOffsetB)
{
    track_paint_util_draw_station_impl(session, ride, direction, height, height, trackElement, fenceOffsetA, fenceOffsetB);
}

void track_paint_util_draw_station_3(
    paint_session& session, const Ride& ride, Direction direction, uint16_t height, uint16_t coverHeight,
    const TrackElement& trackElement)
{
    track_paint_util_draw_station_impl(session, ride, direction, height, coverHeight, trackElement, 5, 7);
}

static void track_paint_util_draw_station_impl(
    paint_session& session, const Ride& ride, Direction direction, uint16_t height, uint16_t coverHeight,
    const TrackElement& trackElement, int32_t fenceOffsetA, int32_t fenceOffsetB)
{
    CoordsXY position = session.MapPosition;
    const auto* stationObj = ride.GetStationObject();
    const bool hasGreenLight = trackElement.HasGreenLight();

    if (stationObj != nullptr && stationObj->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS)
        return;

    bool hasFence;
    uint32_t imageId;

    if (direction == 0 || direction == 2)
    {
        // height += 5 (height + 5);
        hasFence = track_paint_util_has_fence(EDGE_NW, position, trackElement, ride, session.CurrentRotation);

        if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 0)
        {
            if (hasGreenLight)
            {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_SW_NE
                                    : SPR_STATION_PLATFORM_END_GREEN_LIGHT_SW_NE)
                    | session.TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_SW_NE
                                    : SPR_STATION_PLATFORM_END_RED_LIGHT_SW_NE)
                    | session.TrackColours[SCHEME_SUPPORTS];
            }
        }
        else if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 2)
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_SW_NE : SPR_STATION_PLATFORM_BEGIN_SW_NE)
                | session.TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_SW_NE : SPR_STATION_PLATFORM_SW_NE)
                | session.TrackColours[SCHEME_SUPPORTS];
        }
        PaintAddImageAsParent(session, imageId, { 0, 0, height + fenceOffsetA }, { 32, 8, 1 });
        // height -= 5 (height)
        track_paint_util_draw_station_covers(session, EDGE_NW, hasFence, stationObj, coverHeight);
        // height += 5 (height + 5)

        if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 0)
        {
            imageId = (hasGreenLight ? SPR_STATION_PLATFORM_END_GREEN_LIGHT_SW_NE : SPR_STATION_PLATFORM_END_RED_LIGHT_SW_NE)
                | session.TrackColours[SCHEME_SUPPORTS];
        }
        else if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 2)
        {
            imageId = SPR_STATION_PLATFORM_BEGIN_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = SPR_STATION_PLATFORM_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
        }
        PaintAddImageAsParent(session, imageId, { 0, 24, height + fenceOffsetA }, { 32, 8, 1 });
        // height += 2 (height + 7)

        hasFence = track_paint_util_has_fence(EDGE_SE, position, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 0)
            {
                imageId = SPR_STATION_BEGIN_ANGLE_FENCE_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
            }
            else if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 2)
            {
                imageId = SPR_STATION_LIGHT_BACK_ANGLE_FENCED_NE_SW | session.TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId = SPR_STATION_FENCE_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
            }
            PaintAddImageAsParent(session, imageId, { 0, 31, height + fenceOffsetB }, { 32, 1, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 0)
        {
            // Addition: draw only small fence if there is an entrance/exit at the beginning
            imageId = SPR_STATION_FENCE_SMALL_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 31, 23, height + fenceOffsetB }, { 1, 8, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 2)
        {
            // Addition: draw only small fence if there is an entrance/exit at the end
            imageId = SPR_STATION_LIGHT_BACK_NE_SW | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 31, 23, height + fenceOffsetB }, { 1, 8, 7 });
        }
        // height -= 7 (height)
        track_paint_util_draw_station_covers(session, EDGE_SE, hasFence, stationObj, coverHeight);
        // height += 7 (height + 7)

        if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 0)
        {
            imageId = SPR_STATION_FENCE_SMALL_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 31, 0, height + fenceOffsetB }, { 1, 8, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 2)
        {
            imageId = SPR_STATION_LIGHT_BACK_NE_SW | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 31, 0, height + fenceOffsetB }, { 1, 8, 7 });
        }
    }
    else if (direction == 1 || direction == 3)
    {
        // height += 5 (height + 5);
        hasFence = track_paint_util_has_fence(EDGE_NE, position, trackElement, ride, session.CurrentRotation);

        if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 3)
        {
            if (hasGreenLight)
            {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_NW_SE
                                    : SPR_STATION_PLATFORM_END_GREEN_LIGHT_NW_SE)
                    | session.TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_NW_SE
                                    : SPR_STATION_PLATFORM_END_RED_LIGHT_NW_SE)
                    | session.TrackColours[SCHEME_SUPPORTS];
            }
        }
        else if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 1)
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_NW_SE : SPR_STATION_PLATFORM_BEGIN_NW_SE)
                | session.TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_NW_SE : SPR_STATION_PLATFORM_NW_SE)
                | session.TrackColours[SCHEME_SUPPORTS];
        }
        PaintAddImageAsParent(session, imageId, { 0, 0, height + fenceOffsetA }, { 8, 32, 1 });
        // height -= 5 (height)
        track_paint_util_draw_station_covers(session, EDGE_NE, hasFence, stationObj, coverHeight);
        // height += 5 (height + 5)

        if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 3)
        {
            imageId = (hasGreenLight ? SPR_STATION_PLATFORM_END_GREEN_LIGHT_NW_SE : SPR_STATION_PLATFORM_END_RED_LIGHT_NW_SE)
                | session.TrackColours[SCHEME_SUPPORTS];
        }
        else if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 1)
        {
            imageId = SPR_STATION_PLATFORM_BEGIN_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = SPR_STATION_PLATFORM_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
        }
        PaintAddImageAsParent(session, imageId, { 24, 0, height + fenceOffsetA }, { 8, 32, 1 });
        // height += 2 (height + 7)

        hasFence = track_paint_util_has_fence(EDGE_SW, position, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 3)
            {
                imageId = SPR_STATION_BEGIN_ANGLE_FENCE_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            }
            else if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 1)
            {
                imageId = SPR_STATION_LIGHT_BACK_ANGLE_FENCED_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId = SPR_STATION_FENCE_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            }
            PaintAddImageAsParent(session, imageId, { 31, 0, height + fenceOffsetB }, { 1, 32, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 3)
        {
            // Addition: draw only small fence if there is an entrance/exit at the beginning
            imageId = SPR_STATION_FENCE_SMALL_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 23, 31, height + fenceOffsetB }, { 8, 1, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 1)
        {
            // Addition: draw only small fence if there is an entrance/exit at the end
            imageId = SPR_STATION_LIGHT_BACK_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 23, 31, height + fenceOffsetB }, { 8, 1, 7 });
        }

        // height -= 7 (height)
        track_paint_util_draw_station_covers(session, EDGE_SW, hasFence, stationObj, coverHeight);
        // height += 7 (height + 7)

        if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 3)
        {
            imageId = SPR_STATION_FENCE_SMALL_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 0, 31, height + fenceOffsetB }, { 8, 1, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 1)
        {
            imageId = SPR_STATION_LIGHT_BACK_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 0, 31, height + fenceOffsetB }, { 8, 1, 7 });
        }
    }
}

void track_paint_util_draw_station_inverted(
    paint_session& session, const Ride& ride, Direction direction, int32_t height, const TrackElement& trackElement,
    uint8_t stationVariant)
{
    CoordsXY position = session.MapPosition;
    const auto* stationObj = ride.GetStationObject();
    const bool hasGreenLight = trackElement.HasGreenLight();

    if (stationObj != nullptr && stationObj->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS)
        return;

    bool hasFence;
    uint32_t imageId;

    if (direction == 0 || direction == 2)
    {
        // height += 5 (height + 5);
        hasFence = track_paint_util_has_fence(EDGE_NW, position, trackElement, ride, session.CurrentRotation);

        if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 0)
        {
            if (hasGreenLight)
            {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_SW_NE
                                    : SPR_STATION_PLATFORM_END_GREEN_LIGHT_SW_NE)
                    | session.TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_SW_NE
                                    : SPR_STATION_PLATFORM_END_RED_LIGHT_SW_NE)
                    | session.TrackColours[SCHEME_SUPPORTS];
            }
        }
        else if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 2)
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_SW_NE : SPR_STATION_PLATFORM_BEGIN_SW_NE)
                | session.TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_SW_NE : SPR_STATION_PLATFORM_SW_NE)
                | session.TrackColours[SCHEME_SUPPORTS];
        }
        PaintAddImageAsParent(session, imageId, { 0, 0, height + 6 }, { 32, 8, 1 });
        // height -= 5 (height)
        track_paint_util_draw_station_covers_2(session, EDGE_NW, hasFence, stationObj, height, stationVariant);
        // height += 5 (height + 5)

        if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 0)
        {
            imageId = (hasGreenLight ? SPR_STATION_PLATFORM_END_GREEN_LIGHT_SW_NE : SPR_STATION_PLATFORM_END_RED_LIGHT_SW_NE)
                | session.TrackColours[SCHEME_SUPPORTS];
        }
        else if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 2)
        {
            imageId = SPR_STATION_PLATFORM_BEGIN_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = SPR_STATION_PLATFORM_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
        }
        PaintAddImageAsParent(session, imageId, { 0, 24, height + 6 }, { 32, 8, 1 });
        // height += 2 (height + 7)

        hasFence = track_paint_util_has_fence(EDGE_SE, position, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 0)
            {
                imageId = SPR_STATION_INVERTED_BEGIN_ANGLE_FENCE_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
            }
            else if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 2)
            {
                imageId = SPR_STATION_INVERTED_LIGHT_BACK_ANGLE_FENCED_NE_SW | session.TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId = SPR_STATION_INVERTED_FENCE_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
            }
            PaintAddImageAsParent(session, imageId, { 0, 31, height + 8 }, { 32, 1, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 0)
        {
            // Addition: draw only small fence if there is an entrance/exit at the beginning
            imageId = SPR_STATION_FENCE_SMALL_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 31, 23, height + 8 }, { 1, 8, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 2)
        {
            // Addition: draw only small fence if there is an entrance/exit at the end
            imageId = SPR_STATION_LIGHT_BACK_NE_SW | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 31, 23, height + 8 }, { 1, 8, 7 });
        }
        // height -= 7 (height)
        track_paint_util_draw_station_covers_2(session, EDGE_SE, hasFence, stationObj, height, stationVariant);
        // height += 7 (height + 7)

        if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 0)
        {
            imageId = SPR_STATION_FENCE_SMALL_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 31, 0, height + 8 }, { 1, 8, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 2)
        {
            imageId = SPR_STATION_LIGHT_BACK_NE_SW | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 31, 0, height + 8 }, { 1, 8, 7 });
        }
    }
    else if (direction == 1 || direction == 3)
    {
        // height += 5 (height + 5);
        hasFence = track_paint_util_has_fence(EDGE_NE, position, trackElement, ride, session.CurrentRotation);

        if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 3)
        {
            if (hasGreenLight)
            {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_NW_SE
                                    : SPR_STATION_PLATFORM_END_GREEN_LIGHT_NW_SE)
                    | session.TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_NW_SE
                                    : SPR_STATION_PLATFORM_END_RED_LIGHT_NW_SE)
                    | session.TrackColours[SCHEME_SUPPORTS];
            }
        }
        else if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 1)
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_NW_SE : SPR_STATION_PLATFORM_BEGIN_NW_SE)
                | session.TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_NW_SE : SPR_STATION_PLATFORM_NW_SE)
                | session.TrackColours[SCHEME_SUPPORTS];
        }
        PaintAddImageAsParent(session, imageId, { 0, 0, height + 6 }, { 8, 32, 1 });
        // height -= 5 (height)
        track_paint_util_draw_station_covers_2(session, EDGE_NE, hasFence, stationObj, height, stationVariant);
        // height += 5 (height + 5)

        if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 3)
        {
            imageId = (hasGreenLight ? SPR_STATION_PLATFORM_END_GREEN_LIGHT_NW_SE : SPR_STATION_PLATFORM_END_RED_LIGHT_NW_SE)
                | session.TrackColours[SCHEME_SUPPORTS];
        }
        else if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 1)
        {
            imageId = SPR_STATION_PLATFORM_BEGIN_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = SPR_STATION_PLATFORM_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
        }
        PaintAddImageAsParent(session, imageId, { 24, 0, height + 6 }, { 8, 32, 1 });
        // height += 2 (height + 7)

        hasFence = track_paint_util_has_fence(EDGE_SW, position, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 3)
            {
                imageId = SPR_STATION_INVERTED_BEGIN_ANGLE_FENCE_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            }
            else if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 1)
            {
                imageId = SPR_STATION_INVERTED_LIGHT_BACK_ANGLE_FENCED_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId = SPR_STATION_INVERTED_FENCE_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            }
            PaintAddImageAsParent(session, imageId, { 31, 0, height + 8 }, { 1, 32, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 3)
        {
            // Addition: draw only small fence if there is an entrance/exit at the beginning
            imageId = SPR_STATION_FENCE_SMALL_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 23, 31, height + 8 }, { 8, 1, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 1)
        {
            // Addition: draw only small fence if there is an entrance/exit at the end
            imageId = SPR_STATION_LIGHT_BACK_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 23, 31, height + 8 }, { 8, 1, 7 });
        }

        // height -= 7 (height)
        track_paint_util_draw_station_covers_2(session, EDGE_SW, hasFence, stationObj, height, stationVariant);
        // height += 7 (height + 7)

        if (trackElement.GetTrackType() == TrackElemType::BeginStation && direction == 3)
        {
            imageId = SPR_STATION_FENCE_SMALL_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 0, 31, height + 8 }, { 8, 1, 7 });
        }
        else if (trackElement.GetTrackType() == TrackElemType::EndStation && direction == 1)
        {
            imageId = SPR_STATION_LIGHT_BACK_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 0, 31, height + 8 }, { 8, 1, 7 });
        }
    }
}

bool track_paint_util_draw_station_covers(
    paint_session& session, enum edge_t edge, bool hasFence, const StationObject* stationObject, uint16_t height)
{
    return track_paint_util_draw_station_covers_2(session, edge, hasFence, stationObject, height, STATION_VARIANT_BASIC);
}

bool track_paint_util_draw_station_covers_2(
    paint_session& session, enum edge_t edge, bool hasFence, const StationObject* stationObject, uint16_t height,
    uint8_t stationVariant)
{
    if (stationObject == nullptr)
    {
        return false;
    }

    if (!(session.Flags & (PaintSessionFlags::PassedSurface | PaintSessionFlags::IsTrackPiecePreview)))
    {
        return false;
    }

    auto baseImageIndex = stationObject->ShelterImageId;
    if (baseImageIndex == ImageIndexUndefined)
        return false;

    static constexpr const int16_t heights[][2] = {
        { 22, 0 },
        { 30, 0 },
        { 46, 0 },
    };

    int32_t imageOffset = 0;
    CoordsXYZ bounds, boundsOffset;
    CoordsXYZ offset = CoordsXYZ(0, 0, height);
    switch (edge)
    {
        case EDGE_NE:
            bounds = CoordsXYZ(1, 30, heights[stationVariant][0]);
            boundsOffset = CoordsXYZ(0, 1, height + 1);
            imageOffset = hasFence ? SPR_STATION_COVER_OFFSET_SE_NW_BACK_1 : SPR_STATION_COVER_OFFSET_SE_NW_BACK_0;
            break;
        case EDGE_SE:
            bounds = CoordsXYZ(32, 32, 0);
            boundsOffset = CoordsXYZ(0, 0, height + 1 + heights[stationVariant][0]);
            imageOffset = SPR_STATION_COVER_OFFSET_NE_SW_FRONT;
            break;
        case EDGE_SW:
            bounds = CoordsXYZ(32, 32, 0);
            boundsOffset = CoordsXYZ(0, 0, height + 1 + heights[stationVariant][0]);
            imageOffset = SPR_STATION_COVER_OFFSET_SE_NW_FRONT;
            break;
        case EDGE_NW:
            bounds = CoordsXYZ(30, 1, heights[stationVariant][0]);
            boundsOffset = CoordsXYZ(1, 0, height + 1);
            imageOffset = hasFence ? SPR_STATION_COVER_OFFSET_NE_SW_BACK_1 : SPR_STATION_COVER_OFFSET_NE_SW_BACK_0;
            break;
    }

    if (stationVariant == STATION_VARIANT_TALL)
    {
        imageOffset += SPR_STATION_COVER_OFFSET_TALL;
    }

    auto imageTemplate = ImageId::FromUInt32(session.TrackColours[SCHEME_TRACK]);
    auto imageId = imageTemplate.WithIndex(baseImageIndex + imageOffset);
    PaintAddImageAsParent(session, imageId, offset, bounds, boundsOffset);

    // Glass
    if (session.TrackColours[SCHEME_MISC] == IMAGE_TYPE_REMAP && (stationObject->Flags & STATION_OBJECT_FLAGS::IS_TRANSPARENT))
    {
        imageId = ImageId(baseImageIndex + imageOffset + 12).WithTransparancy(imageTemplate.GetPrimary());
        PaintAddImageAsChild(session, imageId, offset, bounds, boundsOffset);
    }
    return true;
}

void track_paint_util_draw_narrow_station_platform(
    paint_session& session, const Ride& ride, Direction direction, int32_t height, int32_t zOffset,
    const TrackElement& trackElement)
{
    CoordsXY position = session.MapPosition;
    const auto* stationObj = ride.GetStationObject();
    if (stationObj != nullptr && stationObj->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS)
        return;

    if (direction & 1)
    {
        bool hasFence = track_paint_util_has_fence(EDGE_NE, position, trackElement, ride, session.CurrentRotation);
        uint32_t imageId = (hasFence ? SPR_STATION_NARROW_EDGE_FENCED_NE : SPR_STATION_NARROW_EDGE_NE)
            | session.TrackColours[SCHEME_SUPPORTS];
        PaintAddImageAsParent(session, imageId, { 0, 0, height + zOffset }, { 8, 32, 1 });
        track_paint_util_draw_station_covers(session, EDGE_NE, hasFence, stationObj, height);

        imageId = SPR_STATION_NARROW_EDGE_SW | session.TrackColours[SCHEME_SUPPORTS];
        PaintAddImageAsParent(session, imageId, { 24, 0, height + zOffset }, { 8, 32, 1 });

        hasFence = track_paint_util_has_fence(EDGE_SW, position, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            imageId = SPR_STATION_FENCE_NW_SE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 31, 0, height + zOffset + 2 }, { 1, 32, 7 });
        }
        track_paint_util_draw_station_covers(session, EDGE_SW, hasFence, stationObj, height);
    }
    else
    {
        bool hasFence = track_paint_util_has_fence(EDGE_NW, position, trackElement, ride, session.CurrentRotation);
        uint32_t imageId = (hasFence ? SPR_STATION_NARROW_EDGE_FENCED_NW : SPR_STATION_NARROW_EDGE_NW)
            | session.TrackColours[SCHEME_SUPPORTS];
        PaintAddImageAsParent(session, imageId, { 0, 0, height + zOffset }, { 32, 8, 1 });
        track_paint_util_draw_station_covers(session, EDGE_NW, hasFence, stationObj, height);

        imageId = SPR_STATION_NARROW_EDGE_SE | session.TrackColours[SCHEME_SUPPORTS];
        PaintAddImageAsParent(session, imageId, { 0, 24, height + zOffset }, { 32, 8, 1 });

        hasFence = track_paint_util_has_fence(EDGE_SE, position, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            imageId = SPR_STATION_FENCE_SW_NE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 0, 31, height + zOffset + 2 }, { 32, 1, 7 });
        }
        track_paint_util_draw_station_covers(session, EDGE_SE, hasFence, stationObj, height);
    }
}

void track_paint_util_draw_pier(
    paint_session& session, const Ride& ride, const StationObject* stationObj, const CoordsXY& position, Direction direction,
    int32_t height, const TrackElement& trackElement, uint8_t rotation)
{
    if (stationObj != nullptr && stationObj->Flags & STATION_OBJECT_FLAGS::NO_PLATFORMS)
        return;

    bool hasFence;
    uint32_t imageId;

    if (direction & 1)
    {
        hasFence = track_paint_util_has_fence(EDGE_NE, position, trackElement, ride, session.CurrentRotation);
        imageId = (hasFence ? SPR_STATION_PIER_EDGE_NE_FENCED : SPR_STATION_PIER_EDGE_NE)
            | session.TrackColours[SCHEME_SUPPORTS];
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 6, 32, 1 }, { 2, 0, height });
        track_paint_util_draw_station_covers(session, EDGE_NE, hasFence, stationObj, height);

        imageId = SPR_STATION_PIER_EDGE_SW | session.TrackColours[SCHEME_SUPPORTS];
        PaintAddImageAsParent(session, imageId, { 24, 0, height }, { 8, 32, 1 });

        hasFence = track_paint_util_has_fence(EDGE_SW, position, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            imageId = SPR_STATION_PIER_FENCE_SW | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 31, 0, height + 2 }, { 1, 32, 7 });
        }
        track_paint_util_draw_station_covers(session, EDGE_SW, hasFence, stationObj, height);
    }
    else
    {
        hasFence = track_paint_util_has_fence(EDGE_NW, position, trackElement, ride, rotation);
        imageId = (hasFence ? SPR_STATION_PIER_EDGE_NW_FENCED : SPR_STATION_PIER_EDGE_NW)
            | session.TrackColours[SCHEME_SUPPORTS];
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 32, 6, 1 }, { 0, 2, height });
        track_paint_util_draw_station_covers(session, EDGE_NW, hasFence, stationObj, height);

        imageId = SPR_STATION_PIER_EDGE_SE | session.TrackColours[SCHEME_SUPPORTS];
        PaintAddImageAsParent(session, imageId, { 0, 24, height }, { 32, 8, 1 });

        hasFence = track_paint_util_has_fence(EDGE_SE, position, trackElement, ride, rotation);
        if (hasFence)
        {
            imageId = SPR_STATION_PIER_FENCE_SE | session.TrackColours[SCHEME_SUPPORTS];
            PaintAddImageAsParent(session, imageId, { 0, 31, height + 2 }, { 32, 1, 7 });
        }
        track_paint_util_draw_station_covers(session, EDGE_SE, hasFence, stationObj, height);
    }
}

void track_paint_util_draw_station_metal_supports(paint_session& session, Direction direction, uint16_t height, uint32_t colour)
{
    track_paint_util_draw_station_metal_supports_2(session, direction, height, colour, 3);
}

void track_paint_util_draw_station_metal_supports_2(
    paint_session& session, Direction direction, uint16_t height, uint32_t colour, uint8_t type)
{
    if (direction & 1)
    {
        metal_a_supports_paint_setup(session, type, 6, 0, height, colour);
        metal_a_supports_paint_setup(session, type, 7, 0, height, colour);
    }
    else
    {
        metal_a_supports_paint_setup(session, type, 5, 0, height, colour);
        metal_a_supports_paint_setup(session, type, 8, 0, height, colour);
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

static constexpr const int8_t right_helix_up_small_quarter_tiles_sprite_map[] = {
    0,
    -1,
    1,
    2,
};

void track_paint_util_right_helix_up_small_quarter_tiles_paint(
    paint_session& session, const int8_t thickness[2], int16_t height, Direction direction, uint8_t trackSequence,
    uint32_t colourFlags, const uint32_t sprites[4][3][2], const CoordsXY offsets[4][3][2],
    const CoordsXY boundsLengths[4][3][2], const CoordsXYZ boundsOffsets[4][3][2])
{
    int32_t index = right_helix_up_small_quarter_tiles_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    if (sprites[direction][index][0] != 0)
    {
        uint32_t imageId = sprites[direction][index][0] | colourFlags;
        CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index][0]);
        CoordsXY boundsLength = boundsLengths[direction][index][0];
        CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index][0]);

        PaintAddImageAsParent(
            session, imageId, { offset, height }, { boundsLength, thickness[0] },
            { boundsOffset.x, boundsOffset.y, height + boundsOffset.z });
    }
    if (sprites[direction][index][1] != 0)
    {
        uint32_t imageId = sprites[direction][index][1] | colourFlags;
        CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index][1]);
        CoordsXY boundsLength = boundsLengths[direction][index][1];
        CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index][1]);

        PaintAddImageAsParent(
            session, imageId, { offset, height }, { boundsLength, thickness[1] },
            { boundsOffset.x, boundsOffset.y, height + boundsOffset.z });
    }
}

// Need to change two numbers in here for the height (+ 28) Don't know whichone is which. This is to fix the Junior/Classic Mini
// Coaster's boundbox to match tunnels).
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

static constexpr const int8_t right_helix_up_large_quarter_sprite_map[] = {
    0, -1, 1, 2, -1, 3, 4,
};
void track_paint_util_right_helix_up_large_quarter_tiles_paint(
    paint_session& session, const int8_t thickness[2], int16_t height, Direction direction, uint8_t trackSequence,
    uint32_t colourFlags, const uint32_t sprites[4][5][2], const CoordsXY offsets[4][5][2],
    const CoordsXY boundsLengths[4][5][2], const CoordsXYZ boundsOffsets[4][5][2])
{
    int32_t index = right_helix_up_large_quarter_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    if (sprites[direction][index][0] != 0)
    {
        uint32_t imageId = sprites[direction][index][0] | colourFlags;
        CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index][0]);
        CoordsXY boundsLength = boundsLengths[direction][index][0];
        CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index][0]);

        PaintAddImageAsParent(
            session, imageId, { offset, height }, { boundsLength, thickness[0] },
            { boundsOffset.x, boundsOffset.y, height + boundsOffset.z });
    }
    if (sprites[direction][index][1] != 0)
    {
        uint32_t imageId = sprites[direction][index][1] | colourFlags;
        CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index][1]);
        CoordsXY boundsLength = boundsLengths[direction][index][1];
        CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index][1]);

        PaintAddImageAsParent(
            session, imageId, { offset, height }, { boundsLength, thickness[1] },
            { boundsOffset.x, boundsOffset.y, height + boundsOffset.z });
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
        { 28, 28 },
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
        { 4, 4, 0 },
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

static constexpr const int8_t eighth_to_diag_sprite_map[] = {
    0, 1, 2, -1, 3,
};
void track_paint_util_eighth_to_diag_tiles_paint(
    paint_session& session, const int8_t thickness[4][4], int16_t height, Direction direction, uint8_t trackSequence,
    uint32_t colourFlags, const uint32_t sprites[4][4], const CoordsXY offsets[4][4], const CoordsXY boundsLengths[4][4],
    const CoordsXYZ boundsOffsets[4][4])
{
    int32_t index = eighth_to_diag_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    uint32_t imageId = sprites[direction][index] | colourFlags;
    CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index]);
    CoordsXY boundsLength = boundsLengths[direction][index];
    CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index]);

    PaintAddImageAsParent(
        session, imageId, { offset.x, offset.y, height }, { boundsLength.x, boundsLength.y, thickness[direction][index] },
        { boundsOffset.x, boundsOffset.y, height + boundsOffset.z });
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

static constexpr const int8_t diag_sprite_map[4][4] = {
    { -1, 0, -1, -1 },
    { -1, -1, -1, 0 },
    { -1, -1, 0, -1 },
    { 0, -1, -1, -1 },
};

void track_paint_util_diag_tiles_paint(
    paint_session& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence, uint32_t colourFlags,
    const uint32_t sprites[4], const CoordsXY offsets[4], const CoordsXY boundsLengths[4], const CoordsXYZ boundsOffsets[4])
{
    int32_t index = diag_sprite_map[direction][trackSequence];
    if (index < 0)
    {
        return;
    }

    uint32_t imageId = sprites[direction] | colourFlags;
    CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction]);
    CoordsXY boundsLength = boundsLengths[direction];
    CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction]);

    PaintAddImageAsParent(
        session, imageId, { offset, height }, { boundsLength, thickness },
        { boundsOffset.x, boundsOffset.y, height + boundsOffset.z });
}

const uint8_t mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[] = {
    6, 4, 5, 3, 1, 2, 0,
};

constexpr CoordsXY defaultRightQuarterTurn5TilesOffsets[4][5] = {
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

constexpr CoordsXYZ defaultRightQuarterTurn5TilesBoundOffsets[4][5] = {
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

constexpr CoordsXY defaultRightQuarterTurn5TilesBoundLengths[4][5] = {
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

static constexpr const int8_t right_quarter_turn_5_tiles_sprite_map[] = {
    0, -1, 1, 2, -1, 3, 4,
};

void track_paint_util_right_quarter_turn_5_tiles_paint(
    paint_session& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence, uint32_t colourFlags,
    const uint32_t sprites[4][5], const CoordsXY offsets[4][5], const CoordsXY boundsLengths[4][5],
    const CoordsXYZ boundsOffsets[4][5])
{
    int32_t index = right_quarter_turn_5_tiles_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    uint32_t imageId = sprites[direction][index] | colourFlags;
    CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index]);
    CoordsXY boundsLength = boundsLengths[direction][index];
    CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index]);

    PaintAddImageAsParent(
        session, imageId, { static_cast<int8_t>(offset.x), static_cast<int8_t>(offset.y), height },
        { boundsLength.x, boundsLength.y, thickness }, { boundsOffset.x, boundsOffset.y, height + boundsOffset.z });
}

void track_paint_util_right_quarter_turn_5_tiles_paint_2(
    paint_session& session, int16_t height, Direction direction, uint8_t trackSequence, uint32_t colourFlags,
    const sprite_bb sprites[][5])
{
    int8_t sprite = right_quarter_turn_5_tiles_sprite_map[trackSequence];
    if (sprite < 0)
    {
        return;
    }

    const sprite_bb* spriteBB = &sprites[direction][sprite];
    const uint32_t imageId = spriteBB->sprite_id | colourFlags;
    const auto& offset = spriteBB->offset;
    const auto& bbOffset = spriteBB->bb_offset;
    PaintAddImageAsParent(
        session, imageId, { offset.x, offset.y, height + offset.z }, spriteBB->bb_size,
        { bbOffset.x, bbOffset.y, height + bbOffset.z });
}

void track_paint_util_right_quarter_turn_5_tiles_paint_3(
    paint_session& session, int16_t height, Direction direction, uint8_t trackSequence, uint32_t colourFlags,
    const sprite_bb sprites[][5])
{
    int8_t sprite = right_quarter_turn_5_tiles_sprite_map[trackSequence];
    if (sprite < 0)
    {
        return;
    }

    const sprite_bb* spriteBB = &sprites[direction][sprite];
    const uint32_t imageId = spriteBB->sprite_id | colourFlags;
    const auto& offset = spriteBB->offset;
    PaintAddImageAsParent(session, imageId, { offset.x, offset.y, height + offset.z }, spriteBB->bb_size);
}

void track_paint_util_right_quarter_turn_5_tiles_tunnel(
    paint_session& session, int16_t height, Direction direction, uint8_t trackSequence, uint8_t tunnelType)
{
    if (direction == 0 && trackSequence == 0)
    {
        paint_util_push_tunnel_left(session, height, tunnelType);
    }
    if (direction == 0 && trackSequence == 6)
    {
        paint_util_push_tunnel_right(session, height, tunnelType);
    }
    if (direction == 1 && trackSequence == 6)
    {
        paint_util_push_tunnel_left(session, height, tunnelType);
    }
    if (direction == 3 && trackSequence == 0)
    {
        paint_util_push_tunnel_right(session, height, tunnelType);
    }
}

void track_paint_util_right_quarter_turn_5_tiles_wooden_supports(
    paint_session& session, int16_t height, Direction direction, uint8_t trackSequence)
{
    if (trackSequence != 1 && trackSequence != 4)
    {
        static constexpr const uint8_t supportTypes[][7] = {
            { 0, 0xFF, 4, 2, 0xFF, 4, 1 },
            { 1, 0xFF, 5, 3, 0xFF, 5, 0 },
            { 0, 0xFF, 2, 4, 0xFF, 2, 1 },
            { 1, 0xFF, 3, 5, 0xFF, 3, 0 },
        };
        uint8_t supportType = supportTypes[direction][trackSequence];
        wooden_a_supports_paint_setup(session, supportType, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
}

const uint8_t mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[] = {
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

static constexpr const int8_t right_quarter_turn_3_tiles_sprite_map[] = {
    0,
    -1,
    1,
    2,
};

void track_paint_util_right_quarter_turn_3_tiles_paint(
    paint_session& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence, uint32_t colourFlags,
    const uint32_t sprites[4][3], const CoordsXY offsets[4][3], const CoordsXY boundsLengths[4][3],
    const CoordsXYZ boundsOffsets[4][3])
{
    int32_t index = right_quarter_turn_3_tiles_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    uint32_t imageId = sprites[direction][index] | colourFlags;
    CoordsXY offset = (offsets == nullptr ? CoordsXY() : offsets[direction][index]);
    CoordsXY boundsLength = boundsLengths[direction][index];
    CoordsXYZ boundsOffset = (boundsOffsets == nullptr ? CoordsXYZ(offset, 0) : boundsOffsets[direction][index]);

    PaintAddImageAsParent(
        session, imageId, { offset, height }, { boundsLength.x, boundsLength.y, thickness },
        { boundsOffset.x, boundsOffset.y, height + boundsOffset.z });
}

void track_paint_util_right_quarter_turn_3_tiles_paint_2(
    paint_session& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence, uint32_t colourFlags,
    const uint32_t sprites[4][3])
{
    track_paint_util_right_quarter_turn_3_tiles_paint_2_with_height_offset(
        session, thickness, height, direction, trackSequence, colourFlags, sprites, 0);
}

void track_paint_util_right_quarter_turn_3_tiles_paint_2_with_height_offset(
    paint_session& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence, uint32_t colourFlags,
    const uint32_t sprites[4][3], int32_t heightOffset)
{
    int8_t sprite = right_quarter_turn_3_tiles_sprite_map[trackSequence];
    if (sprite < 0)
    {
        return;
    }

    uint32_t imageId = sprites[(direction + 1) % 4][sprite] | colourFlags;

    switch (direction)
    {
        case 0:
            switch (trackSequence)
            {
                case 0:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 32, 20, thickness }, { 0, 6, height + heightOffset });
                    break;
                case 2:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 16, 16, thickness }, { 16, 16, height + heightOffset });
                    break;
                case 3:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 20, 32, thickness }, { 6, 0, height + heightOffset });
                    break;
            }
            break;

        case 1:
            switch (trackSequence)
            {
                case 0:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 20, 32, thickness }, { 6, 0, height + heightOffset });
                    break;
                case 2:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 16, 16, thickness }, { 16, 0, height + heightOffset });
                    break;
                case 3:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 32, 20, thickness }, { 0, 6, height + heightOffset });
                    break;
            }
            break;

        case 2:
            switch (trackSequence)
            {
                case 0:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 32, 20, thickness }, { 0, 6, height + heightOffset });
                    break;
                case 2:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 16, 16, thickness }, { 0, 0, height + heightOffset });
                    break;
                case 3:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 20, 32, thickness }, { 6, 0, height + heightOffset });
                    break;
            }
            break;

        case 3:
            switch (trackSequence)
            {
                case 0:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 20, 32, thickness }, { 6, 0, height + heightOffset });
                    break;
                case 2:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 16, 16, thickness }, { 0, 16, height + heightOffset });
                    break;
                case 3:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 32, 20, thickness }, { 0, 6, height + heightOffset });
                    break;
            }
            break;
    }
}

void track_paint_util_right_quarter_turn_3_tiles_paint_3(
    paint_session& session, int16_t height, Direction direction, uint8_t trackSequence, uint32_t colourFlags,
    const sprite_bb sprites[4][3])
{
    int8_t sprite = right_quarter_turn_3_tiles_sprite_map[trackSequence];
    if (sprite < 0)
    {
        return;
    }
    const sprite_bb* spriteBB = &sprites[direction][sprite];
    const auto& offset = spriteBB->offset;
    const auto& bbOffset = spriteBB->bb_offset;
    PaintAddImageAsParent(
        session, spriteBB->sprite_id | colourFlags, { offset.x, offset.y, offset.z + height }, spriteBB->bb_size,
        { bbOffset.x, bbOffset.y, height + bbOffset.z });
}

void track_paint_util_right_quarter_turn_3_tiles_paint_4(
    paint_session& session, int16_t height, Direction direction, uint8_t trackSequence, uint32_t colourFlags,
    const sprite_bb sprites[4][3])
{
    int8_t sprite = right_quarter_turn_3_tiles_sprite_map[trackSequence];
    if (sprite < 0)
    {
        return;
    }

    const sprite_bb* spriteBB = &sprites[direction][sprite];
    const uint32_t imageId = spriteBB->sprite_id | colourFlags;
    const auto& offset = spriteBB->offset;
    PaintAddImageAsParent(session, imageId, { offset.x, offset.y, height + offset.z }, spriteBB->bb_size);
}

void track_paint_util_right_quarter_turn_3_tiles_tunnel(
    paint_session& session, int16_t height, Direction direction, uint8_t trackSequence, uint8_t tunnelType)
{
    if (direction == 0 && trackSequence == 0)
    {
        paint_util_push_tunnel_left(session, height, tunnelType);
    }

    if (direction == 0 && trackSequence == 3)
    {
        paint_util_push_tunnel_right(session, height, tunnelType);
    }

    if (direction == 1 && trackSequence == 3)
    {
        paint_util_push_tunnel_left(session, height, tunnelType);
    }

    if (direction == 3 && trackSequence == 0)
    {
        paint_util_push_tunnel_right(session, height, tunnelType);
    }
}

void track_paint_util_right_quarter_turn_3_tiles_25_deg_up_tunnel(
    paint_session& session, int16_t height, Direction direction, uint8_t trackSequence, uint8_t tunnelType0,
    uint8_t tunnelType3)
{
    if (direction == 0 && trackSequence == 0)
    {
        paint_util_push_tunnel_left(session, height - 8, tunnelType0);
    }
    if (direction == 0 && trackSequence == 3)
    {
        paint_util_push_tunnel_right(session, height + 8, tunnelType3);
    }
    if (direction == 1 && trackSequence == 3)
    {
        paint_util_push_tunnel_left(session, height + 8, tunnelType3);
    }
    if (direction == 3 && trackSequence == 0)
    {
        paint_util_push_tunnel_right(session, height - 8, tunnelType0);
    }
}

void track_paint_util_right_quarter_turn_3_tiles_25_deg_down_tunnel(
    paint_session& session, int16_t height, Direction direction, uint8_t trackSequence, uint8_t tunnelType0,
    uint8_t tunnelType3)
{
    if (direction == 0 && trackSequence == 0)
    {
        paint_util_push_tunnel_left(session, height + 8, tunnelType0);
    }
    if (direction == 0 && trackSequence == 3)
    {
        paint_util_push_tunnel_right(session, height - 8, tunnelType3);
    }
    if (direction == 1 && trackSequence == 3)
    {
        paint_util_push_tunnel_left(session, height - 8, tunnelType3);
    }
    if (direction == 3 && trackSequence == 0)
    {
        paint_util_push_tunnel_right(session, height + 8, tunnelType0);
    }
}

static constexpr const int8_t left_quarter_turn_3_tiles_sprite_map[] = {
    2,
    -1,
    1,
    0,
};

void track_paint_util_left_quarter_turn_3_tiles_paint(
    paint_session& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence, uint32_t colourFlags,
    const uint32_t sprites[4][3])
{
    track_paint_util_left_quarter_turn_3_tiles_paint_with_height_offset(
        session, thickness, height, direction, trackSequence, colourFlags, sprites, 0);
}

void track_paint_util_left_quarter_turn_3_tiles_paint_with_height_offset(
    paint_session& session, int8_t thickness, int16_t height, Direction direction, uint8_t trackSequence, uint32_t colourFlags,
    const uint32_t sprites[4][3], int32_t heightOffset)
{
    int8_t sprite = left_quarter_turn_3_tiles_sprite_map[trackSequence];
    if (sprite < 0)
    {
        return;
    }

    uint32_t imageId = sprites[(direction + 1) % 4][sprite] | colourFlags;

    switch (direction)
    {
        case 0:
            switch (trackSequence)
            {
                case 0:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 32, 20, thickness }, { 0, 6, height + heightOffset });
                    break;
                case 2:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 16, 16, thickness }, { 16, 0, height + heightOffset });
                    break;
                case 3:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 20, 32, thickness }, { 6, 0, height + heightOffset });
                    break;
            }
            break;

        case 1:
            switch (trackSequence)
            {
                case 0:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 20, 32, thickness }, { 6, 0, height + heightOffset });
                    break;
                case 2:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 16, 16, thickness }, { 0, 0, height + heightOffset });
                    break;
                case 3:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 32, 20, thickness }, { 0, 6, height + heightOffset });
                    break;
            }
            break;

        case 2:
            switch (trackSequence)
            {
                case 0:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 32, 20, thickness }, { 0, 6, height + heightOffset });
                    break;
                case 2:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 16, 16, thickness }, { 0, 16, height + heightOffset });
                    break;
                case 3:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 20, 32, thickness }, { 6, 0, height + heightOffset });
                    break;
            }
            break;

        case 3:
            switch (trackSequence)
            {
                case 0:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 20, 32, thickness }, { 6, 0, height + heightOffset });
                    break;
                case 2:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 16, 16, thickness }, { 16, 16, height + heightOffset });
                    break;
                case 3:
                    PaintAddImageAsParent(
                        session, imageId, { 0, 0, height }, { 32, 20, thickness }, { 0, 6, height + heightOffset });
                    break;
            }
            break;
    }
}

void track_paint_util_left_quarter_turn_3_tiles_tunnel(
    paint_session& session, int16_t height, uint8_t tunnelType, Direction direction, uint8_t trackSequence)
{
    if (direction == 0 && trackSequence == 0)
    {
        paint_util_push_tunnel_left(session, height, tunnelType);
    }

    if (direction == 2 && trackSequence == 3)
    {
        paint_util_push_tunnel_right(session, height, tunnelType);
    }

    if (direction == 3 && trackSequence == 0)
    {
        paint_util_push_tunnel_right(session, height, tunnelType);
    }

    if (direction == 3 && trackSequence == 3)
    {
        paint_util_push_tunnel_left(session, height, tunnelType);
    }
}

void track_paint_util_left_quarter_turn_1_tile_paint(
    paint_session& session, int8_t thickness, int16_t height, int16_t boundBoxZOffset, Direction direction,
    uint32_t colourFlags, const uint32_t* sprites)
{
    uint32_t imageId = sprites[direction] | colourFlags;

    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(
                session, imageId, { 0, 0, height }, { 26, 24, thickness }, { 6, 2, height + boundBoxZOffset });
            break;
        case 1:
            PaintAddImageAsParent(
                session, imageId, { 0, 0, height }, { 26, 26, thickness }, { 0, 0, height + boundBoxZOffset });
            break;
        case 2:
            PaintAddImageAsParent(
                session, imageId, { 0, 0, height }, { 24, 26, thickness }, { 2, 6, height + boundBoxZOffset });
            break;
        case 3:
            PaintAddImageAsParent(
                session, imageId, { 0, 0, height }, { 24, 24, thickness }, { 6, 6, height + boundBoxZOffset });
            break;
    }
}

void track_paint_util_right_quarter_turn_1_tile_tunnel(
    paint_session& session, Direction direction, uint16_t baseHeight, int8_t startOffset, uint8_t startTunnel, int8_t endOffset,
    uint8_t endTunnel)
{
    track_paint_util_left_quarter_turn_1_tile_tunnel(
        session, (direction + 3) % 4, baseHeight, endOffset, endTunnel, startOffset, startTunnel);
}

void track_paint_util_left_quarter_turn_1_tile_tunnel(
    paint_session& session, Direction direction, uint16_t baseHeight, int8_t startOffset, uint8_t startTunnel, int8_t endOffset,
    uint8_t endTunnel)
{
    switch (direction)
    {
        case 0:
            paint_util_push_tunnel_left(session, baseHeight + startOffset, startTunnel);
            break;
        case 2:
            paint_util_push_tunnel_right(session, baseHeight + endOffset, endTunnel);
            break;
        case 3:
            paint_util_push_tunnel_right(session, baseHeight + startOffset, startTunnel);
            paint_util_push_tunnel_left(session, baseHeight + endOffset, endTunnel);
            break;
    }
}

void track_paint_util_spinning_tunnel_paint(paint_session& session, int8_t thickness, int16_t height, Direction direction)
{
    int32_t frame = gCurrentTicks >> 2 & 3;
    uint32_t colourFlags = session.TrackColours[SCHEME_SUPPORTS];

    uint32_t colourFlags2 = session.TrackColours[SCHEME_TRACK];
    if (colourFlags2 & IMAGE_TYPE_REMAP_2_PLUS)
    {
        colourFlags |= colourFlags2 & (IMAGE_TYPE_REMAP_2_PLUS | (0x1F << 24));
    }

    uint32_t imageId = trackSpritesGhostTrainSpinningTunnel[direction & 1][0][frame] | colourFlags;
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 28, 20, thickness }, { 2, 6, height });
    }
    else
    {
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { 20, 28, thickness }, { 6, 2, height });
    }

    imageId = trackSpritesGhostTrainSpinningTunnel[direction & 1][1][frame] | colourFlags;
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 26, 1, 23 }, { 4, 28, height });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 26, 23 }, { 28, 4, height });
    }
}

void track_paint_util_onride_photo_small_paint(
    paint_session& session, Direction direction, int32_t height, const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { SPR_ON_RIDE_PHOTO_SIGN_SMALL_SW_NE, SPR_ON_RIDE_PHOTO_CAMERA_SMALL_S, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_S },
        { SPR_ON_RIDE_PHOTO_SIGN_SMALL_NW_SE, SPR_ON_RIDE_PHOTO_CAMERA_SMALL_W, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_W },
        { SPR_ON_RIDE_PHOTO_SIGN_SMALL_NE_SW, SPR_ON_RIDE_PHOTO_CAMERA_SMALL_N, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_N },
        { SPR_ON_RIDE_PHOTO_SIGN_SMALL_SE_NW, SPR_ON_RIDE_PHOTO_CAMERA_SMALL_E, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_E },
    };

    bool takingPhoto = trackElement.IsTakingPhoto();
    uint32_t imageId = imageIds[direction][0] | session.TrackColours[SCHEME_MISC];
    uint32_t flashImageId = imageIds[direction][takingPhoto ? 2 : 1] | session.TrackColours[SCHEME_MISC];
    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(session, imageId, { 26, 0, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, imageId, { 26, 31, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, flashImageId, { 6, 0, height }, { 1, 1, 19 });
            break;
        case 1:
            PaintAddImageAsParent(session, imageId, { 0, 6, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, imageId, { 31, 6, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, flashImageId, { 0, 26, height }, { 1, 1, 19 });
            break;
        case 2:
            PaintAddImageAsParent(session, imageId, { 6, 0, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, imageId, { 6, 31, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, flashImageId, { 26, 31, height }, { 1, 1, 19 });
            break;
        case 3:
            PaintAddImageAsParent(session, imageId, { 0, 26, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, imageId, { 31, 26, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, flashImageId, { 31, 6, height }, { 1, 1, 19 });
            break;
    }
}

void track_paint_util_onride_photo_paint(
    paint_session& session, Direction direction, int32_t height, const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { SPR_ON_RIDE_PHOTO_SIGN_SW_NE, SPR_ON_RIDE_PHOTO_CAMERA_S, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_S },
        { SPR_ON_RIDE_PHOTO_SIGN_NW_SE, SPR_ON_RIDE_PHOTO_CAMERA_W, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_W },
        { SPR_ON_RIDE_PHOTO_SIGN_NE_SW, SPR_ON_RIDE_PHOTO_CAMERA_N, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_N },
        { SPR_ON_RIDE_PHOTO_SIGN_SE_NW, SPR_ON_RIDE_PHOTO_CAMERA_E, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_E },
    };

    bool takingPhoto = trackElement.IsTakingPhoto();
    uint32_t imageId = imageIds[direction][0] | session.TrackColours[SCHEME_MISC];
    uint32_t flashImageId = imageIds[direction][takingPhoto ? 2 : 1] | session.TrackColours[SCHEME_MISC];
    switch (direction)
    {
        case 0:
            PaintAddImageAsParent(session, imageId, { 26, 0, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, imageId, { 26, 31, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, flashImageId, { 6, 0, height }, { 1, 1, 19 });
            break;
        case 1:
            PaintAddImageAsParent(session, imageId, { 0, 6, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, imageId, { 31, 6, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, flashImageId, { 0, 26, height }, { 1, 1, 19 });
            break;
        case 2:
            PaintAddImageAsParent(session, imageId, { 6, 0, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, imageId, { 6, 31, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, flashImageId, { 26, 31, height }, { 1, 1, 19 });
            break;
        case 3:
            PaintAddImageAsParent(session, imageId, { 0, 26, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, imageId, { 31, 26, height }, { 1, 1, 19 });
            PaintAddImageAsParent(session, flashImageId, { 31, 6, height }, { 1, 1, 19 });
            break;
    }
}

static constexpr const uint16_t RightVerticalLoopSegments[] = {
    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
    SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4,
    SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
    0,
    0,
    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0,
    SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC,
    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0,
    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0,
};

void track_paint_util_right_vertical_loop_segments(paint_session& session, Direction direction, uint8_t trackSequence)
{
    if (trackSequence > 9)
    {
        // P
        return;
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(RightVerticalLoopSegments[trackSequence], direction), 0xFFFF, 0);
}

void track_paint_util_left_corkscrew_up_supports(paint_session& session, Direction direction, uint16_t height)
{
    // TODO: Figure out which of these looks best, and use one to keep a consistent world
    if (direction == 2)
    {
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
    }
    metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    if (direction != 2)
    {
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
    }
}

/**
 *
 *  rct2: 0x006C4794
 */
void PaintTrack(paint_session& session, Direction direction, int32_t height, const TrackElement& trackElement)
{
    RideId rideIndex = trackElement.GetRideIndex();
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
    {
        log_error("Attempted to paint invalid ride: %d", rideIndex);
        return;
    }

    if ((!gTrackDesignSaveMode || rideIndex == gTrackDesignSaveRideIndex)
        && !(session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
    {
        auto trackType = trackElement.GetTrackType();
        int32_t trackSequence = trackElement.GetSequenceIndex();
        int32_t trackColourScheme = trackElement.GetColourScheme();

        if (PaintShouldShowHeightMarkers(session, VIEWPORT_FLAG_TRACK_HEIGHTS))
        {
            session.InteractionType = ViewportInteractionItem::None;
            const auto& ted = GetTrackElementDescriptor(trackType);
            if (ted.HeightMarkerPositions & (1 << trackSequence))
            {
                uint16_t ax = ride->GetRideTypeDescriptor().Heights.VehicleZOffset;
                // 0x1689 represents 0 height there are -127 to 128 heights above and below it
                // There are 3 arrays of 256 heights (units, m, ft) chosen with the get_height_marker_offset()
                uint32_t imageId = SPRITE_ID_PALETTE_COLOUR_1(COLOUR_LIGHT_BLUE) | (0x1689 + get_height_marker_offset());
                auto heightNum = (height + 8) / 16 - gMapBaseZ;

                PaintAddImageAsParent(
                    session, imageId + heightNum, { 16, 16, height + ax + 3 }, { 1, 1, 0 }, { 1000, 1000, 2047 });
            }
        }

#ifdef __ENABLE_LIGHTFX__
        if (lightfx_is_available())
        {
            uint8_t zOffset = 16;
            if (ride->type == RIDE_TYPE_TOILETS || ride->type == RIDE_TYPE_FIRST_AID || ride->type == RIDE_TYPE_CASH_MACHINE)
                zOffset = 23;

            if (ride->type == RIDE_TYPE_INFORMATION_KIOSK)
                LightFxAddKioskLights(session.MapPosition, height, zOffset);
            else if (RideTypeDescriptors[ride->type].HasFlag(RIDE_TYPE_FLAG_IS_SHOP))
                LightFxAddShopLights(session.MapPosition, trackElement.GetDirection(), height, zOffset);
        }
#endif

        session.InteractionType = ViewportInteractionItem::Ride;
        session.TrackColours[SCHEME_TRACK] = SPRITE_ID_PALETTE_COLOUR_2(
            ride->track_colour[trackColourScheme].main, ride->track_colour[trackColourScheme].additional);
        session.TrackColours[SCHEME_SUPPORTS] = SPRITE_ID_PALETTE_COLOUR_1(ride->track_colour[trackColourScheme].supports);
        session.TrackColours[SCHEME_MISC] = IMAGE_TYPE_REMAP;
        session.TrackColours[SCHEME_3] = 0x20C00000;
        if (trackElement.IsHighlighted())
        {
            session.TrackColours[SCHEME_TRACK] = 0x21600000;
            session.TrackColours[SCHEME_SUPPORTS] = 0x21600000;
            session.TrackColours[SCHEME_MISC] = 0x21600000;
            session.TrackColours[SCHEME_3] = 0x21600000;
        }
        if (trackElement.IsGhost())
        {
            uint32_t ghost_id = CONSTRUCTION_MARKER;
            session.InteractionType = ViewportInteractionItem::None;
            session.TrackColours[SCHEME_TRACK] = ghost_id;
            session.TrackColours[SCHEME_SUPPORTS] = ghost_id;
            session.TrackColours[SCHEME_MISC] = ghost_id;
            session.TrackColours[SCHEME_3] = ghost_id;
        }

        if (ride->type >= RIDE_TYPE_COUNT)
        {
            return;
        }

        const auto& rtd = GetRideTypeDescriptor(trackElement.GetRideType());
        TRACK_PAINT_FUNCTION_GETTER paintFunctionGetter = rtd.TrackPaintFunction;
        if (paintFunctionGetter != nullptr)
        {
            TRACK_PAINT_FUNCTION paintFunction = paintFunctionGetter(trackType);
            if (paintFunction != nullptr)
            {
                paintFunction(session, *ride, trackSequence, direction, height, trackElement);
            }
        }
    }
}
