/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../paint/tile_element/Paint.TileElement.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"

/** rct2: 0x008B08D0 */
static void mine_ride_track_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19338), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19339), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void mine_ride_track_station(
    paint_session& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { 19338, SPR_STATION_BASE_A_SW_NE },
        { 19339, SPR_STATION_BASE_A_NW_SE },
        { 19338, SPR_STATION_BASE_A_SW_NE },
        { 19339, SPR_STATION_BASE_A_NW_SE },
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]), { 0, 0, height },
        { 32, 20, 1 }, { 0, 6, height + 3 });
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][1]), { 0, 0, height },
        { 32, 32, 1 });
    track_paint_util_draw_station_metal_supports_2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 0);
    track_paint_util_draw_station_2(session, ride, direction, height, trackElement, 9, 11);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008B08E0 */
static void mine_ride_track_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19388), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19389), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19390), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19391), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008B08F0 */
static void mine_ride_track_flat_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19380), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19381), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19382), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19383), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008B0900 */
static void mine_ride_track_25_deg_up_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19384), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19385), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19386), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19387), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008B0910 */
static void mine_ride_track_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mine_ride_track_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0920 */
static void mine_ride_track_flat_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mine_ride_track_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0930 */
static void mine_ride_track_25_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mine_ride_track_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0940 */
static void mine_ride_track_left_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19427), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19432), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19437), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19422), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19426), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19431), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19436), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19421), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19425), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19430), { 0, 0, height },
                        { 16, 16, 3 }, { 6, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19435), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19420), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19424), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19429), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19434), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19419), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19423), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19428), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19433), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19418), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0950 */
static void mine_ride_track_right_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mine_ride_track_left_quarter_turn_5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008B0960 */
static void mine_ride_track_flat_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19340), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19348), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19341), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19349), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19342), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19343), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008B0970 */
static void mine_ride_track_flat_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19344), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19345), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19346), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19350), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19347), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19351), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008B0980 */
static void mine_ride_track_left_bank_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19346), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19350), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19347), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19351), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19344), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19345), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008B0990 */
static void mine_ride_track_right_bank_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19342), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19343), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19340), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19348), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19341), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19349), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008B09A0 */
static void mine_ride_track_banked_left_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19447), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19458), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19452), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19457), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19442), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19446), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19451), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19456), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19441), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19445), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19450), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19455), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19440), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19444), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19449), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19454), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19439), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19443), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19448), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19453), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19459), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19438), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B09B0 */
static void mine_ride_track_banked_right_quarter_turn_5(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mine_ride_track_banked_left_quarter_turn_5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008B09C0 */
static void mine_ride_track_left_bank_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19352), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19356), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19353), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19357), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19354), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19355), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008B09D0 */
static void mine_ride_track_right_bank_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19358), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19359), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19360), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19362), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19361), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19363), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008B09E0 */
static void mine_ride_track_25_deg_up_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19364), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19368), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19365), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19369), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19366), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19367), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008B09F0 */
static void mine_ride_track_25_deg_up_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19370), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19371), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19372), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19374), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19373), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19375), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008B0A00 */
static void mine_ride_track_left_bank_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mine_ride_track_25_deg_up_to_right_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0A10 */
static void mine_ride_track_right_bank_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mine_ride_track_25_deg_up_to_left_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0A20 */
static void mine_ride_track_25_deg_down_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mine_ride_track_right_bank_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0A30 */
static void mine_ride_track_25_deg_down_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mine_ride_track_left_bank_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0A40 */
static void mine_ride_track_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19376), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19377), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19378), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19379), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008B0A50 */
static void mine_ride_track_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mine_ride_track_left_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0A60 */
static void mine_ride_track_s_bend_left(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19460), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19464), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19463), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19467), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19461), { 0, 0, height },
                        { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 5, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19465), { 0, 0, height },
                        { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 6, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19462), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19466), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19462), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19466), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19461), { 0, 0, height },
                        { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 5, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19465), { 0, 0, height },
                        { 32, 26, 3 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 6, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19463), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19467), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19460), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19464), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0A70 */
