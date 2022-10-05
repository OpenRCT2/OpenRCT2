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

/** rct2: 0x008B0460 */
static void inverted_impulse_rc_track_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19662), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19663), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        metal_a_supports_paint_setup(
            session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
    }

    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_INVERTED_3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008B0470, 0x008B0480, 0x008B0490 */
static void inverted_impulse_rc_track_station(
    paint_session& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { SPR_STATION_BASE_C_SW_NE, 19662, SPR_STATION_INVERTED_BAR_B_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 19663, SPR_STATION_INVERTED_BAR_B_NW_SE },
        { SPR_STATION_BASE_C_SW_NE, 19662, SPR_STATION_INVERTED_BAR_B_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 19663, SPR_STATION_INVERTED_BAR_B_NW_SE },
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][0]), { 0, 0, height },
        { 32, 28, 1 }, { 0, 2, height });
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]), { 0, 0, height + 29 },
        { 32, 20, 3 }, { 0, 6, height + 29 });
    PaintAddImageAsChildRotated(
        session, direction, session.TrackColours[SCHEME_SUPPORTS].WithIndex(imageIds[direction][2]), { 0, 6, height + 29 },
        { 32, 20, 3 }, { 0, 6, height + 29 });
    track_paint_util_draw_station_metal_supports_2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 11);
    track_paint_util_draw_station_inverted(session, ride, direction, height, trackElement, STATION_VARIANT_TALL);
    paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_SQUARE_INVERTED_9);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008B04A0 */
static void inverted_impulse_rc_track_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19672), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 45 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19673), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 45 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19674), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 45 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19675), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 45 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                metal_a_supports_paint_setup(
                    session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008B04B0 */
static void inverted_impulse_rc_track_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19688), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 93 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19689), { 0, 0, height + 29 }, { 32, 2, 81 },
                { 0, 4, height + 11 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19690), { 0, 0, height + 29 }, { 32, 2, 81 },
                { 0, 4, height + 11 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19691), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 93 });
            break;
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 56, TUNNEL_INVERTED_5);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
}

/** rct2: 0x008B04C0 */
static void inverted_impulse_rc_track_flat_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19664), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19665), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19666), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19667), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    switch (direction)
    {
        case 0:
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }

    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_INVERTED_3);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height, TUNNEL_INVERTED_5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x008B04D0 */
static void inverted_impulse_rc_track_25_deg_up_to_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19676), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 61 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19680), { 0, 0, height + 29 }, { 32, 10, 49 },
                { 0, 10, height + 11 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19677), { 0, 0, height + 29 }, { 32, 2, 49 },
                { 0, 4, height + 11 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19681), { 0, 0, height + 29 }, { 32, 10, 49 },
                { 0, 10, height + 11 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19678), { 0, 0, height + 29 }, { 32, 2, 49 },
                { 0, 4, height + 11 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19679), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 61 });
            break;
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_INVERTED_5);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
}

/** rct2: 0x008B04E0 */
static void inverted_impulse_rc_track_60_deg_up_to_25_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19682), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 61 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19686), { 0, 0, height + 29 }, { 32, 10, 49 },
                { 0, 10, height + 11 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19683), { 0, 0, height + 29 }, { 32, 2, 49 },
                { 0, 4, height + 11 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19687), { 0, 0, height + 29 }, { 32, 10, 49 },
                { 0, 10, height + 11 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19684), { 0, 0, height + 29 }, { 32, 2, 49 },
                { 0, 4, height + 11 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19685), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 61 });
            break;
    }
    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_INVERTED_4);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 24, TUNNEL_INVERTED_5);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
}

/** rct2: 0x008B04F0 */
static void inverted_impulse_rc_track_25_deg_up_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19668), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19669), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19670), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19671), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    switch (direction)
    {
        case 0:
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 6, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 8, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 7, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            metal_a_supports_paint_setup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 5, 0, height + 52, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }

    if (direction == 0 || direction == 3)
    {
        paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_INVERTED_3);
    }
    else
    {
        paint_util_push_tunnel_rotated(session, direction, height + 8, TUNNEL_13);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008B0500 */
static void inverted_impulse_rc_track_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_impulse_rc_track_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0510 */
static void inverted_impulse_rc_track_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_impulse_rc_track_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0520 */
static void inverted_impulse_rc_track_flat_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_impulse_rc_track_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0530 */
static void inverted_impulse_rc_track_25_deg_down_to_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_impulse_rc_track_60_deg_up_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0540 */
static void inverted_impulse_rc_track_60_deg_down_to_25_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_impulse_rc_track_25_deg_up_to_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0550 */
static void inverted_impulse_rc_track_25_deg_down_to_flat(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_impulse_rc_track_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B05A0 */
static void inverted_impulse_rc_track_90_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19700), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 61 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19701), { 0, 0, height + 29 },
                        { 32, 2, 31 }, { 0, 4, height + 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19702), { 0, 0, height + 29 },
                        { 32, 2, 31 }, { 0, 4, height + 11 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19703), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 61 });
                    break;
            }
            PaintUtilSetVerticalTunnel(session, height + 32);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008B05B0 */
