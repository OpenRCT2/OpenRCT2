/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../config/Config.h"
#include "../drawing/Drawing.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../paint/Paint.h"
#include "../paint/Supports.h"
#include "../paint/tile_element/Paint.TileElement.h"
#include "../scenario/Scenario.h"
#include "../sprites.h"
#include "../world/Map.h"
#include "../world/Sprite.h"
#include "TrackData.h"
#include "TrackDesign.h"
#include "RideData.h"
#include "TrackPaint.h"
#include "Station.h"

// clang-format off
/* rct2: 0x007667AC */
static LocationXY16 loc_7667AC[] = {
    { -1, 0 },
    { 0,  -1 },
    { 1,  0 },
    { 0,  1 },
};

/* rct2: 0x007667AE */
static LocationXY16 loc_7667AE[] = {
    { 0, -1 },
    { 1, 0 },
    { 0, 1 },
    { -1, 0 },
};

const uint8_t track_map_2x2[][4] = {
    {0, 1, 2, 3},
    {1, 3, 0, 2},
    {3, 2, 1, 0},
    {2, 0, 3, 1}
};

const uint8_t edges_2x2[] = {
    EDGE_NE | EDGE_NW,
    EDGE_NE | EDGE_SE,
    EDGE_SW | EDGE_NW,
    EDGE_SW | EDGE_SE,
};

const uint8_t track_map_3x3[][9] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8},
    {0, 3, 5, 7, 2, 8, 1, 6, 4},
    {0, 7, 8, 6, 5, 4, 3, 1, 2},
    {0, 6, 4, 1, 8, 2, 7, 3, 5}
};

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

const uint8_t track_map_4x4[][16] = {
    {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15},
    {3,  7,  11, 15, 2,  6,  10, 14, 1,  5,  9,  13, 0,  4,  8,  12},
    {15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1,  0},
    {12, 8,  4,  0,  13, 9,  5,  1,  14, 10, 6,  2,  15, 11, 7,  3},
};

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
    EDGE_SW | EDGE_SE
};

const uint8_t track_map_1x4[][4] = {
    {0, 1, 2, 3},
    {2, 3, 0, 1},
    {2, 3, 0, 1},
    {0, 1, 2, 3},
};

const uint32_t floorSpritesCork[] = {
    SPR_FLOOR_CORK_SE_SW,
    SPR_FLOOR_CORK_SW,
    SPR_FLOOR_CORK_SE,
    SPR_FLOOR_CORK
};

const uint32_t fenceSpritesRope[] = {
    SPR_FENCE_ROPE_NE,
    SPR_FENCE_ROPE_SE,
    SPR_FENCE_ROPE_SW,
    SPR_FENCE_ROPE_NW
};

const uint32_t fenceSpritesMetalB[] = {
    SPR_FENCE_METAL_B_NE,
    SPR_FENCE_METAL_B_SE,
    SPR_FENCE_METAL_B_SW,
    SPR_FENCE_METAL_B_NW
};

const uint32_t trackSpritesSubmarineRideMiniHelicoptersQuarterTurn3Tiles[4][3] = {
    {
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_0,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_1,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SW_SE_PART_2
    }, {
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_0,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_1,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NW_SW_PART_2
    }, {
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_0,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_1,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_NE_NW_PART_2
    }, {
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_0,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_1,
        SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_QUARTER_TURN_3_TILES_SE_NE_PART_2
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
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_SW_NE_FRAME_3
        },
        {
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_SW_NE_FRAME_0,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_SW_NE_FRAME_1,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_SW_NE_FRAME_2,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_SW_NE_FRAME_3
        }
    },
    {
        {
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_NW_SE_FRAME_0,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_NW_SE_FRAME_1,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_NW_SE_FRAME_2,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_BACK_NW_SE_FRAME_3
        },
        {
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_NW_SE_FRAME_0,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_NW_SE_FRAME_1,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_NW_SE_FRAME_2,
            SPR_GHOST_TRAIN_SPINNING_TUNNEL_FRONT_NW_SE_FRAME_3
        }
    }
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
// clang-format on

bool track_paint_util_has_fence(
    enum edge_t edge, LocationXY16 position, const rct_tile_element * tileElement, Ride * ride, uint8_t rotation)
{
    LocationXY16 offset = { 0, 0 };
    switch (edge)
    {
    case EDGE_NE:
        offset = loc_7667AC[rotation];
        break;
    case EDGE_SE:
        offset = loc_7667AE[(rotation + 2) & 3];
        break;
    case EDGE_SW:
        offset = loc_7667AC[(rotation + 2) & 3];
        break;
    case EDGE_NW:
        offset = loc_7667AE[rotation];
        break;
    }

    int32_t entranceX = (position.x / 32) + offset.x;
    int32_t entranceY = (position.y / 32) + offset.y;

    int32_t entranceId = tile_element_get_station(tileElement);
    const TileCoordsXYZD entrance = ride_get_entrance_location(ride, entranceId);
    const TileCoordsXYZD exit = ride_get_exit_location(ride, entranceId);

    return ((entrance.x != entranceX || entrance.y != entranceY) &&
            (exit.x != entranceX || exit.y != entranceY));
}

void track_paint_util_paint_floor(paint_session * session, uint8_t edges, uint32_t colourFlags, uint16_t height,
                                  const uint32_t floorSprites[4])
{
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

    sub_98197C(session, imageId | colourFlags, 0, 0, 32, 32, 1, height, 0, 0, height);
}

void track_paint_util_paint_fences(
    paint_session *          session,
    uint8_t                    edges,
    LocationXY16             position,
    const rct_tile_element * tileElement,
    Ride *                   ride,
    uint32_t                   colourFlags,
    uint16_t                   height,
    const uint32_t             fenceSprites[4],
    uint8_t                    rotation)
{
    uint32_t imageId;

    if (edges & EDGE_NW && track_paint_util_has_fence(EDGE_NW, position, tileElement, ride, rotation))
    {
        imageId = fenceSprites[3] | colourFlags;
        sub_98199C(session, imageId, 0, 0, 32, 1, 7, height, 0, 2, height + 2);
    }
    if (edges & EDGE_NE && track_paint_util_has_fence(EDGE_NE, position, tileElement, ride, rotation))
    {
        imageId = fenceSprites[0] | colourFlags;
        sub_98199C(session, imageId, 0, 0, 1, 32, 7, height, 2, 0, height + 2);
    }
    if (edges & EDGE_SE && track_paint_util_has_fence(EDGE_SE, position, tileElement, ride, rotation))
    {
        imageId = fenceSprites[1] | colourFlags;
        sub_98197C(session, imageId, 0, 0, 32, 1, 7, height, 0, 30, height + 2);
    }
    if (edges & EDGE_SW && track_paint_util_has_fence(EDGE_SW, position, tileElement, ride, rotation))
    {
        imageId = fenceSprites[2] | colourFlags;
        sub_98197C(session, imageId, 0, 0, 1, 32, 7, height, 30, 0, height + 2);
    }
}

/* Supports are only placed every 2 tiles for flat pieces*/
bool track_paint_util_should_paint_supports(LocationXY16 position)
{
    if ((position.x & (1 << 5)) == (position.y & (1 << 5)))
        return true;

    if ((!(position.x & (1 << 5))) && (!(position.y & (1 << 5))))
        return true;

    return false;
}

static void track_paint_util_draw_station_impl(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    direction,
    uint16_t                   height,
    uint16_t                   coverHeight,
    const rct_tile_element * tileElement,
    int32_t                   fenceOffsetA,
    int32_t                   fenceOffsetB);

void track_paint_util_draw_station(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    direction,
    uint16_t                   height,
    const rct_tile_element * tileElement)
{
    track_paint_util_draw_station_impl(session, rideIndex, direction, height, height, tileElement, 5, 7);
}

void track_paint_util_draw_station_2(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    direction,
    uint16_t                   height,
    const rct_tile_element * tileElement,
    int32_t                   fenceOffsetA,
    int32_t                   fenceOffsetB)
{
    track_paint_util_draw_station_impl(session, rideIndex, direction, height, height, tileElement, fenceOffsetA, fenceOffsetB);
}

void track_paint_util_draw_station_3(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    direction,
    uint16_t                   height,
    uint16_t                   coverHeight,
    const rct_tile_element * tileElement)
{
    track_paint_util_draw_station_impl(session, rideIndex, direction, height, coverHeight, tileElement, 5, 7);
}