static void mine_ride_track_s_bend_right(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19468), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19472), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19471), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19475), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19469), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 8, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19473), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 7, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19470), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19474), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19470), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19474), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19469), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 8, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19473), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 7, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19471), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19475), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19468), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19472), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0AB0 */
static void mine_ride_track_left_quarter_turn_3(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19397), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19400), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19403), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19394), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19396), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19399), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19402), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19393), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19395), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19398), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19401), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19392), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0AC0 */
static void mine_ride_track_right_quarter_turn_3(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mine_ride_track_left_quarter_turn_3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008B0AD0 */
static void mine_ride_track_left_quarter_turn_3_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19409), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19416), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19412), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19415), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19406), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19408), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19411), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19414), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19405), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19407), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19410), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19413), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19417), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19404), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0AE0 */
static void mine_ride_track_right_quarter_turn_3_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mine_ride_track_left_quarter_turn_3_bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008B0AF0 */
static void mine_ride_track_left_half_banked_helix_up_small(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19495), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19502), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19498), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19501), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19492), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19494), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19497), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19500), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19491), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19493), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19496), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19499), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19503), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19490), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19492), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19495), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19502), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19498), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19501), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19491), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19494), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19497), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19500), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19490), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19493), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19496), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19499), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19503), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0B00 */
static void mine_ride_track_right_half_banked_helix_up_small(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19476), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19479), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19482), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19485), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19489), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19477), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19480), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19483), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19486), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19478), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19481), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19488), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19484), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19487), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19479), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19482), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19485), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19489), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19476), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19480), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19483), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19486), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19477), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19481), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19488), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19484), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19487), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19478), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0B10 */
static void mine_ride_track_left_half_banked_helix_down_small(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mine_ride_track_right_half_banked_helix_up_small(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008B0B20 */
static void mine_ride_track_right_half_banked_helix_down_small(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mine_ride_track_left_half_banked_helix_up_small(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008B0B30 */
static void mine_ride_track_left_half_banked_helix_up_large(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19535), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19546), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19540), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19545), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19530), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19534), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19539), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19544), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19529), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19533), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19538), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19543), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19528), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19532), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19537), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19542), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19527), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19531), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19536), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19541), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19547), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19526), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19530), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19535), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19546), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19540), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19545), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 8:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19529), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19534), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19539), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19544), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 10:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19528), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19533), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19538), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19543), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 11:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19527), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19532), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19537), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19542), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 13:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19526), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19531), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19536), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19541), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19547), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0B40 */
static void mine_ride_track_right_half_banked_helix_up_large(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19504), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19509), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19514), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19519), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19525), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19505), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19510), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19515), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19520), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19506), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19511), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19516), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19521), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19507), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19512), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19517), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19522), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19508), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19513), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19524), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19518), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19523), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19509), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19514), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19519), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19525), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19504), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 8:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19510), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19515), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19520), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19505), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 10:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19511), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19516), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19521), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19506), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 11:
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19512), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19517), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19522), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19507), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 13:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19513), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19524), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19518), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19523), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19508), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0B50 */
static void mine_ride_track_left_half_banked_helix_down_large(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mine_ride_track_right_half_banked_helix_up_large(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008B0B60 */
static void mine_ride_track_right_half_banked_helix_down_large(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mine_ride_track_left_half_banked_helix_up_large(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008B0B70 */
static void mine_ride_track_on_ride_photo(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 5, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 8, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19338), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK_ALT, 6, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK_ALT, 7, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19339), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 5, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK, 8, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19338), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK_ALT, 6, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_FORK_ALT, 7, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19339), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
    }
    track_paint_util_onride_photo_paint(session, direction, height + 3, trackElement);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008B0B90 */
static void mine_ride_track_left_eighth_to_diag(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19564), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19568), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19572), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19576), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19565), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19569), { 0, 0, height },
                        { 34, 16, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19573), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19577), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19566), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19570), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19574), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19578), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19567), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19571), { 0, 0, height },
                        { 16, 18, 3 }, { 0, 16, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19575), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19579), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0BA0 */
static void mine_ride_track_right_eighth_to_diag(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19548), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19552), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19556), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19560), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19549), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19553), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19557), { 0, 0, height },
                        { 34, 16, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19561), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19550), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19554), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19558), { 0, 0, height },
                        { 28, 28, 3 }, { 4, 4, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19562), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19551), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19555), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19559), { 0, 0, height },
                        { 16, 18, 3 }, { 0, 16, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19563), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0BB0 */
static void mine_ride_track_left_eighth_to_orthogonal(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mine_ride_track_right_eighth_to_diag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0BC0 */
static void mine_ride_track_right_eighth_to_orthogonal(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mine_ride_track_left_eighth_to_diag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008B0BD0 */
static void mine_ride_track_left_eighth_bank_to_diag(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19596), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19600), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19604), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19608), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19597), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19601), { 0, 0, height },
                        { 34, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19605), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19609), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19598), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19602), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19606), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19610), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19599), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19603), { 0, 0, height },
                        { 16, 18, 0 }, { 0, 16, height + 27 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19607), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19611), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0BE0 */