static void inverted_impulse_rc_track_90_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_impulse_rc_track_90_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0560 */
static void inverted_impulse_rc_track_60_deg_up_to_90_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19692), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 85 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19693), { 0, 0, height + 29 },
                        { 32, 2, 55 }, { 0, 4, height + 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19694), { 0, 0, height + 29 },
                        { 32, 2, 55 }, { 0, 4, height + 11 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19695), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 85 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height - 8, TUNNEL_INVERTED_4);
            }
            PaintUtilSetVerticalTunnel(session, height + 56);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008B0570 */
static void inverted_impulse_rc_track_90_deg_down_to_60_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_impulse_rc_track_60_deg_up_to_90_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008B0580 */
static void inverted_impulse_rc_track_90_deg_up_to_60_deg_up(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19696), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 85 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19697), { 0, 0, height + 29 }, { 32, 2, 55 },
                { 0, 4, height + 11 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19698), { 0, 0, height + 29 }, { 32, 2, 55 },
                { 0, 4, height + 11 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19699), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 85 });
            break;
    }
    switch (direction)
    {
        case 1:
            PaintUtilPushTunnelRight(session, height + 48, TUNNEL_INVERTED_5);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 48, TUNNEL_INVERTED_5);
            break;
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 96, 0x20);
}

/** rct2: 0x008B0590 */
static void inverted_impulse_rc_track_60_deg_down_to_90_deg_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19698), { 0, 0, height + 29 },
                        { 32, 2, 55 }, { 0, 4, height + 11 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19699), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 85 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19696), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 85 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19697), { 0, 0, height + 29 },
                        { 32, 2, 55 }, { 0, 4, height + 11 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                paint_util_push_tunnel_rotated(session, direction, height + 48, TUNNEL_INVERTED_5);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96, 0x20);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008B05C0 */
static void inverted_impulse_rc_track_left_quarter_turn_1_90_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19708), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 125 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19709), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 125 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19717), { 0, 0, height + 29 },
                        { 2, 32, 31 }, { 4, 0, height + 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19710), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 125 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19718), { 0, 0, height + 29 },
                        { 32, 2, 31 }, { 0, 4, height + 11 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19711), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 125 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19719), { 0, 0, height + 29 },
                        { 32, 2, 31 }, { 0, 4, height + 11 });
                    break;
            }
            PaintUtilSetVerticalTunnel(session, height + 96);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96, 0x20);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008B05D0 */
static void inverted_impulse_rc_track_right_quarter_turn_1_90_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19704), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 125 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19712), { 0, 0, height + 29 },
                        { 32, 2, 31 }, { 0, 4, height + 11 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19705), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 125 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19713), { 0, 0, height + 29 },
                        { 32, 2, 31 }, { 0, 4, height + 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19706), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 125 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19714), { 0, 0, height + 29 },
                        { 32, 2, 31 }, { 0, 4, height + 11 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19707), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 125 });
                    break;
            }
            PaintUtilSetVerticalTunnel(session, height + 96);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96, 0x20);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008B05E0 */
static void inverted_impulse_rc_track_left_quarter_turn_1_90_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_impulse_rc_track_right_quarter_turn_1_90_deg_up(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008B05F0 */
static void inverted_impulse_rc_track_right_quarter_turn_1_90_deg_down(
    paint_session& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_impulse_rc_track_left_quarter_turn_1_90_deg_up(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

TRACK_PAINT_FUNCTION get_track_paint_function_inverted_impulse_rc(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return inverted_impulse_rc_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return inverted_impulse_rc_track_station;
        case TrackElemType::Up25:
            return inverted_impulse_rc_track_25_deg_up;
        case TrackElemType::Up60:
            return inverted_impulse_rc_track_60_deg_up;
        case TrackElemType::FlatToUp25:
            return inverted_impulse_rc_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToUp60:
            return inverted_impulse_rc_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return inverted_impulse_rc_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return inverted_impulse_rc_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return inverted_impulse_rc_track_25_deg_down;
        case TrackElemType::Down60:
            return inverted_impulse_rc_track_60_deg_down;
        case TrackElemType::FlatToDown25:
            return inverted_impulse_rc_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToDown60:
            return inverted_impulse_rc_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return inverted_impulse_rc_track_60_deg_down_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return inverted_impulse_rc_track_25_deg_down_to_flat;
        case TrackElemType::Up90:
            return inverted_impulse_rc_track_90_deg_up;
        case TrackElemType::Down90:
            return inverted_impulse_rc_track_90_deg_down;
        case TrackElemType::Up60ToUp90:
            return inverted_impulse_rc_track_60_deg_up_to_90_deg_up;
        case TrackElemType::Down90ToDown60:
            return inverted_impulse_rc_track_90_deg_down_to_60_deg_down;
        case TrackElemType::Up90ToUp60:
            return inverted_impulse_rc_track_90_deg_up_to_60_deg_up;
        case TrackElemType::Down60ToDown90:
            return inverted_impulse_rc_track_60_deg_down_to_90_deg_down;
        case TrackElemType::LeftQuarterTurn1TileUp90:
            return inverted_impulse_rc_track_left_quarter_turn_1_90_deg_up;
        case TrackElemType::RightQuarterTurn1TileUp90:
            return inverted_impulse_rc_track_right_quarter_turn_1_90_deg_up;
        case TrackElemType::LeftQuarterTurn1TileDown90:
            return inverted_impulse_rc_track_left_quarter_turn_1_90_deg_down;
        case TrackElemType::RightQuarterTurn1TileDown90:
            return inverted_impulse_rc_track_right_quarter_turn_1_90_deg_down;
    }
    return nullptr;
}