static void track_paint_util_draw_station_impl(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    direction,
    uint16_t                   height,
    uint16_t                   coverHeight,
    const rct_tile_element * tileElement,
    int32_t                   fenceOffsetA,
    int32_t                   fenceOffsetB)
{
    LocationXY16                         position      = session->MapPosition;
    Ride *                               ride          = get_ride(rideIndex);
    const rct_ride_entrance_definition * entranceStyle = &RideEntranceDefinitions[ride->entrance_style];
    const bool                           hasGreenLight = tile_element_get_green_light(tileElement);

    bool   hasFence;
    uint32_t imageId;

    if (direction == 0 || direction == 2)
    {
        // height += 5 (height + 5);
        hasFence = track_paint_util_has_fence(EDGE_NW, position, tileElement, ride, session->CurrentRotation);

        if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 0)
        {
            if (hasGreenLight)
            {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_SW_NE
                                    : SPR_STATION_PLATFORM_END_GREEN_LIGHT_SW_NE) |
                          session->TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId =
                    (hasFence ? SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_SW_NE : SPR_STATION_PLATFORM_END_RED_LIGHT_SW_NE) |
                    session->TrackColours[SCHEME_SUPPORTS];
            }
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 2)
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_SW_NE : SPR_STATION_PLATFORM_BEGIN_SW_NE) |
                      session->TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_SW_NE : SPR_STATION_PLATFORM_SW_NE) |
                      session->TrackColours[SCHEME_SUPPORTS];
        }
        sub_98196C(session, imageId, 0, 0, 32, 8, 1, height + fenceOffsetA);
        // height -= 5 (height)
        track_paint_util_draw_station_covers(session, EDGE_NW, hasFence, entranceStyle, coverHeight);
        // height += 5 (height + 5)

        if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 0)
        {
            imageId = (hasGreenLight ? SPR_STATION_PLATFORM_END_GREEN_LIGHT_SW_NE : SPR_STATION_PLATFORM_END_RED_LIGHT_SW_NE) |
                      session->TrackColours[SCHEME_SUPPORTS];
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 2)
        {
            imageId = SPR_STATION_PLATFORM_BEGIN_SW_NE | session->TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = SPR_STATION_PLATFORM_SW_NE | session->TrackColours[SCHEME_SUPPORTS];
        }
        sub_98196C(session, imageId, 0, 24, 32, 8, 1, height + fenceOffsetA);
        // height += 2 (height + 7)

        hasFence = track_paint_util_has_fence(EDGE_SE, position, tileElement, ride, session->CurrentRotation);
        if (hasFence)
        {
            if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 0)
            {
                imageId = SPR_STATION_BEGIN_ANGLE_FENCE_SW_NE | session->TrackColours[SCHEME_SUPPORTS];
            }
            else if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 2)
            {
                imageId = SPR_STATION_LIGHT_BACK_ANGLE_FENCED_NE_SW | session->TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId = SPR_STATION_FENCE_SW_NE | session->TrackColours[SCHEME_SUPPORTS];
            }
            sub_98196C(session, imageId, 0, 31, 32, 1, 7, height + fenceOffsetB);
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 0)
        {
            // Addition: draw only small fence if there is an entrance/exit at the beginning
            imageId = SPR_STATION_FENCE_SMALL_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 31, 23, 1, 8, 7, height + fenceOffsetB);
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 2)
        {
            // Addition: draw only small fence if there is an entrance/exit at the end
            imageId = SPR_STATION_LIGHT_BACK_NE_SW | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 31, 23, 1, 8, 7, height + fenceOffsetB);
        }
        // height -= 7 (height)
        track_paint_util_draw_station_covers(session, EDGE_SE, hasFence, entranceStyle, coverHeight);
        // height += 7 (height + 7)

        if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 0)
        {
            imageId = SPR_STATION_FENCE_SMALL_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 31, 0, 1, 8, 7, height + fenceOffsetB);
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 2)
        {
            imageId = SPR_STATION_LIGHT_BACK_NE_SW | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 31, 0, 1, 8, 7, height + fenceOffsetB);
        }
    }
    else if (direction == 1 || direction == 3)
    {
        // height += 5 (height + 5);
        hasFence = track_paint_util_has_fence(EDGE_NE, position, tileElement, ride, session->CurrentRotation);

        if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 3)
        {
            if (hasGreenLight)
            {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_NW_SE
                                    : SPR_STATION_PLATFORM_END_GREEN_LIGHT_NW_SE) |
                          session->TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId =
                    (hasFence ? SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_NW_SE : SPR_STATION_PLATFORM_END_RED_LIGHT_NW_SE) |
                    session->TrackColours[SCHEME_SUPPORTS];
            }
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 1)
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_NW_SE : SPR_STATION_PLATFORM_BEGIN_NW_SE) |
                      session->TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_NW_SE : SPR_STATION_PLATFORM_NW_SE) |
                      session->TrackColours[SCHEME_SUPPORTS];
        }
        sub_98196C(session, imageId, 0, 0, 8, 32, 1, height + fenceOffsetA);
        // height -= 5 (height)
        track_paint_util_draw_station_covers(session, EDGE_NE, hasFence, entranceStyle, coverHeight);
        // height += 5 (height + 5)

        if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 3)
        {
            imageId = (hasGreenLight ? SPR_STATION_PLATFORM_END_GREEN_LIGHT_NW_SE : SPR_STATION_PLATFORM_END_RED_LIGHT_NW_SE) |
                      session->TrackColours[SCHEME_SUPPORTS];
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 1)
        {
            imageId = SPR_STATION_PLATFORM_BEGIN_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = SPR_STATION_PLATFORM_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
        }
        sub_98196C(session, imageId, 24, 0, 8, 32, 1, height + fenceOffsetA);
        // height += 2 (height + 7)

        hasFence = track_paint_util_has_fence(EDGE_SW, position, tileElement, ride, session->CurrentRotation);
        if (hasFence)
        {
            if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 3)
            {
                imageId = SPR_STATION_BEGIN_ANGLE_FENCE_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            }
            else if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 1)
            {
                imageId = SPR_STATION_LIGHT_BACK_ANGLE_FENCED_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId = SPR_STATION_FENCE_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            }
            sub_98196C(session, imageId, 31, 0, 1, 32, 7, height + fenceOffsetB);
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 3)
        {
            // Addition: draw only small fence if there is an entrance/exit at the beginning
            imageId = SPR_STATION_FENCE_SMALL_SW_NE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 23, 31, 8, 1, 7, height + fenceOffsetB);
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 1)
        {
            // Addition: draw only small fence if there is an entrance/exit at the end
            imageId = SPR_STATION_LIGHT_BACK_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 23, 31, 8, 1, 7, height + fenceOffsetB);
        }

        // height -= 7 (height)
        track_paint_util_draw_station_covers(session, EDGE_SW, hasFence, entranceStyle, coverHeight);
        // height += 7 (height + 7)

        if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 3)
        {
            imageId = SPR_STATION_FENCE_SMALL_SW_NE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 0, 31, 8, 1, 7, height + fenceOffsetB);
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 1)
        {
            imageId = SPR_STATION_LIGHT_BACK_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 0, 31, 8, 1, 7, height + fenceOffsetB);
        }
    }
}