static void mine_ride_track_right_eighth_bank_to_diag(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19580), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19584), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19588), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19592), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19581), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19585), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19589), { 0, 0, height },
                        { 34, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19593), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19582), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19586), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19590), { 0, 0, height },
                        { 28, 28, 0 }, { 4, 4, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19594), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19583), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19587), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19591), { 0, 0, height },
                        { 16, 18, 0 }, { 0, 16, height + 27 });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19595), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0BF0 */
static void mine_ride_track_left_eighth_bank_to_orthogonal(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mine_ride_track_right_eighth_bank_to_diag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0C00 */
static void mine_ride_track_right_eighth_bank_to_orthogonal(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mine_ride_track_left_eighth_bank_to_diag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008B0B80 */
static void mine_ride_track_diag_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19615), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19612), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19614), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19613), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0C30 */
static void mine_ride_track_diag_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19627), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19624), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19626), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19625), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008B0C10 */
static void mine_ride_track_diag_flat_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19619), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19616), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19618), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19617), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008B0C20 */
static void mine_ride_track_diag_25_deg_up_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19623), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19620), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19622), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19621), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008B0C60 */
static void mine_ride_track_diag_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19625), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19626), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19624), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19627), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008B0C40 */
static void mine_ride_track_diag_flat_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19621), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19622), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19620), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19623), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008B0C50 */
static void mine_ride_track_diag_25_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19617), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19618), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19616), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19619), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008B0C90 */
static void mine_ride_track_diag_flat_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19635), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19632), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19636), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19634), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19633), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0CA0 */
static void mine_ride_track_diag_flat_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19640), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19637), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19639), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19641), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19638), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0CB0 */
static void mine_ride_track_diag_left_bank_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19638), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19639), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19641), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19637), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19640), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0CC0 */
static void mine_ride_track_diag_right_bank_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19633), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19634), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19632), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19636), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19635), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0CF0 */
static void mine_ride_track_diag_left_bank_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19655), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19652), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19656), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19654), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19653), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008B0D00 */
static void mine_ride_track_diag_right_bank_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19660), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19657), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19659), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19661), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19658), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008B0CD0 */
static void mine_ride_track_diag_25_deg_up_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19645), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19642), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19646), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19644), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19643), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008B0CE0 */
static void mine_ride_track_diag_25_deg_up_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19650), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19647), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19649), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19651), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19648), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008B0D10 */
static void mine_ride_track_diag_left_bank_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19648), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19649), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19651), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19647), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19650), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008B0D20 */
static void mine_ride_track_diag_right_bank_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19643), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19644), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19642), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19646), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19645), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008B0D30 */
static void mine_ride_track_diag_25_deg_down_to_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19658), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19659), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19661), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19657), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19660), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008B0D40 */
static void mine_ride_track_diag_25_deg_down_to_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19653), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19654), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19652), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19656), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19655), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_b_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008B0C70 */
static void mine_ride_track_diag_left_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19631), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19628), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19630), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19629), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008B0C80 */
static void mine_ride_track_diag_right_bank(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19629), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19630), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19628), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19631), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    metal_a_supports_paint_setup(
                        session, METAL_SUPPORTS_FORK_ALT, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

TRACK_PAINT_FUNCTION get_track_paint_function_mine_ride(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return mine_ride_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return mine_ride_track_station;
        case TrackElemType::Up25:
            return mine_ride_track_25_deg_up;
        case TrackElemType::FlatToUp25:
            return mine_ride_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return mine_ride_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return mine_ride_track_25_deg_down;
        case TrackElemType::FlatToDown25:
            return mine_ride_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return mine_ride_track_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return mine_ride_track_left_quarter_turn_5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return mine_ride_track_right_quarter_turn_5;
        case TrackElemType::FlatToLeftBank:
            return mine_ride_track_flat_to_left_bank;
        case TrackElemType::FlatToRightBank:
            return mine_ride_track_flat_to_right_bank;
        case TrackElemType::LeftBankToFlat:
            return mine_ride_track_left_bank_to_flat;
        case TrackElemType::RightBankToFlat:
            return mine_ride_track_right_bank_to_flat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return mine_ride_track_banked_left_quarter_turn_5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return mine_ride_track_banked_right_quarter_turn_5;
        case TrackElemType::LeftBankToUp25:
            return mine_ride_track_left_bank_to_25_deg_up;
        case TrackElemType::RightBankToUp25:
            return mine_ride_track_right_bank_to_25_deg_up;
        case TrackElemType::Up25ToLeftBank:
            return mine_ride_track_25_deg_up_to_left_bank;
        case TrackElemType::Up25ToRightBank:
            return mine_ride_track_25_deg_up_to_right_bank;
        case TrackElemType::LeftBankToDown25:
            return mine_ride_track_left_bank_to_25_deg_down;
        case TrackElemType::RightBankToDown25:
            return mine_ride_track_right_bank_to_25_deg_down;
        case TrackElemType::Down25ToLeftBank:
            return mine_ride_track_25_deg_down_to_left_bank;
        case TrackElemType::Down25ToRightBank:
            return mine_ride_track_25_deg_down_to_right_bank;
        case TrackElemType::LeftBank:
            return mine_ride_track_left_bank;
        case TrackElemType::RightBank:
            return mine_ride_track_right_bank;
        case TrackElemType::SBendLeft:
            return mine_ride_track_s_bend_left;
        case TrackElemType::SBendRight:
            return mine_ride_track_s_bend_right;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return mine_ride_track_left_quarter_turn_3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return mine_ride_track_right_quarter_turn_3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return mine_ride_track_left_quarter_turn_3_bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return mine_ride_track_right_quarter_turn_3_bank;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return mine_ride_track_left_half_banked_helix_up_small;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return mine_ride_track_right_half_banked_helix_up_small;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return mine_ride_track_left_half_banked_helix_down_small;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return mine_ride_track_right_half_banked_helix_down_small;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return mine_ride_track_left_half_banked_helix_up_large;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return mine_ride_track_right_half_banked_helix_up_large;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return mine_ride_track_left_half_banked_helix_down_large;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return mine_ride_track_right_half_banked_helix_down_large;
        case TrackElemType::OnRidePhoto:
            return mine_ride_track_on_ride_photo;
        case TrackElemType::LeftEighthToDiag:
            return mine_ride_track_left_eighth_to_diag;
        case TrackElemType::RightEighthToDiag:
            return mine_ride_track_right_eighth_to_diag;
        case TrackElemType::LeftEighthToOrthogonal:
            return mine_ride_track_left_eighth_to_orthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return mine_ride_track_right_eighth_to_orthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return mine_ride_track_left_eighth_bank_to_diag;
        case TrackElemType::RightEighthBankToDiag:
            return mine_ride_track_right_eighth_bank_to_diag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return mine_ride_track_left_eighth_bank_to_orthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return mine_ride_track_right_eighth_bank_to_orthogonal;
        case TrackElemType::DiagFlat:
            return mine_ride_track_diag_flat;
        case TrackElemType::DiagUp25:
            return mine_ride_track_diag_25_deg_up;
        case TrackElemType::DiagFlatToUp25:
            return mine_ride_track_diag_flat_to_25_deg_up;
        case TrackElemType::DiagUp25ToFlat:
            return mine_ride_track_diag_25_deg_up_to_flat;
        case TrackElemType::DiagDown25:
            return mine_ride_track_diag_25_deg_down;
        case TrackElemType::DiagFlatToDown25:
            return mine_ride_track_diag_flat_to_25_deg_down;
        case TrackElemType::DiagDown25ToFlat:
            return mine_ride_track_diag_25_deg_down_to_flat;
        case TrackElemType::DiagFlatToLeftBank:
            return mine_ride_track_diag_flat_to_left_bank;
        case TrackElemType::DiagFlatToRightBank:
            return mine_ride_track_diag_flat_to_right_bank;
        case TrackElemType::DiagLeftBankToFlat:
            return mine_ride_track_diag_left_bank_to_flat;
        case TrackElemType::DiagRightBankToFlat:
            return mine_ride_track_diag_right_bank_to_flat;
        case TrackElemType::DiagLeftBankToUp25:
            return mine_ride_track_diag_left_bank_to_25_deg_up;
        case TrackElemType::DiagRightBankToUp25:
            return mine_ride_track_diag_right_bank_to_25_deg_up;
        case TrackElemType::DiagUp25ToLeftBank:
            return mine_ride_track_diag_25_deg_up_to_left_bank;
        case TrackElemType::DiagUp25ToRightBank:
            return mine_ride_track_diag_25_deg_up_to_right_bank;
        case TrackElemType::DiagLeftBankToDown25:
            return mine_ride_track_diag_left_bank_to_25_deg_down;
        case TrackElemType::DiagRightBankToDown25:
            return mine_ride_track_diag_right_bank_to_25_deg_down;
        case TrackElemType::DiagDown25ToLeftBank:
            return mine_ride_track_diag_25_deg_down_to_left_bank;
        case TrackElemType::DiagDown25ToRightBank:
            return mine_ride_track_diag_25_deg_down_to_right_bank;
        case TrackElemType::DiagLeftBank:
            return mine_ride_track_diag_left_bank;
        case TrackElemType::DiagRightBank:
            return mine_ride_track_diag_right_bank;
    }
    return nullptr;
}