void track_paint_util_draw_station_inverted(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement,
    uint8_t                    stationVariant)
{
    LocationXY16                         position      = session->MapPosition;
    Ride *                               ride          = get_ride(rideIndex);
    const rct_ride_entrance_definition * entranceStyle = &RideEntranceDefinitions[ride->entrance_style];
    const bool                           hasGreenLight = tile_element_get_green_light(tileElement);

    bool   hasFence;
    uint32_t imageId;

    if (direction == 0 || direction == 2)
    {
        // height += 5 (height + 5);
        hasFence = track_paint_util_has_fence(EDGE_NW, position, tileElement, ride, session->CurrentRotation);

        if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 0)
        {
            if (hasGreenLight)
            {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_SW_NE
                                    : SPR_STATION_PLATFORM_END_GREEN_LIGHT_SW_NE) |
                          session->TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId =
                    (hasFence ? SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_SW_NE : SPR_STATION_PLATFORM_END_RED_LIGHT_SW_NE) |
                    session->TrackColours[SCHEME_SUPPORTS];
            }
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 2)
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_SW_NE : SPR_STATION_PLATFORM_BEGIN_SW_NE) |
                      session->TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_SW_NE : SPR_STATION_PLATFORM_SW_NE) |
                      session->TrackColours[SCHEME_SUPPORTS];
        }
        sub_98196C(session, imageId, 0, 0, 32, 8, 1, height + 6);
        // height -= 5 (height)
        track_paint_util_draw_station_covers_2(session, EDGE_NW, hasFence, entranceStyle, height, stationVariant);
        // height += 5 (height + 5)

        if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 0)
        {
            imageId = (hasGreenLight ? SPR_STATION_PLATFORM_END_GREEN_LIGHT_SW_NE : SPR_STATION_PLATFORM_END_RED_LIGHT_SW_NE) |
                      session->TrackColours[SCHEME_SUPPORTS];
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 2)
        {
            imageId = SPR_STATION_PLATFORM_BEGIN_SW_NE | session->TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = SPR_STATION_PLATFORM_SW_NE | session->TrackColours[SCHEME_SUPPORTS];
        }
        sub_98196C(session, imageId, 0, 24, 32, 8, 1, height + 6);
        // height += 2 (height + 7)

        hasFence = track_paint_util_has_fence(EDGE_SE, position, tileElement, ride, session->CurrentRotation);
        if (hasFence)
        {
            if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 0)
            {
                imageId = SPR_STATION_INVERTED_BEGIN_ANGLE_FENCE_SW_NE | session->TrackColours[SCHEME_SUPPORTS];
            }
            else if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 2)
            {
                imageId = SPR_STATION_INVERTED_LIGHT_BACK_ANGLE_FENCED_NE_SW | session->TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId = SPR_STATION_INVERTED_FENCE_SW_NE | session->TrackColours[SCHEME_SUPPORTS];
            }
            sub_98196C(session, imageId, 0, 31, 32, 1, 7, height + 8);
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 0)
        {
            // Addition: draw only small fence if there is an entrance/exit at the beginning
            imageId = SPR_STATION_FENCE_SMALL_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 31, 23, 1, 8, 7, height + 8);
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 2)
        {
            // Addition: draw only small fence if there is an entrance/exit at the end
            imageId = SPR_STATION_LIGHT_BACK_NE_SW | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 31, 23, 1, 8, 7, height + 8);
        }
        // height -= 7 (height)
        track_paint_util_draw_station_covers_2(session, EDGE_SE, hasFence, entranceStyle, height, stationVariant);
        // height += 7 (height + 7)

        if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 0)
        {
            imageId = SPR_STATION_FENCE_SMALL_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 31, 0, 1, 8, 7, height + 8);
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 2)
        {
            imageId = SPR_STATION_LIGHT_BACK_NE_SW | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 31, 0, 1, 8, 7, height + 8);
        }
    }
    else if (direction == 1 || direction == 3)
    {
        // height += 5 (height + 5);
        hasFence = track_paint_util_has_fence(EDGE_NE, position, tileElement, ride, session->CurrentRotation);

        if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 3)
        {
            if (hasGreenLight)
            {
                imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_END_GREEN_LIGHT_NW_SE
                                    : SPR_STATION_PLATFORM_END_GREEN_LIGHT_NW_SE) |
                          session->TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId =
                    (hasFence ? SPR_STATION_PLATFORM_FENCED_END_RED_LIGHT_NW_SE : SPR_STATION_PLATFORM_END_RED_LIGHT_NW_SE) |
                    session->TrackColours[SCHEME_SUPPORTS];
            }
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 1)
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_BEGIN_FENCED_NW_SE : SPR_STATION_PLATFORM_BEGIN_NW_SE) |
                      session->TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = (hasFence ? SPR_STATION_PLATFORM_FENCED_NW_SE : SPR_STATION_PLATFORM_NW_SE) |
                      session->TrackColours[SCHEME_SUPPORTS];
        }
        sub_98196C(session, imageId, 0, 0, 8, 32, 1, height + 6);
        // height -= 5 (height)
        track_paint_util_draw_station_covers_2(session, EDGE_NE, hasFence, entranceStyle, height, stationVariant);
        // height += 5 (height + 5)

        if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 3)
        {
            imageId = (hasGreenLight ? SPR_STATION_PLATFORM_END_GREEN_LIGHT_NW_SE : SPR_STATION_PLATFORM_END_RED_LIGHT_NW_SE) |
                      session->TrackColours[SCHEME_SUPPORTS];
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 1)
        {
            imageId = SPR_STATION_PLATFORM_BEGIN_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
        }
        else
        {
            imageId = SPR_STATION_PLATFORM_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
        }
        sub_98196C(session, imageId, 24, 0, 8, 32, 1, height + 6);
        // height += 2 (height + 7)

        hasFence = track_paint_util_has_fence(EDGE_SW, position, tileElement, ride, session->CurrentRotation);
        if (hasFence)
        {
            if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 3)
            {
                imageId = SPR_STATION_INVERTED_BEGIN_ANGLE_FENCE_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            }
            else if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 1)
            {
                imageId = SPR_STATION_INVERTED_LIGHT_BACK_ANGLE_FENCED_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            }
            else
            {
                imageId = SPR_STATION_INVERTED_FENCE_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            }
            sub_98196C(session, imageId, 31, 0, 1, 32, 7, height + 8);
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 3)
        {
            // Addition: draw only small fence if there is an entrance/exit at the beginning
            imageId = SPR_STATION_FENCE_SMALL_SW_NE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 23, 31, 8, 1, 7, height + 8);
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 1)
        {
            // Addition: draw only small fence if there is an entrance/exit at the end
            imageId = SPR_STATION_LIGHT_BACK_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 23, 31, 8, 1, 7, height + 8);
        }

        // height -= 7 (height)
        track_paint_util_draw_station_covers_2(session, EDGE_SW, hasFence, entranceStyle, height, stationVariant);
        // height += 7 (height + 7)

        if (track_element_get_type(tileElement) == TRACK_ELEM_BEGIN_STATION && direction == 3)
        {
            imageId = SPR_STATION_FENCE_SMALL_SW_NE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 0, 31, 8, 1, 7, height + 8);
        }
        else if (track_element_get_type(tileElement) == TRACK_ELEM_END_STATION && direction == 1)
        {
            imageId = SPR_STATION_LIGHT_BACK_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 0, 31, 8, 1, 7, height + 8);
        }
    }
}

bool track_paint_util_draw_station_covers(paint_session * session, enum edge_t edge, bool hasFence,
                                          const rct_ride_entrance_definition * entranceStyle, uint16_t height)
{
    return track_paint_util_draw_station_covers_2(session, edge, hasFence, entranceStyle, height, STATION_VARIANT_BASIC);
}

bool track_paint_util_draw_station_covers_2(paint_session * session, enum edge_t edge, bool hasFence,
                                            const rct_ride_entrance_definition * entranceStyle, uint16_t height,
                                            uint8_t stationVariant)
{
    if (!(session->Unk141E9DB & (G141E9DB_FLAG_1 | G141E9DB_FLAG_2)))
    {
        return false;
    }

    static constexpr const int16_t heights[][2] = {
        { 22, 0 },
        { 30, 0 },
        { 46, 0 },
    };

    uint32_t        imageId;
    uint32_t        baseImageId = entranceStyle->base_image_id;
    int32_t        imageOffset = 0;
    LocationXYZ16 offset, bounds = { 0, 0, 0 }, boundsOffset = { 0, 0, 0 };

    offset = LocationXYZ16{ 0, 0, static_cast<int16_t>(height) };
    switch (edge)
    {
    case EDGE_NE:
        bounds       = LocationXYZ16{ 1, 30, heights[stationVariant][0] };
        boundsOffset = LocationXYZ16{ 0, 1, static_cast<int16_t>(height + 1) };
        imageOffset  = hasFence ? SPR_STATION_COVER_OFFSET_SE_NW_BACK_1 : SPR_STATION_COVER_OFFSET_SE_NW_BACK_0;
        break;
    case EDGE_SE:
        bounds       = LocationXYZ16{ 32, 32, 0 };
        boundsOffset = LocationXYZ16{ 0, 0, static_cast<int16_t>(height + 1 + heights[stationVariant][0]) };
        imageOffset  = SPR_STATION_COVER_OFFSET_NE_SW_FRONT;
        break;
    case EDGE_SW:
        bounds       = LocationXYZ16{ 32, 32, 0 };
        boundsOffset = LocationXYZ16{ 0, 0, static_cast<int16_t>(height + 1 + heights[stationVariant][0]) };
        imageOffset  = SPR_STATION_COVER_OFFSET_SE_NW_FRONT;
        break;
    case EDGE_NW:
        bounds       = LocationXYZ16{ 30, 1, heights[stationVariant][0] };
        boundsOffset = LocationXYZ16{ 1, 0, static_cast<int16_t>(height + 1) };
        imageOffset  = hasFence ? SPR_STATION_COVER_OFFSET_NE_SW_BACK_1 : SPR_STATION_COVER_OFFSET_NE_SW_BACK_0;
        break;
    }

    if (session->TrackColours[SCHEME_MISC] != IMAGE_TYPE_REMAP)
    {
        baseImageId &= 0x7FFFF;
    }

    if (baseImageId <= 0x20)
    {
        return false;
    }

    if (stationVariant == STATION_VARIANT_TALL)
    {
        imageOffset += SPR_STATION_COVER_OFFSET_TALL;
    }

    if (baseImageId & IMAGE_TYPE_TRANSPARENT)
    {
        imageId = (baseImageId & ~IMAGE_TYPE_TRANSPARENT) + imageOffset;
        sub_98197C(
            session, imageId, (int8_t)offset.x, (int8_t)offset.y, bounds.x, bounds.y, (int8_t)bounds.z, offset.z, boundsOffset.x,
            boundsOffset.y, boundsOffset.z);

        uint32_t edi = session->TrackColours[SCHEME_TRACK] & (0b11111 << 19);

        // weird jump
        imageId = (baseImageId | edi) + ((1 << 23) | (1 << 24) | (1 << 25)) + imageOffset + 12;
        sub_98199C(
            session, imageId, (int8_t)offset.x, (int8_t)offset.y, bounds.x, bounds.y, (int8_t)bounds.z, offset.z, boundsOffset.x,
            boundsOffset.y, boundsOffset.z);
        return true;
    }

    imageId = (baseImageId + imageOffset) | session->TrackColours[SCHEME_TRACK];
    sub_98197C(
        session, imageId, (int8_t)offset.x, (int8_t)offset.y, bounds.x, bounds.y, (int8_t)bounds.z, offset.z, boundsOffset.x,
        boundsOffset.y, boundsOffset.z);
    return true;
}

void track_paint_util_draw_station_platform(
    paint_session * session, Ride * ride, uint8_t direction, int32_t height, int32_t zOffset, const rct_tile_element * tileElement)
{
    LocationXY16                         position      = session->MapPosition;
    const rct_ride_entrance_definition * entranceStyle = &RideEntranceDefinitions[ride->entrance_style];
    if (direction & 1)
    {
        bool   hasFence = track_paint_util_has_fence(EDGE_NE, position, tileElement, ride, session->CurrentRotation);
        uint32_t imageId  = (hasFence ? SPR_STATION_NARROW_EDGE_FENCED_NE : SPR_STATION_NARROW_EDGE_NE) |
                         session->TrackColours[SCHEME_SUPPORTS];
        sub_98196C(session, imageId, 0, 0, 8, 32, 1, height + zOffset);
        track_paint_util_draw_station_covers(session, EDGE_NE, hasFence, entranceStyle, height);

        imageId = SPR_STATION_NARROW_EDGE_SW | session->TrackColours[SCHEME_SUPPORTS];
        sub_98196C(session, imageId, 24, 0, 8, 32, 1, height + zOffset);

        hasFence = track_paint_util_has_fence(EDGE_SW, position, tileElement, ride, session->CurrentRotation);
        if (hasFence)
        {
            imageId = SPR_STATION_FENCE_NW_SE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 31, 0, 1, 32, 7, height + zOffset + 2);
        }
        track_paint_util_draw_station_covers(session, EDGE_SW, hasFence, entranceStyle, height);
    }
    else
    {
        bool   hasFence = track_paint_util_has_fence(EDGE_NW, position, tileElement, ride, session->CurrentRotation);
        uint32_t imageId  = (hasFence ? SPR_STATION_NARROW_EDGE_FENCED_NW : SPR_STATION_NARROW_EDGE_NW) |
                         session->TrackColours[SCHEME_SUPPORTS];
        sub_98196C(session, imageId, 0, 0, 32, 8, 1, height + zOffset);
        track_paint_util_draw_station_covers(session, EDGE_NW, hasFence, entranceStyle, height);

        imageId = SPR_STATION_NARROW_EDGE_SE | session->TrackColours[SCHEME_SUPPORTS];
        sub_98196C(session, imageId, 0, 24, 32, 8, 1, height + zOffset);

        hasFence = track_paint_util_has_fence(EDGE_SE, position, tileElement, ride, session->CurrentRotation);
        if (hasFence)
        {
            imageId = SPR_STATION_FENCE_SW_NE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 0, 31, 32, 1, 7, height + zOffset + 2);
        }
        track_paint_util_draw_station_covers(session, EDGE_SE, hasFence, entranceStyle, height);
    }
}

void track_paint_util_draw_pier(
    paint_session *                      session,
    Ride *                               ride,
    const rct_ride_entrance_definition * entranceStyle,
    LocationXY16                         position,
    uint8_t                                direction,
    int32_t                               height,
    const rct_tile_element *             tileElement,
    uint8_t                                rotation)
{
    bool   hasFence;
    uint32_t imageId;

    if (direction & 1)
    {
        hasFence = track_paint_util_has_fence(EDGE_NE, position, tileElement, ride, session->CurrentRotation);
        imageId =
            (hasFence ? SPR_STATION_PIER_EDGE_NE_FENCED : SPR_STATION_PIER_EDGE_NE) | session->TrackColours[SCHEME_SUPPORTS];
        sub_98197C(session, imageId, 0, 0, 6, 32, 1, height, 2, 0, height);
        track_paint_util_draw_station_covers(session, EDGE_NE, hasFence, entranceStyle, height);

        imageId = SPR_STATION_PIER_EDGE_SW | session->TrackColours[SCHEME_SUPPORTS];
        sub_98196C(session, imageId, 24, 0, 8, 32, 1, height);

        hasFence = track_paint_util_has_fence(EDGE_SW, position, tileElement, ride, session->CurrentRotation);
        if (hasFence)
        {
            imageId = SPR_STATION_PIER_FENCE_SW | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 31, 0, 1, 32, 7, height + 2);
        }
        track_paint_util_draw_station_covers(session, EDGE_SW, hasFence, entranceStyle, height);
    }
    else
    {
        hasFence = track_paint_util_has_fence(EDGE_NW, position, tileElement, ride, rotation);
        imageId =
            (hasFence ? SPR_STATION_PIER_EDGE_NW_FENCED : SPR_STATION_PIER_EDGE_NW) | session->TrackColours[SCHEME_SUPPORTS];
        sub_98197C(session, imageId, 0, 0, 32, 6, 1, height, 0, 2, height);
        track_paint_util_draw_station_covers(session, EDGE_NW, hasFence, entranceStyle, height);

        imageId = SPR_STATION_PIER_EDGE_SE | session->TrackColours[SCHEME_SUPPORTS];
        sub_98196C(session, imageId, 0, 24, 32, 8, 1, height);

        hasFence = track_paint_util_has_fence(EDGE_SE, position, tileElement, ride, rotation);
        if (hasFence)
        {
            imageId = SPR_STATION_PIER_FENCE_SE | session->TrackColours[SCHEME_SUPPORTS];
            sub_98196C(session, imageId, 0, 31, 32, 1, 7, height + 2);
        }
        track_paint_util_draw_station_covers(session, EDGE_SE, hasFence, entranceStyle, height);
    }
}

void track_paint_util_draw_station_metal_supports(paint_session * session, uint8_t direction, uint16_t height, uint32_t colour)
{
    track_paint_util_draw_station_metal_supports_2(session, direction, height, colour, 3);
}

void track_paint_util_draw_station_metal_supports_2(paint_session * session, uint8_t direction, uint16_t height, uint32_t colour,
                                                    uint8_t type)
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

const LocationXY16 defaultRightHelixUpSmallQuarterBoundLengths[4][3][2] = {
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

const LocationXYZ16 defaultRightHelixUpSmallQuarterBoundOffsets[4][3][2] = {
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

static constexpr const int8_t right_helix_up_small_quarter_tiles_sprite_map[] = { 0, -1, 1, 2 };

void track_paint_util_right_helix_up_small_quarter_tiles_paint(paint_session * session, const int8_t thickness[2], int16_t height,
                                                               int32_t direction, uint8_t trackSequence, uint32_t colourFlags,
                                                               const uint32_t        sprites[4][3][2],
                                                               const LocationXY16  offsets[4][3][2],
                                                               const LocationXY16  boundsLengths[4][3][2],
                                                               const LocationXYZ16 boundsOffsets[4][3][2])
{
    int32_t index = right_helix_up_small_quarter_tiles_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    if (sprites[direction][index][0] != 0)
    {
        uint32_t        imageId      = sprites[direction][index][0] | colourFlags;
        LocationXY16  offset       = (offsets == nullptr ? LocationXY16{ 0, 0 } : offsets[direction][index][0]);
        LocationXY16  boundsLength = boundsLengths[direction][index][0];
        LocationXYZ16 boundsOffset = (boundsOffsets == nullptr ? LocationXYZ16{ offset.x, offset.y, 0 }
                                                            : boundsOffsets[direction][index][0]);

        sub_98197C(
            session, imageId, (int8_t)offset.x, (int8_t)offset.y, boundsLength.x, boundsLength.y, thickness[0], height,
            boundsOffset.x, boundsOffset.y, height + boundsOffset.z);
    }
    if (sprites[direction][index][1] != 0)
    {
        uint32_t        imageId      = sprites[direction][index][1] | colourFlags;
        LocationXY16  offset       = (offsets == nullptr ? LocationXY16{ 0, 0 } : offsets[direction][index][1]);
        LocationXY16  boundsLength = boundsLengths[direction][index][1];
        LocationXYZ16 boundsOffset = (boundsOffsets == nullptr ? LocationXYZ16{ offset.x, offset.y, 0 }
                                                            : boundsOffsets[direction][index][1]);

        sub_98197C(
            session, imageId, (int8_t)offset.x, (int8_t)offset.y, boundsLength.x, boundsLength.y, thickness[1], height,
            boundsOffset.x, boundsOffset.y, height + boundsOffset.z);
    }
}

const LocationXYZ16 defaultRightHelixUpLargeQuarterBoundOffsets[4][5][2] = {
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

const LocationXY16 defaultRightHelixUpLargeQuarterBoundLengths[4][5][2] = {
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

static constexpr const int8_t right_helix_up_large_quarter_sprite_map[] = { 0, -1, 1, 2, -1, 3, 4 };
void track_paint_util_right_helix_up_large_quarter_tiles_paint(paint_session * session, const int8_t thickness[2], int16_t height,
                                                               int32_t direction, uint8_t trackSequence, uint32_t colourFlags,
                                                               const uint32_t        sprites[4][5][2],
                                                               const LocationXY16  offsets[4][5][2],
                                                               const LocationXY16  boundsLengths[4][5][2],
                                                               const LocationXYZ16 boundsOffsets[4][5][2])
{
    int32_t index = right_helix_up_large_quarter_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    if (sprites[direction][index][0] != 0)
    {
        uint32_t        imageId      = sprites[direction][index][0] | colourFlags;
        LocationXY16  offset       = (offsets == nullptr ? LocationXY16{ 0, 0 } : offsets[direction][index][0]);
        LocationXY16  boundsLength = boundsLengths[direction][index][0];
        LocationXYZ16 boundsOffset = (boundsOffsets == nullptr ? LocationXYZ16{ offset.x, offset.y, 0 }
                                                            : boundsOffsets[direction][index][0]);

        sub_98197C(
            session, imageId, (int8_t)offset.x, (int8_t)offset.y, boundsLength.x, boundsLength.y, thickness[0], height,
            boundsOffset.x, boundsOffset.y, height + boundsOffset.z);
    }
    if (sprites[direction][index][1] != 0)
    {
        uint32_t        imageId      = sprites[direction][index][1] | colourFlags;
        LocationXY16  offset       = (offsets == nullptr ? LocationXY16{ 0, 0 } : offsets[direction][index][1]);
        LocationXY16  boundsLength = boundsLengths[direction][index][1];
        LocationXYZ16 boundsOffset = (boundsOffsets == nullptr ? LocationXYZ16{ offset.x, offset.y, 0 }
                                                            : boundsOffsets[direction][index][1]);

        sub_98197C(
            session, imageId, (int8_t)offset.x, (int8_t)offset.y, boundsLength.x, boundsLength.y, thickness[1], height,
            boundsOffset.x, boundsOffset.y, height + boundsOffset.z);
    }
}

const LocationXY16 defaultLeftEighthToDiagBoundLengths[4][4] = {
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

const LocationXYZ16 defaultLeftEighthToDiagBoundOffsets[4][4] = {
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

const LocationXY16 defaultRightEighthToDiagBoundLengths[4][4] = {
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

const LocationXYZ16 defaultRightEighthToDiagBoundOffsets[4][4] = {
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

const uint8_t mapLeftEighthTurnToOrthogonal[] = { 4, 2, 3, 1, 0 };

static constexpr const int8_t eighth_to_diag_sprite_map[] = { 0, 1, 2, -1, 3 };
void track_paint_util_eighth_to_diag_tiles_paint(paint_session * session, const int8_t thickness[4][4], int16_t height,
                                                 int32_t direction, uint8_t trackSequence, uint32_t colourFlags,
                                                 const uint32_t sprites[4][4], const LocationXY16 offsets[4][4],
                                                 const LocationXY16  boundsLengths[4][4],
                                                 const LocationXYZ16 boundsOffsets[4][4])
{
    int32_t index = eighth_to_diag_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    uint32_t        imageId      = sprites[direction][index] | colourFlags;
    LocationXY16  offset       = (offsets == nullptr ? LocationXY16{ 0, 0 } : offsets[direction][index]);
    LocationXY16  boundsLength = boundsLengths[direction][index];
    LocationXYZ16 boundsOffset =
        (boundsOffsets == nullptr ? LocationXYZ16{ offset.x, offset.y, 0 } : boundsOffsets[direction][index]);

    sub_98197C(
        session, imageId, (int8_t)offset.x, (int8_t)offset.y, boundsLength.x, boundsLength.y, thickness[direction][index], height,
        boundsOffset.x, boundsOffset.y, height + boundsOffset.z);
}

const LocationXY16 defaultDiagTileOffsets[4] = {
    { -16, -16 },
    { -16, -16 },
    { -16, -16 },
    { -16, -16 },
};

const LocationXY16 defaultDiagBoundLengths[4] = {
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

void track_paint_util_diag_tiles_paint(paint_session * session, int8_t thickness, int16_t height, int32_t direction,
                                       uint8_t trackSequence, uint32_t colourFlags, const uint32_t sprites[4],
                                       const LocationXY16 offsets[4], const LocationXY16 boundsLengths[4],
                                       const LocationXYZ16 boundsOffsets[4])
{
    int32_t index = diag_sprite_map[direction][trackSequence];
    if (index < 0)
    {
        return;
    }

    uint32_t        imageId      = sprites[direction] | colourFlags;
    LocationXY16  offset       = (offsets == nullptr ? LocationXY16{ 0, 0 } : offsets[direction]);
    LocationXY16  boundsLength = boundsLengths[direction];
    LocationXYZ16 boundsOffset =
        (boundsOffsets == nullptr ? LocationXYZ16{ offset.x, offset.y, 0 } : boundsOffsets[direction]);

    sub_98197C(
        session, imageId, (int8_t)offset.x, (int8_t)offset.y, boundsLength.x, boundsLength.y, thickness, height, boundsOffset.x,
        boundsOffset.y, height + boundsOffset.z);
}

const uint8_t mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[] = { 6, 4, 5, 3, 1, 2, 0 };

const LocationXY16 defaultRightQuarterTurn5TilesOffsets[4][5] = {
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

const LocationXYZ16 defaultRightQuarterTurn5TilesBoundOffsets[4][5] = {
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

const LocationXY16 defaultRightQuarterTurn5TilesBoundLengths[4][5] = {
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

static constexpr const int8_t right_quarter_turn_5_tiles_sprite_map[] = { 0, -1, 1, 2, -1, 3, 4 };
void               track_paint_util_right_quarter_turn_5_tiles_paint(paint_session * session, int8_t thickness, int16_t height,
                                                                     int32_t direction, uint8_t trackSequence, uint32_t colourFlags,
                                                                     const uint32_t sprites[4][5], const LocationXY16 offsets[4][5],
                                                                     const LocationXY16  boundsLengths[4][5],
                                                                     const LocationXYZ16 boundsOffsets[4][5])
{
    int32_t index = right_quarter_turn_5_tiles_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    uint32_t        imageId      = sprites[direction][index] | colourFlags;
    LocationXY16  offset       = (offsets == nullptr ? LocationXY16{ 0, 0 } : offsets[direction][index]);
    LocationXY16  boundsLength = boundsLengths[direction][index];
    LocationXYZ16 boundsOffset =
        (boundsOffsets == nullptr ? LocationXYZ16{ offset.x, offset.y, 0 } : boundsOffsets[direction][index]);

    sub_98197C(
        session, imageId, (int8_t)offset.x, (int8_t)offset.y, boundsLength.x, boundsLength.y, thickness, height, boundsOffset.x,
        boundsOffset.y, height + boundsOffset.z);
}

void track_paint_util_right_quarter_turn_5_tiles_paint_2(paint_session * session, int16_t height, int32_t direction,
                                                         uint8_t trackSequence, uint32_t colourFlags, const sprite_bb sprites[][5])
{
    int8_t sprite = right_quarter_turn_5_tiles_sprite_map[trackSequence];
    if (sprite < 0)
    {
        return;
    }

    const sprite_bb * spriteBB = &sprites[direction][sprite];
    uint32_t            imageId  = spriteBB->sprite_id | colourFlags;
    sub_98197C(
        session, imageId, (int8_t)spriteBB->offset.x, (int8_t)spriteBB->offset.y, spriteBB->bb_size.x, spriteBB->bb_size.y,
        (int8_t)spriteBB->bb_size.z, height + spriteBB->offset.z, spriteBB->bb_offset.x, spriteBB->bb_offset.y,
        height + spriteBB->bb_offset.z);
}

void track_paint_util_right_quarter_turn_5_tiles_paint_3(paint_session * session, int16_t height, int32_t direction,
                                                         uint8_t trackSequence, uint32_t colourFlags, const sprite_bb sprites[][5])
{
    int8_t sprite = right_quarter_turn_5_tiles_sprite_map[trackSequence];
    if (sprite < 0)
    {
        return;
    }

    const sprite_bb * spriteBB = &sprites[direction][sprite];
    uint32_t            imageId  = spriteBB->sprite_id | colourFlags;
    sub_98196C(
        session, imageId, (int8_t)spriteBB->offset.x, (int8_t)spriteBB->offset.y, spriteBB->bb_size.x, spriteBB->bb_size.y,
        (int8_t)spriteBB->bb_size.z, height + spriteBB->offset.z);
}

void track_paint_util_right_quarter_turn_5_tiles_tunnel(paint_session * session, int16_t height, uint8_t direction,
                                                        uint8_t trackSequence, uint8_t tunnelType)
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

void track_paint_util_right_quarter_turn_5_tiles_wooden_supports(paint_session * session, int16_t height, uint8_t direction,
                                                                 uint8_t trackSequence)
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
        wooden_a_supports_paint_setup(session, supportType, 0, height, session->TrackColours[SCHEME_SUPPORTS], NULL);
    }
}

const uint8_t mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[] = { 3, 1, 2, 0 };

const LocationXY16 defaultRightQuarterTurn3TilesOffsets[4][3] = {
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

const LocationXYZ16 defaultRightQuarterTurn3TilesBoundOffsets[4][3] = {
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

const LocationXY16 defaultRightQuarterTurn3TilesBoundLengths[4][3] = {
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

static constexpr const int8_t right_quarter_turn_3_tiles_sprite_map[] = { 0, -1, 1, 2 };
void               track_paint_util_right_quarter_turn_3_tiles_paint(paint_session * session, int8_t thickness, int16_t height,
                                                                     int32_t direction, uint8_t trackSequence, uint32_t colourFlags,
                                                                     const uint32_t sprites[4][3], const LocationXY16 offsets[4][3],
                                                                     const LocationXY16  boundsLengths[4][3],
                                                                     const LocationXYZ16 boundsOffsets[4][3])
{
    int32_t index = right_quarter_turn_3_tiles_sprite_map[trackSequence];
    if (index < 0)
    {
        return;
    }

    uint32_t        imageId      = sprites[direction][index] | colourFlags;
    LocationXY16  offset       = (offsets == nullptr ? LocationXY16{ 0, 0 } : offsets[direction][index]);
    LocationXY16  boundsLength = boundsLengths[direction][index];
    LocationXYZ16 boundsOffset =
        (boundsOffsets == nullptr ? LocationXYZ16{ offset.x, offset.y, 0 } : boundsOffsets[direction][index]);

    sub_98197C(
        session, imageId, (int8_t)offset.x, (int8_t)offset.y, boundsLength.x, boundsLength.y, thickness, height, boundsOffset.x,
        boundsOffset.y, height + boundsOffset.z);
}

void track_paint_util_right_quarter_turn_3_tiles_paint_2(paint_session * session, int8_t thickness, int16_t height,
                                                         int32_t direction, uint8_t trackSequence, uint32_t colourFlags,
                                                         const uint32_t sprites[4][3])
{
    track_paint_util_right_quarter_turn_3_tiles_paint_2_with_height_offset(session, thickness, height, direction, trackSequence,
                                                                           colourFlags, sprites, 0);
}

void track_paint_util_right_quarter_turn_3_tiles_paint_2_with_height_offset(paint_session * session, int8_t thickness,
                                                                            int16_t height, int32_t direction,
                                                                            uint8_t trackSequence, uint32_t colourFlags,
                                                                            const uint32_t sprites[4][3],
                                                                            int32_t heightOffset)
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
            sub_98197C(session, imageId, 0, 0, 32, 20, thickness, height, 0, 6, height + heightOffset);
            break;
        case 2:
            sub_98197C(session, imageId, 0, 0, 16, 16, thickness, height, 16, 16, height + heightOffset);
            break;
        case 3:
            sub_98197C(session, imageId, 0, 0, 20, 32, thickness, height, 6, 0, height + heightOffset);
            break;
        }
        break;

    case 1:
        switch (trackSequence)
        {
        case 0:
            sub_98197C(session, imageId, 0, 0, 20, 32, thickness, height, 6, 0, height + heightOffset);
            break;
        case 2:
            sub_98197C(session, imageId, 0, 0, 16, 16, thickness, height, 16, 0, height + heightOffset);
            break;
        case 3:
            sub_98197C(session, imageId, 0, 0, 32, 20, thickness, height, 0, 6, height + heightOffset);
            break;
        }
        break;

    case 2:
        switch (trackSequence)
        {
        case 0:
            sub_98197C(session, imageId, 0, 0, 32, 20, thickness, height, 0, 6, height + heightOffset);
            break;
        case 2:
            sub_98197C(session, imageId, 0, 0, 16, 16, thickness, height, 0, 0, height + heightOffset);
            break;
        case 3:
            sub_98197C(session, imageId, 0, 0, 20, 32, thickness, height, 6, 0, height + heightOffset);
            break;
        }
        break;

    case 3:
        switch (trackSequence)
        {
        case 0:
            sub_98197C(session, imageId, 0, 0, 20, 32, thickness, height, 6, 0, height + heightOffset);
            break;
        case 2:
            sub_98197C(session, imageId, 0, 0, 16, 16, thickness, height, 0, 16, height + heightOffset);
            break;
        case 3:
            sub_98197C(session, imageId, 0, 0, 32, 20, thickness, height, 0, 6, height + heightOffset);
            break;
        }
        break;
    }
}

void track_paint_util_right_quarter_turn_3_tiles_paint_3(paint_session * session, int16_t height, int32_t direction,
                                                         uint8_t trackSequence, uint32_t colourFlags,
                                                         const sprite_bb sprites[4][3])
{
    int8_t sprite = right_quarter_turn_3_tiles_sprite_map[trackSequence];
    if (sprite < 0)
    {
        return;
    }
    const sprite_bb * spriteBB = &sprites[direction][sprite];
    sub_98197C(
        session, spriteBB->sprite_id | colourFlags, (int8_t)spriteBB->offset.x, (int8_t)spriteBB->offset.y, spriteBB->bb_size.x,
        spriteBB->bb_size.y, (int8_t)spriteBB->bb_size.z, spriteBB->offset.z + height, spriteBB->bb_offset.x,
        spriteBB->bb_offset.y, height + spriteBB->bb_offset.z);
}

void track_paint_util_right_quarter_turn_3_tiles_paint_4(paint_session * session, int16_t height, int32_t direction,
    uint8_t trackSequence, uint32_t colourFlags,
    const sprite_bb sprites[4][3])
{
    int8_t sprite = right_quarter_turn_3_tiles_sprite_map[trackSequence];
    if (sprite < 0)
    {
        return;
    }

    const sprite_bb * spriteBB = &sprites[direction][sprite];
    uint32_t            imageId  = spriteBB->sprite_id | colourFlags;
    sub_98196C(
        session, imageId, (int8_t)spriteBB->offset.x, (int8_t)spriteBB->offset.y, spriteBB->bb_size.x, spriteBB->bb_size.y,
        (int8_t)spriteBB->bb_size.z, height + spriteBB->offset.z);
}

void track_paint_util_right_quarter_turn_3_tiles_tunnel(paint_session * session, int16_t height, uint8_t direction,
                                                        uint8_t trackSequence, uint8_t tunnelType)
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

void track_paint_util_right_quarter_turn_3_tiles_25_deg_up_tunnel(paint_session * session, int16_t height, uint8_t direction,
                                                                  uint8_t trackSequence, uint8_t tunnelType0, uint8_t tunnelType3)
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

void track_paint_util_right_quarter_turn_3_tiles_25_deg_down_tunnel(paint_session * session, int16_t height, uint8_t direction,
                                                                    uint8_t trackSequence, uint8_t tunnelType0, uint8_t tunnelType3)
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

static constexpr const int8_t left_quarter_turn_3_tiles_sprite_map[] = { 2, -1, 1, 0 };
void track_paint_util_left_quarter_turn_3_tiles_paint(paint_session * session, int8_t thickness, int16_t height, int32_t direction,
                                                      uint8_t trackSequence, uint32_t colourFlags, const uint32_t sprites[4][3])
{
    track_paint_util_left_quarter_turn_3_tiles_paint_with_height_offset(session, thickness, height, direction, trackSequence,
                                                                        colourFlags, sprites, 0);
}

void track_paint_util_left_quarter_turn_3_tiles_paint_with_height_offset(paint_session * session, int8_t thickness,
                                                                         int16_t height, int32_t direction, uint8_t trackSequence,
                                                                         uint32_t colourFlags, const uint32_t sprites[4][3],
                                                                         int32_t heightOffset)
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
            sub_98197C(session, imageId, 0, 0, 32, 20, thickness, height, 0, 6, height + heightOffset);
            break;
        case 2:
            sub_98197C(session, imageId, 0, 0, 16, 16, thickness, height, 16, 0, height + heightOffset);
            break;
        case 3:
            sub_98197C(session, imageId, 0, 0, 20, 32, thickness, height, 6, 0, height + heightOffset);
            break;
        }
        break;

    case 1:
        switch (trackSequence)
        {
        case 0:
            sub_98197C(session, imageId, 0, 0, 20, 32, thickness, height, 6, 0, height + heightOffset);
            break;
        case 2:
            sub_98197C(session, imageId, 0, 0, 16, 16, thickness, height, 0, 0, height + heightOffset);
            break;
        case 3:
            sub_98197C(session, imageId, 0, 0, 32, 20, thickness, height, 0, 6, height + heightOffset);
            break;
        }
        break;

    case 2:
        switch (trackSequence)
        {
        case 0:
            sub_98197C(session, imageId, 0, 0, 32, 20, thickness, height, 0, 6, height + heightOffset);
            break;
        case 2:
            sub_98197C(session, imageId, 0, 0, 16, 16, thickness, height, 0, 16, height + heightOffset);
            break;
        case 3:
            sub_98197C(session, imageId, 0, 0, 20, 32, thickness, height, 6, 0, height + heightOffset);
            break;
        }
        break;

    case 3:
        switch (trackSequence)
        {
        case 0:
            sub_98197C(session, imageId, 0, 0, 20, 32, thickness, height, 6, 0, height + heightOffset);
            break;
        case 2:
            sub_98197C(session, imageId, 0, 0, 16, 16, thickness, height, 16, 16, height + heightOffset);
            break;
        case 3:
            sub_98197C(session, imageId, 0, 0, 32, 20, thickness, height, 0, 6, height + heightOffset);
            break;
        }
        break;
    }
}

// void track_paint_util_left_quarter_turn_3_tiles_paint_2(int16_t height, int32_t direction, uint8_t rotation, uint8_t trackSequence,
// uint32_t colourFlags, const sprite_bb sprites[][5])
// {
//  int8_t sprite = right_quarter_turn_5_tiles_sprite_map[trackSequence];
//  if (sprite < 0) {
//      return;
//  }
//
//  const sprite_bb *spriteBB = &sprites[direction][sprite];
//  uint32_t imageId = spriteBB->SpriteId | colourFlags;
//  sub_98197C(session, imageId,
//              (int8_t)spriteBB->offset.x, (int8_t)spriteBB->offset.y,
//              spriteBB->bb_size.x, spriteBB->bb_size.y, (int8_t)spriteBB->bb_size.z,
//              height + spriteBB->offset.z,
//              spriteBB->bb_offset.x, spriteBB->bb_offset.y, height + spriteBB->bb_offset.z,
//              rotation);
// }

void track_paint_util_left_quarter_turn_3_tiles_tunnel(paint_session * session, int16_t height, uint8_t tunnelType,
                                                       uint8_t direction, uint8_t trackSequence)
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

void track_paint_util_left_quarter_turn_1_tile_paint(paint_session * session, int8_t thickness, int16_t height,
                                                     int16_t boundBoxZOffset, int32_t direction, uint32_t colourFlags,
                                                     const uint32_t * sprites)
{
    uint32_t imageId = sprites[direction] | colourFlags;

    switch (direction)
    {
    case 0:
        sub_98197C(session, imageId, 0, 0, 26, 24, thickness, height, 6, 2, height + boundBoxZOffset);
        break;
    case 1:
        sub_98197C(session, imageId, 0, 0, 26, 26, thickness, height, 0, 0, height + boundBoxZOffset);
        break;
    case 2:
        sub_98197C(session, imageId, 0, 0, 24, 26, thickness, height, 2, 6, height + boundBoxZOffset);
        break;
    case 3:
        sub_98197C(session, imageId, 0, 0, 24, 24, thickness, height, 6, 6, height + boundBoxZOffset);
        break;
    }
}

void track_paint_util_right_quarter_turn_1_tile_tunnel(paint_session * session, uint8_t direction, uint16_t baseHeight,
                                                       int8_t startOffset, uint8_t startTunnel, int8_t endOffset, uint8_t endTunnel)
{
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, (direction + 3) % 4, baseHeight, endOffset, endTunnel,
                                                     startOffset, startTunnel);
}

void track_paint_util_left_quarter_turn_1_tile_tunnel(paint_session * session, uint8_t direction, uint16_t baseHeight,
                                                      int8_t startOffset, uint8_t startTunnel, int8_t endOffset, uint8_t endTunnel)
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

void track_paint_util_spinning_tunnel_paint(paint_session * session, int8_t thickness, int16_t height, uint8_t direction)
{
    int32_t frame       = gScenarioTicks >> 2 & 3;
    uint32_t colourFlags = session->TrackColours[SCHEME_SUPPORTS];

    uint32_t colourFlags2 = session->TrackColours[SCHEME_TRACK];
    if (colourFlags2 & IMAGE_TYPE_REMAP_2_PLUS)
    {
        colourFlags |= colourFlags2 & (IMAGE_TYPE_REMAP_2_PLUS | (0x1F << 24));
    }

    uint32_t imageId = trackSpritesGhostTrainSpinningTunnel[direction & 1][0][frame] | colourFlags;
    if (direction == 0 || direction == 2)
    {
        sub_98199C(session, imageId, 0, 0, 28, 20, thickness, height, 2, 6, height);
    }
    else
    {
        sub_98199C(session, imageId, 0, 0, 20, 28, thickness, height, 6, 2, height);
    }

    imageId = trackSpritesGhostTrainSpinningTunnel[direction & 1][1][frame] | colourFlags;
    if (direction == 0 || direction == 2)
    {
        sub_98197C(session, imageId, 0, 0, 26, 1, 23, height, 4, 28, height);
    }
    else
    {
        sub_98197C(session, imageId, 0, 0, 1, 26, 23, height, 28, 4, height);
    }
}

void track_paint_util_onride_photo_small_paint(
    paint_session * session, uint8_t direction, int32_t height, const rct_tile_element * tileElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { SPR_ON_RIDE_PHOTO_SIGN_SMALL_SW_NE, SPR_ON_RIDE_PHOTO_CAMERA_SMALL_S, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_S },
        { SPR_ON_RIDE_PHOTO_SIGN_SMALL_NW_SE, SPR_ON_RIDE_PHOTO_CAMERA_SMALL_W, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_W },
        { SPR_ON_RIDE_PHOTO_SIGN_SMALL_NE_SW, SPR_ON_RIDE_PHOTO_CAMERA_SMALL_N, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_N },
        { SPR_ON_RIDE_PHOTO_SIGN_SMALL_SE_NW, SPR_ON_RIDE_PHOTO_CAMERA_SMALL_E, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_SMALL_E },
    };

    bool   takingPhoto  = tile_element_is_taking_photo(tileElement);
    uint32_t imageId      = imageIds[direction][0] | session->TrackColours[SCHEME_MISC];
    uint32_t flashImageId = imageIds[direction][takingPhoto ? 2 : 1] | session->TrackColours[SCHEME_MISC];
    switch (direction)
    {
    case 0:
        sub_98196C(session, imageId, 26, 0, 1, 1, 19, height);
        sub_98196C(session, imageId, 26, 31, 1, 1, 19, height);
        sub_98196C(session, flashImageId, 6, 0, 1, 1, 19, height);
        break;
    case 1:
        sub_98196C(session, imageId, 0, 6, 1, 1, 19, height);
        sub_98196C(session, imageId, 31, 6, 1, 1, 19, height);
        sub_98196C(session, flashImageId, 0, 26, 1, 1, 19, height);
        break;
    case 2:
        sub_98196C(session, imageId, 6, 0, 1, 1, 19, height);
        sub_98196C(session, imageId, 6, 31, 1, 1, 19, height);
        sub_98196C(session, flashImageId, 26, 31, 1, 1, 19, height);
        break;
    case 3:
        sub_98196C(session, imageId, 0, 26, 1, 1, 19, height);
        sub_98196C(session, imageId, 31, 26, 1, 1, 19, height);
        sub_98196C(session, flashImageId, 31, 6, 1, 1, 19, height);
        break;
    }
}

void track_paint_util_onride_photo_paint(
    paint_session * session, uint8_t direction, int32_t height, const rct_tile_element * tileElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { SPR_ON_RIDE_PHOTO_SIGN_SW_NE, SPR_ON_RIDE_PHOTO_CAMERA_S, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_S },
        { SPR_ON_RIDE_PHOTO_SIGN_NW_SE, SPR_ON_RIDE_PHOTO_CAMERA_W, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_W },
        { SPR_ON_RIDE_PHOTO_SIGN_NE_SW, SPR_ON_RIDE_PHOTO_CAMERA_N, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_N },
        { SPR_ON_RIDE_PHOTO_SIGN_SE_NW, SPR_ON_RIDE_PHOTO_CAMERA_E, SPR_ON_RIDE_PHOTO_CAMERA_FLASH_E },
    };

    bool   takingPhoto  = tile_element_is_taking_photo(tileElement);
    uint32_t imageId      = imageIds[direction][0] | session->TrackColours[SCHEME_MISC];
    uint32_t flashImageId = imageIds[direction][takingPhoto ? 2 : 1] | session->TrackColours[SCHEME_MISC];
    switch (direction)
    {
    case 0:
        sub_98196C(session, imageId, 26, 0, 1, 1, 19, height);
        sub_98196C(session, imageId, 26, 31, 1, 1, 19, height);
        sub_98196C(session, flashImageId, 6, 0, 1, 1, 19, height);
        break;
    case 1:
        sub_98196C(session, imageId, 0, 6, 1, 1, 19, height);
        sub_98196C(session, imageId, 31, 6, 1, 1, 19, height);
        sub_98196C(session, flashImageId, 0, 26, 1, 1, 19, height);
        break;
    case 2:
        sub_98196C(session, imageId, 6, 0, 1, 1, 19, height);
        sub_98196C(session, imageId, 6, 31, 1, 1, 19, height);
        sub_98196C(session, flashImageId, 26, 31, 1, 1, 19, height);
        break;
    case 3:
        sub_98196C(session, imageId, 0, 26, 1, 1, 19, height);
        sub_98196C(session, imageId, 31, 26, 1, 1, 19, height);
        sub_98196C(session, flashImageId, 31, 6, 1, 1, 19, height);
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

void track_paint_util_right_vertical_loop_segments(paint_session * session, uint8_t direction, uint8_t trackSequence)
{
    if (trackSequence > 9)
    {
        // P
        return;
    }

    paint_util_set_segment_support_height(
        session, paint_util_rotate_segments(RightVerticalLoopSegments[trackSequence], direction), 0xFFFF, 0);
}

void track_paint_util_left_corkscrew_up_supports(paint_session * session, uint8_t direction, uint16_t height)
{
    // TODO: Figure out which of these looks best, and use one to keep a consistent world
    if (direction == 2)
    {
        paint_util_set_segment_support_height(
            session, paint_util_rotate_segments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
            0xFFFF, 0);
    }
    metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
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
void track_paint(paint_session * session, uint8_t direction, int32_t height, const rct_tile_element * tileElement)
{
    int32_t rideIndex = track_element_get_ride_index(tileElement);
    Ride * ride      = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_NULL)
    {
        log_error("Attempted to paint invalid ride: %d", rideIndex);
        return;
    }

    // HACK Set entrance style to plain if none to stop glitch until entrance track piece is implemented
    bool isEntranceStyleNone = false;
    if (ride->entrance_style == RIDE_ENTRANCE_STYLE_NONE)
    {
        isEntranceStyleNone  = true;
        ride->entrance_style = RIDE_ENTRANCE_STYLE_PLAIN;
    }

    rct_drawpixelinfo * dpi = session->DPI;

    if ((!gTrackDesignSaveMode || rideIndex == gTrackDesignSaveRideIndex) &&
        !(gCurrentViewportFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
    {
        int32_t trackType         = track_element_get_type(tileElement);
        int32_t trackSequence     = tile_element_get_track_sequence(tileElement);
        int32_t trackColourScheme = track_element_get_colour_scheme(tileElement);

        if ((gCurrentViewportFlags & VIEWPORT_FLAG_TRACK_HEIGHTS) && dpi->zoom_level == 0)
        {
            session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
            if (TrackHeightMarkerPositions[trackType] & (1 << trackSequence))
            {
                uint16_t ax  = RideData5[ride->type].z_offset;
                uint32_t ebx = 0x20381689 + (height + 8) / 16;
                ebx += get_height_marker_offset();
                ebx -= gMapBaseZ;
                sub_98197C(session, ebx, 16, 16, 1, 1, 0, height + ax + 3, 1000, 1000, 2047);
            }
        }

        session->InteractionType               = VIEWPORT_INTERACTION_ITEM_RIDE;
        session->TrackColours[SCHEME_TRACK]    = SPRITE_ID_PALETTE_COLOUR_2(ride->track_colour_main[trackColourScheme],
                                                                         ride->track_colour_additional[trackColourScheme]);
        session->TrackColours[SCHEME_SUPPORTS] = SPRITE_ID_PALETTE_COLOUR_1(ride->track_colour_supports[trackColourScheme]);
        session->TrackColours[SCHEME_MISC]     = IMAGE_TYPE_REMAP;
        session->TrackColours[SCHEME_3]        = 0x20C00000;
        if (tileElement->type & TILE_ELEMENT_TYPE_FLAG_HIGHLIGHT)
        {
            session->TrackColours[SCHEME_TRACK]    = 0x21600000;
            session->TrackColours[SCHEME_SUPPORTS] = 0x21600000;
            session->TrackColours[SCHEME_MISC]     = 0x21600000;
            session->TrackColours[SCHEME_3]        = 0x21600000;
        }
        if (tileElement->flags & TILE_ELEMENT_FLAG_GHOST)
        {
            uint32_t ghost_id                        = CONSTRUCTION_MARKER;
            session->InteractionType               = VIEWPORT_INTERACTION_ITEM_NONE;
            session->TrackColours[SCHEME_TRACK]    = ghost_id;
            session->TrackColours[SCHEME_SUPPORTS] = ghost_id;
            session->TrackColours[SCHEME_MISC]     = ghost_id;
            session->TrackColours[SCHEME_3]        = ghost_id;
        }

        TRACK_PAINT_FUNCTION_GETTER paintFunctionGetter = RideTypeTrackPaintFunctions[ride->type];
        if (paintFunctionGetter != nullptr)
        {
            TRACK_PAINT_FUNCTION paintFunction = paintFunctionGetter(trackType, direction);
            if (paintFunction != nullptr)
            {
                paintFunction(session, rideIndex, trackSequence, direction, height, tileElement);
            }
        }
    }

    if (isEntranceStyleNone)
    {
        ride->entrance_style = RIDE_ENTRANCE_STYLE_NONE;
    }
}
