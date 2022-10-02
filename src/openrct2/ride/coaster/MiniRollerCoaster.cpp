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

static constexpr const uint32_t MINI_RC_BLOCK_BRAKE_SW_NE_OPEN = 18742;
static constexpr const uint32_t MINI_RC_BLOCK_BRAKE_NW_SE_OPEN = 18743;
static constexpr const uint32_t MINI_RC_BLOCK_BRAKE_SW_NE_CLOSED = 18744;
static constexpr const uint32_t MINI_RC_BLOCK_BRAKE_NW_SE_CLOSED = 18745;

static constexpr const uint32_t _MiniRCBlockBrakeImages[NumOrthogonalDirections][2] = {
    { MINI_RC_BLOCK_BRAKE_SW_NE_OPEN, MINI_RC_BLOCK_BRAKE_SW_NE_CLOSED },
    { MINI_RC_BLOCK_BRAKE_NW_SE_OPEN, MINI_RC_BLOCK_BRAKE_NW_SE_CLOSED },
    { MINI_RC_BLOCK_BRAKE_SW_NE_OPEN, MINI_RC_BLOCK_BRAKE_SW_NE_CLOSED },
    { MINI_RC_BLOCK_BRAKE_NW_SE_OPEN, MINI_RC_BLOCK_BRAKE_NW_SE_CLOSED },
};

/** rct2: 0x008A4ABC */
static void mini_rc_track_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19044), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19045), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19046), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19047), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18738), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18739), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
        }
    }
    PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void mini_rc_track_station(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { 18746, SPR_STATION_BASE_A_SW_NE },
        { 18747, SPR_STATION_BASE_A_NW_SE },
        { 18746, SPR_STATION_BASE_A_SW_NE },
        { 18747, SPR_STATION_BASE_A_NW_SE },
    };

    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        bool isClosed = trackElement.BlockBrakeClosed();
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_MiniRCBlockBrakeImages[direction][isClosed]),
            { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height + 3 });
    }
    else
    {
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]), { 0, 0, height },
            { 32, 20, 1 }, { 0, 6, height + 3 });
    }
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][1]), { 0, 0, height },
        { 32, 32, 1 });
    track_paint_util_draw_station_metal_supports_2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 0);
    track_paint_util_draw_station_2(session, ride, direction, height, trackElement, 9, 11);
    PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A4ACC */
static void mini_rc_track_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19056), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19057), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19058), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19059), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18796), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18797), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18798), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18799), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A4ADC */
static void mini_rc_track_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18812), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 38, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18813), { 0, 0, height }, { 32, 1, 98 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 38, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18814), { 0, 0, height }, { 32, 1, 98 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 38, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18815), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 38, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008A4AEC */
static void mini_rc_track_flat_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19048), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19049), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19050), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19051), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18788), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18789), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18790), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18791), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A4AFC */
static void mini_rc_track_25_deg_up_to_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18800), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 18, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18801), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18804), { 0, 0, height }, { 32, 1, 66 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 18, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18802), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18805), { 0, 0, height }, { 32, 1, 66 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 18, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18803), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 18, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A4B0C */
static void mini_rc_track_60_deg_up_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18806), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 26, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18807), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18810), { 0, 0, height }, { 32, 1, 66 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 26, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18808), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18811), { 0, 0, height }, { 32, 1, 66 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 26, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18809), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 26, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A4B1C */
static void mini_rc_track_25_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19052), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19053), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19054), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19055), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18792), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18793), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18794), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18795), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                if (track_paint_util_should_paint_supports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, Tunnel14);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008A4B2C */
static void mini_rc_track_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4B3C */
static void mini_rc_track_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4B4C */
static void mini_rc_track_flat_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4B5C */
static void mini_rc_track_25_deg_down_to_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_60_deg_up_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4B6C */
static void mini_rc_track_60_deg_down_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_25_deg_up_to_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4B7C */
static void mini_rc_track_25_deg_down_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4B8C */
static void mini_rc_track_left_quarter_turn_5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18851), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18856), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18861), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18846), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18850), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18855), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18860), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18845), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18849), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18854), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18859), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18844), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18848), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18853), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18858), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18843), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18847), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18852), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18857), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18842), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TunnelSquareFlat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4B9C */
static void mini_rc_track_right_quarter_turn_5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mini_rc_track_left_quarter_turn_5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A4BAC */
static void mini_rc_track_flat_to_left_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18748), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18756), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18749), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18757), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18750), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18751), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A4BBC */
static void mini_rc_track_flat_to_right_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18752), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18753), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18754), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18758), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18755), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18759), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A4BCC */
static void mini_rc_track_left_bank_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18754), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18758), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18755), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18759), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18752), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18753), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A4BDC */
static void mini_rc_track_right_bank_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18750), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18751), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18748), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18756), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18749), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18757), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A4BEC */
static void mini_rc_track_banked_left_quarter_turn_5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18871), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18882), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18876), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18881), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18866), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18870), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18875), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18880), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18865), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18869), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18874), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18879), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18864), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18868), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18873), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18878), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18863), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18867), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18872), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18877), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18883), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18862), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TunnelSquareFlat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4BFC */
static void mini_rc_track_banked_right_quarter_turn_5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mini_rc_track_banked_left_quarter_turn_5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A4C0C */
static void mini_rc_track_left_bank_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18760), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18764), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18761), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18765), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18762), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18763), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A4C1C */
static void mini_rc_track_right_bank_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18766), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18767), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18768), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18770), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18769), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18771), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A4C2C */
static void mini_rc_track_25_deg_up_to_left_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18772), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18776), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18773), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18777), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18774), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18775), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, Tunnel14);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008A4C3C */
static void mini_rc_track_25_deg_up_to_right_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18778), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18779), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18780), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18782), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18781), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18783), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, Tunnel14);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008A4C4C */
static void mini_rc_track_left_bank_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_25_deg_up_to_right_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4C5C */
static void mini_rc_track_right_bank_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_25_deg_up_to_left_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4C6C */
static void mini_rc_track_25_deg_down_to_left_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_right_bank_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4C7C */
static void mini_rc_track_25_deg_down_to_right_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_left_bank_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4C8C */
static void mini_rc_track_left_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18784), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18785), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18786), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18787), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A4C9C */
static void mini_rc_track_right_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_left_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4CAC */
static void mini_rc_track_left_quarter_turn_5_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18936), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18941), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18946), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18951), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18937), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18942), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18947), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18952), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18938), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18943), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18948), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18953), { 0, 0, height },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18939), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18944), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18949), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18954), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18940), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18945), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18950), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18955), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelSquare8);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelSquare8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A4CBC */
static void mini_rc_track_right_quarter_turn_5_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18916), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18921), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18926), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18931), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18917), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18922), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18927), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18932), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18918), { 0, 0, height },
                        { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18923), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18928), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18933), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18919), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18924), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18929), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18934), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18920), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18925), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18930), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18935), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelSquare8);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelSquare8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A4CCC */
static void mini_rc_track_left_quarter_turn_5_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mini_rc_track_right_quarter_turn_5_25_deg_up(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A4CDC */
static void mini_rc_track_right_quarter_turn_5_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mini_rc_track_left_quarter_turn_5_25_deg_up(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A4CEC */
static void mini_rc_track_s_bend_left(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18900), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18904), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18903), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18907), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18901), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 5, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18905), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 6, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18902), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18906), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18902), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18906), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18901), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 5, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18905), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 6, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18903), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18907), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18900), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18904), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TunnelSquareFlat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4CFC */
static void mini_rc_track_s_bend_right(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18908), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18912), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18911), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18915), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18909), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 8, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18913), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 7, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18910), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18914), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18910), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18914), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18909), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 8, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18913), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 7, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18911), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18915), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18908), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18912), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TunnelSquareFlat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4D3C */
static void mini_rc_track_left_quarter_turn_3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18821), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18824), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18827), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18818), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18820), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18823), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18826), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18817), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18819), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18822), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18825), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18816), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TunnelSquareFlat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4D4C */
static void mini_rc_track_right_quarter_turn_3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mini_rc_track_left_quarter_turn_3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A4D5C */
static void mini_rc_track_left_quarter_turn_3_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18833), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18840), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18836), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18839), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18830), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18832), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18835), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18838), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18829), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18831), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18834), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18837), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18841), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18828), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TunnelSquareFlat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4D6C */
static void mini_rc_track_right_quarter_turn_3_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mini_rc_track_left_quarter_turn_3_bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A4D7C */
static void mini_rc_track_left_quarter_turn_3_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18967), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18969), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18971), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18965), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18966), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18968), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18970), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18964), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelSquare8);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelSquare8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A4D8C */
static void mini_rc_track_right_quarter_turn_3_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18956), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18958), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18960), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18962), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18957), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18959), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18961), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18963), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelSquare8);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelSquare8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A4D9C */
static void mini_rc_track_left_quarter_turn_3_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mini_rc_track_right_quarter_turn_3_25_deg_up(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A4DAC */
static void mini_rc_track_right_quarter_turn_3_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mini_rc_track_left_quarter_turn_3_25_deg_up(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A4DBC */
static void mini_rc_track_left_half_banked_helix_up_small(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18991), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18998), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18994), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18997), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18988), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction), 0xFFFF,
                0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18990), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18993), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18996), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18987), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18989), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18992), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18995), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18999), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18986), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelSquareFlat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18988), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18991), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18998), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18994), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18997), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, TunnelSquareFlat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18987), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18990), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18993), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18996), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18986), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18989), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18992), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18995), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18999), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4DCC */
static void mini_rc_track_right_half_banked_helix_up_small(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18972), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18975), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18978), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18981), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18985), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18973), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18976), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18979), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18982), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18974), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18977), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18984), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18980), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18983), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelSquareFlat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18975), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18978), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18981), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18985), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18972), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TunnelSquareFlat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18976), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18979), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18982), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18973), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18977), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18984), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18980), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18983), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18974), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4DDC */
static void mini_rc_track_left_half_banked_helix_down_small(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mini_rc_track_right_half_banked_helix_up_small(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A4DEC */
static void mini_rc_track_right_half_banked_helix_down_small(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mini_rc_track_left_half_banked_helix_up_small(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A4DFC */
static void mini_rc_track_left_half_banked_helix_up_large(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19031), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19042), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19036), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19041), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19026), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction), 0xFFFF,
                0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19030), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19035), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19040), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19025), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19029), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19034), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19039), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19024), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19028), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19033), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19038), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19023), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19027), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19032), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19037), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19043), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19022), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelSquareFlat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19026), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19031), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19042), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19036), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19041), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, TunnelSquareFlat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19025), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19030), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19035), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19040), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 10:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19024), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19029), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19034), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19039), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19023), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19028), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19033), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19038), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 13:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19022), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19027), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19032), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19037), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19043), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4E0C */
static void mini_rc_track_right_half_banked_helix_up_large(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19000), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19005), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19010), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19015), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19021), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19001), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19006), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19011), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19016), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19002), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19007), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19012), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19017), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19003), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19008), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19013), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19018), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19004), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19009), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19020), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19014), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19019), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelSquareFlat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19005), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19010), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19015), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19021), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19000), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TunnelSquareFlat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelSquareFlat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19006), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19011), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19016), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19001), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 10:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19007), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19012), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19017), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19002), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19008), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19013), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19018), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19003), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 13:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19009), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19020), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19014), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19019), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19004), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4E1C */
static void mini_rc_track_left_half_banked_helix_down_large(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mini_rc_track_right_half_banked_helix_up_large(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A4E2C */
static void mini_rc_track_right_half_banked_helix_down_large(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mini_rc_track_left_half_banked_helix_up_large(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A4E5C */
static void mini_rc_track_left_quarter_turn_1_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18893), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18897), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18894), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18898), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18895), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18899), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18892), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18896), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
    }
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, -8, TunnelSquare7, +56, TunnelSquare8);
    PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008A4E3C */
static void mini_rc_track_right_quarter_turn_1_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18884), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18888), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18885), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18889), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18886), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18890), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18887), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18891), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
    }
    track_paint_util_right_quarter_turn_1_tile_tunnel(session, direction, height, -8, TunnelSquare7, +56, TunnelSquare8);
    PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008A4E4C */
static void mini_rc_track_left_quarter_turn_1_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_right_quarter_turn_1_60_deg_up(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A4E6C */
static void mini_rc_track_right_quarter_turn_1_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_left_quarter_turn_1_60_deg_up(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A4E7C */
static void mini_rc_track_brakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18740), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18741), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A50CC */
static void mini_rc_track_25_deg_up_left_banked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19222), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19223), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19224), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19225), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A50DC */
static void mini_rc_track_25_deg_up_right_banked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19226), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19227), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19228), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19229), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A4E8C */
static void mini_rc_track_on_ride_photo(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, MetalSupportsFork, 5, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, MetalSupportsFork, 8, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18738), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 6, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 7, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18739), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, MetalSupportsFork, 5, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, MetalSupportsFork, 8, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18738), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 6, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 7, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(18739), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
    }
    track_paint_util_onride_photo_paint(session, direction, height + 3, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A50EC */
static void mini_rc_track_25_deg_down_left_banked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_25_deg_up_right_banked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A50FC */
static void mini_rc_track_25_deg_down_right_banked(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_25_deg_up_left_banked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4EAC */
static void mini_rc_track_left_eighth_to_diag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19076), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19080), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19084), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19088), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19077), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19081), { 0, 0, height },
                        { 34, 16, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19085), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19089), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19078), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19082), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19086), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19090), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19079), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19083), { 0, 0, height },
                        { 16, 18, 3 }, { 0, 16, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19087), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19091), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4EBC */
static void mini_rc_track_right_eighth_to_diag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19060), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19064), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19068), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19072), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19061), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19065), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19069), { 0, 0, height },
                        { 34, 16, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19073), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19062), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19066), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19070), { 0, 0, height },
                        { 28, 28, 3 }, { 4, 4, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19074), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19063), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19067), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19071), { 0, 0, height },
                        { 16, 18, 3 }, { 0, 16, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19075), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4ECC */
static void mini_rc_track_left_eighth_to_orthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mini_rc_track_right_eighth_to_diag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4EDC */
static void mini_rc_track_right_eighth_to_orthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mini_rc_track_left_eighth_to_diag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008A4EEC */
static void mini_rc_track_left_eighth_bank_to_diag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19108), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19112), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19116), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19120), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19109), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19113), { 0, 0, height },
                        { 34, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19117), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19121), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19110), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19114), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19118), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19122), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19111), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19115), { 0, 0, height },
                        { 16, 18, 0 }, { 0, 16, height + 27 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19119), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19123), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4EFC */
static void mini_rc_track_right_eighth_bank_to_diag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19092), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19096), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19100), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19104), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19093), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19097), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19101), { 0, 0, height },
                        { 34, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19105), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19094), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19098), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19102), { 0, 0, height },
                        { 28, 28, 0 }, { 4, 4, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19106), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19095), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19099), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19103), { 0, 0, height },
                        { 16, 18, 0 }, { 0, 16, height + 27 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19107), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4F0C */
static void mini_rc_track_left_eighth_bank_to_orthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mini_rc_track_right_eighth_bank_to_diag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A4F1C */
static void mini_rc_track_right_eighth_bank_to_orthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    mini_rc_track_left_eighth_bank_to_diag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008A4E9C */
static void mini_rc_track_diag_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19189), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19127), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19186), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19124), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19188), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19126), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19187), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalASupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19125), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalASupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4F4C */
static void mini_rc_track_diag_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19201), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19139), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19198), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19136), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19200), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19138), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 1, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19199), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 2, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 1, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19137), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 2, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A4FAC */
static void mini_rc_track_diag_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19151), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19148), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19150), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 38, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19149), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 42, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 38, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 42, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008A4F2C */
static void mini_rc_track_diag_flat_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19193), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19131), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19190), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19128), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19192), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19130), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19191), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19129), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A4F8C */
static void mini_rc_track_diag_25_deg_up_to_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19143), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19140), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19142), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 22, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19141), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 22, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 22, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 22, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A4F9C */
static void mini_rc_track_diag_60_deg_up_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19147), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19144), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19146), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 27, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19145), { -16, -16, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 27, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 27, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 27, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A4F3C */
static void mini_rc_track_diag_25_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19197), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19135), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19194), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19132), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19196), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19134), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19195), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19133), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A4F7C */
static void mini_rc_track_diag_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19199), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19137), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19200), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19138), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19198), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19136), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 1, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19201), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 2, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 1, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19139), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 2, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A4FDC */
static void mini_rc_track_diag_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19149), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19150), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19148), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 30, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19151), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 34, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 30, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 34, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008A4F5C */
static void mini_rc_track_diag_flat_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19195), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19133), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19196), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19134), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19194), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19132), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19197), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19135), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A4FBC */
static void mini_rc_track_diag_25_deg_down_to_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19145), { -16, -16, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19146), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19144), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 23, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19147), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 23, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 23, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 23, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A4FCC */
static void mini_rc_track_diag_60_deg_down_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19141), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19142), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19140), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19143), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A4F6C */
static void mini_rc_track_diag_25_deg_down_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19191), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19129), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19192), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19130), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19190), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19128), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19193), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19131), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A500C */
static void mini_rc_track_diag_flat_to_left_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19159), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19156), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19160), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19158), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19157), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A501C */
static void mini_rc_track_diag_flat_to_right_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19164), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19161), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19163), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19165), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19162), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A502C */
static void mini_rc_track_diag_left_bank_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19162), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19163), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19165), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19161), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19164), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A503C */
static void mini_rc_track_diag_right_bank_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19157), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19158), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19156), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19160), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19159), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A506C */
static void mini_rc_track_diag_left_bank_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19179), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19176), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19180), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19178), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19177), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A507C */
static void mini_rc_track_diag_right_bank_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19184), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19181), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19183), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19185), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19182), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A504C */
static void mini_rc_track_diag_25_deg_up_to_left_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19169), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19166), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19170), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19168), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19167), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A505C */
static void mini_rc_track_diag_25_deg_up_to_right_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19174), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19171), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19173), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19175), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19172), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A508C */
static void mini_rc_track_diag_left_bank_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19172), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19173), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19175), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19171), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19174), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A509C */
static void mini_rc_track_diag_right_bank_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19167), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19168), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19166), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19170), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19169), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A50AC */
static void mini_rc_track_diag_25_deg_down_to_left_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19182), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19183), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19185), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19181), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19184), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A50BC */
static void mini_rc_track_diag_25_deg_down_to_right_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19177), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19178), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19176), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19180), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 35 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19179), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A4FEC */
static void mini_rc_track_diag_left_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19155), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19152), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19154), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19153), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A4FFC */
static void mini_rc_track_diag_right_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19153), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19154), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19152), { -16, -16, height },
                        { 32, 32, 0 }, { -16, -16, height + 27 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19155), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 0, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 2, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 3, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A530C */
static void mini_rc_track_block_brakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isClosed = trackElement.BlockBrakeClosed();
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_MiniRCBlockBrakeImages[direction][isClosed]),
        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
    switch (direction)
    {
        case 0:
        case 2:
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
        case 3:
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A510C */
static void mini_rc_track_left_banked_quarter_turn_3_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19317), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19319), { 0, 6, height },
                        { 32, 1, 34 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19321), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19315), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19316), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19318), { 6, 0, height },
                        { 1, 32, 34 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19320), { 6, 0, height },
                        { 1, 32, 34 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19314), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelSquare8);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelSquare8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A511C */
static void mini_rc_track_right_banked_quarter_turn_3_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19306), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19308), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19310), { 0, 6, height },
                        { 32, 1, 34 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19312), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19307), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19309), { 6, 0, height },
                        { 1, 32, 34 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19311), { 6, 0, height },
                        { 1, 32, 34 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19313), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelSquare8);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelSquare8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A512C */
static void mini_rc_track_left_banked_quarter_turn_3_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mini_rc_track_right_banked_quarter_turn_3_25_deg_up(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A513C */
static void mini_rc_track_right_banked_quarter_turn_3_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    mini_rc_track_left_banked_quarter_turn_3_25_deg_up(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A514C */
static void mini_rc_track_left_banked_quarter_turn_5_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19286), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19291), { 0, 0, height },
                        { 32, 1, 34 }, { 0, 27, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19296), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19301), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19287), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19292), { 0, 0, height }, { 1, 1, 34 },
                        { 30, 30, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19297), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19302), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19288), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19293), { 0, 0, height }, { 1, 1, 34 },
                        { 30, 30, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19298), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19303), { 0, 0, height },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19289), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19294), { 0, 0, height }, { 1, 1, 34 },
                        { 30, 30, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19299), { 0, 0, height },
                        { 1, 32, 34 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19304), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19290), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19295), { 0, 0, height },
                        { 1, 32, 34 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19300), { 0, 0, height },
                        { 1, 32, 34 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19305), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelSquare8);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelSquare8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A515C */
static void mini_rc_track_right_banked_quarter_turn_5_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19266), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19271), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19276), { 0, 0, height },
                        { 32, 1, 34 }, { 0, 27, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19281), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19267), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19272), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19277), { 0, 0, height }, { 1, 1, 34 },
                        { 30, 30, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19282), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19268), { 0, 0, height },
                        { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19273), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19278), { 0, 0, height }, { 1, 1, 34 },
                        { 30, 30, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19283), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19269), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19274), { 0, 0, height },
                        { 1, 32, 34 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19279), { 0, 0, height }, { 1, 1, 34 },
                        { 30, 30, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19284), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19270), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19275), { 0, 0, height },
                        { 1, 32, 34 }, { 27, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19280), { 0, 0, height },
                        { 1, 32, 34 }, { 27, 0, height });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19285), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelSquare8);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelSquare8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A516C */
static void mini_rc_track_left_banked_quarter_turn_5_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mini_rc_track_right_banked_quarter_turn_5_25_deg_up(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A517C */
static void mini_rc_track_right_banked_quarter_turn_5_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    mini_rc_track_left_banked_quarter_turn_5_25_deg_up(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A518C */
static void mini_rc_track_25_deg_up_to_left_banked_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19230), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19231), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19238), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19232), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19233), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A519C */
static void mini_rc_track_25_deg_up_to_right_banked_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19234), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19235), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19236), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19239), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19237), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A51AC */
static void mini_rc_track_left_banked_25_deg_up_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19240), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19241), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19248), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19242), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19243), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A51BC */
static void mini_rc_track_right_banked_25_deg_up_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19244), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19245), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19246), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19249), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19247), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquare7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A51CC */
static void mini_rc_track_25_deg_down_to_left_banked_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_right_banked_25_deg_up_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A51DC */
static void mini_rc_track_25_deg_down_to_right_banked_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_left_banked_25_deg_up_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A51EC */
static void mini_rc_track_left_banked_25_deg_down_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_25_deg_up_to_right_banked_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A51FC */
static void mini_rc_track_right_banked_25_deg_down_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_25_deg_up_to_left_banked_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A520C */
static void mini_rc_track_left_banked_flat_to_left_banked_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19250), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19251), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19252), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19253), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A521C */
static void mini_rc_track_right_banked_flat_to_right_banked_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19254), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19255), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19256), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19257), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A524C */
static void mini_rc_track_left_banked_25_deg_up_to_left_banked_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19258), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19259), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19260), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19261), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, Tunnel14);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008A525C */
static void mini_rc_track_right_banked_25_deg_up_to_right_banked_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19262), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19263), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19264), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19265), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, Tunnel14);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008A526C */
static void mini_rc_track_left_banked_flat_to_left_banked_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_right_banked_25_deg_up_to_right_banked_flat(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A527C */
static void mini_rc_track_right_banked_flat_to_right_banked_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_left_banked_25_deg_up_to_left_banked_flat(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A522C */
static void mini_rc_track_left_banked_25_deg_down_to_left_banked_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_right_banked_flat_to_right_banked_25_deg_up(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A523C */
static void mini_rc_track_right_banked_25_deg_down_to_right_banked_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_left_banked_flat_to_left_banked_25_deg_up(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A528C */
static void mini_rc_track_flat_to_left_banked_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19202), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19203), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19210), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19204), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19205), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A529C */
static void mini_rc_track_flat_to_right_banked_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19206), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19207), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19208), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19211), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19209), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelSquare8);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A52AC */
static void mini_rc_track_left_banked_25_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19212), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19213), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19220), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19214), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19215), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, Tunnel14);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008A52BC */
static void mini_rc_track_right_banked_25_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19216), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19217), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19218), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19221), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19219), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelSquareFlat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, Tunnel14);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x008A52CC */
static void mini_rc_track_flat_to_left_banked_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_right_banked_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A52DC */
static void mini_rc_track_flat_to_right_banked_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_left_banked_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A52EC */
static void mini_rc_track_left_banked_25_deg_down_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_flat_to_right_banked_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A52FC */
static void mini_rc_track_right_banked_25_deg_down_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    mini_rc_track_flat_to_left_banked_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void mini_rc_track_left_curved_lift_hill(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19333), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19335), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19337), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19331), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19332), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19334), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19336), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19330), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 16, TunnelSquare8);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 16, TunnelSquare8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

static void mini_rc_track_right_curved_lift_hill(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19322), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19324), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19326), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19328), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19323), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19325), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19327), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, MetalSupportsForkAlt, 4, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(19329), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 16, TunnelSquare8);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 16, TunnelSquare8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

static void mini_rc_track_booster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    // These offsets could be moved to the g2.dat file when that supports offsets.
    int8_t ne_sw_offsetX = 7;
    int8_t ne_sw_offsetY = -15;
    int8_t nw_se_offsetX = -15;
    int8_t nw_se_offsetY = 7;

    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_MINI_RC_BOOSTER_NE_SW),
                { ne_sw_offsetX, ne_sw_offsetY, height }, { 32, 20, 3 }, { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsFork, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_MINI_RC_BOOSTER_NW_SE),
                { nw_se_offsetX, nw_se_offsetY, height }, { 32, 20, 3 }, { 0, 6, height });
            if (track_paint_util_should_paint_supports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, MetalSupportsForkAlt, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_mini_rc(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return mini_rc_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return mini_rc_track_station;
        case TrackElemType::Up25:
            return mini_rc_track_25_deg_up;
        case TrackElemType::Up60:
            return mini_rc_track_60_deg_up;
        case TrackElemType::FlatToUp25:
            return mini_rc_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToUp60:
            return mini_rc_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return mini_rc_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return mini_rc_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return mini_rc_track_25_deg_down;
        case TrackElemType::Down60:
            return mini_rc_track_60_deg_down;
        case TrackElemType::FlatToDown25:
            return mini_rc_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToDown60:
            return mini_rc_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return mini_rc_track_60_deg_down_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return mini_rc_track_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return mini_rc_track_left_quarter_turn_5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return mini_rc_track_right_quarter_turn_5;
        case TrackElemType::FlatToLeftBank:
            return mini_rc_track_flat_to_left_bank;
        case TrackElemType::FlatToRightBank:
            return mini_rc_track_flat_to_right_bank;
        case TrackElemType::LeftBankToFlat:
            return mini_rc_track_left_bank_to_flat;
        case TrackElemType::RightBankToFlat:
            return mini_rc_track_right_bank_to_flat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return mini_rc_track_banked_left_quarter_turn_5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return mini_rc_track_banked_right_quarter_turn_5;
        case TrackElemType::LeftBankToUp25:
            return mini_rc_track_left_bank_to_25_deg_up;
        case TrackElemType::RightBankToUp25:
            return mini_rc_track_right_bank_to_25_deg_up;
        case TrackElemType::Up25ToLeftBank:
            return mini_rc_track_25_deg_up_to_left_bank;
        case TrackElemType::Up25ToRightBank:
            return mini_rc_track_25_deg_up_to_right_bank;
        case TrackElemType::LeftBankToDown25:
            return mini_rc_track_left_bank_to_25_deg_down;
        case TrackElemType::RightBankToDown25:
            return mini_rc_track_right_bank_to_25_deg_down;
        case TrackElemType::Down25ToLeftBank:
            return mini_rc_track_25_deg_down_to_left_bank;
        case TrackElemType::Down25ToRightBank:
            return mini_rc_track_25_deg_down_to_right_bank;
        case TrackElemType::LeftBank:
            return mini_rc_track_left_bank;
        case TrackElemType::RightBank:
            return mini_rc_track_right_bank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return mini_rc_track_left_quarter_turn_5_25_deg_up;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return mini_rc_track_right_quarter_turn_5_25_deg_up;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return mini_rc_track_left_quarter_turn_5_25_deg_down;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return mini_rc_track_right_quarter_turn_5_25_deg_down;
        case TrackElemType::SBendLeft:
            return mini_rc_track_s_bend_left;
        case TrackElemType::SBendRight:
            return mini_rc_track_s_bend_right;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return mini_rc_track_left_quarter_turn_3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return mini_rc_track_right_quarter_turn_3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return mini_rc_track_left_quarter_turn_3_bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return mini_rc_track_right_quarter_turn_3_bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return mini_rc_track_left_quarter_turn_3_25_deg_up;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return mini_rc_track_right_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return mini_rc_track_left_quarter_turn_3_25_deg_down;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return mini_rc_track_right_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return mini_rc_track_left_half_banked_helix_up_small;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return mini_rc_track_right_half_banked_helix_up_small;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return mini_rc_track_left_half_banked_helix_down_small;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return mini_rc_track_right_half_banked_helix_down_small;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return mini_rc_track_left_half_banked_helix_up_large;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return mini_rc_track_right_half_banked_helix_up_large;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return mini_rc_track_left_half_banked_helix_down_large;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return mini_rc_track_right_half_banked_helix_down_large;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return mini_rc_track_left_quarter_turn_1_60_deg_up;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return mini_rc_track_right_quarter_turn_1_60_deg_up;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return mini_rc_track_left_quarter_turn_1_60_deg_down;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return mini_rc_track_right_quarter_turn_1_60_deg_down;
        case TrackElemType::Brakes:
            return mini_rc_track_brakes;
        case TrackElemType::Up25LeftBanked:
            return mini_rc_track_25_deg_up_left_banked;
        case TrackElemType::Up25RightBanked:
            return mini_rc_track_25_deg_up_right_banked;
        case TrackElemType::OnRidePhoto:
            return mini_rc_track_on_ride_photo;
        case TrackElemType::Down25LeftBanked:
            return mini_rc_track_25_deg_down_left_banked;
        case TrackElemType::Down25RightBanked:
            return mini_rc_track_25_deg_down_right_banked;
        case TrackElemType::LeftEighthToDiag:
            return mini_rc_track_left_eighth_to_diag;
        case TrackElemType::RightEighthToDiag:
            return mini_rc_track_right_eighth_to_diag;
        case TrackElemType::LeftEighthToOrthogonal:
            return mini_rc_track_left_eighth_to_orthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return mini_rc_track_right_eighth_to_orthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return mini_rc_track_left_eighth_bank_to_diag;
        case TrackElemType::RightEighthBankToDiag:
            return mini_rc_track_right_eighth_bank_to_diag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return mini_rc_track_left_eighth_bank_to_orthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return mini_rc_track_right_eighth_bank_to_orthogonal;
        case TrackElemType::DiagFlat:
            return mini_rc_track_diag_flat;
        case TrackElemType::DiagUp25:
            return mini_rc_track_diag_25_deg_up;
        case TrackElemType::DiagUp60:
            return mini_rc_track_diag_60_deg_up;
        case TrackElemType::DiagFlatToUp25:
            return mini_rc_track_diag_flat_to_25_deg_up;
        case TrackElemType::DiagUp25ToUp60:
            return mini_rc_track_diag_25_deg_up_to_60_deg_up;
        case TrackElemType::DiagUp60ToUp25:
            return mini_rc_track_diag_60_deg_up_to_25_deg_up;
        case TrackElemType::DiagUp25ToFlat:
            return mini_rc_track_diag_25_deg_up_to_flat;
        case TrackElemType::DiagDown25:
            return mini_rc_track_diag_25_deg_down;
        case TrackElemType::DiagDown60:
            return mini_rc_track_diag_60_deg_down;
        case TrackElemType::DiagFlatToDown25:
            return mini_rc_track_diag_flat_to_25_deg_down;
        case TrackElemType::DiagDown25ToDown60:
            return mini_rc_track_diag_25_deg_down_to_60_deg_down;
        case TrackElemType::DiagDown60ToDown25:
            return mini_rc_track_diag_60_deg_down_to_25_deg_down;
        case TrackElemType::DiagDown25ToFlat:
            return mini_rc_track_diag_25_deg_down_to_flat;
        case TrackElemType::DiagFlatToLeftBank:
            return mini_rc_track_diag_flat_to_left_bank;
        case TrackElemType::DiagFlatToRightBank:
            return mini_rc_track_diag_flat_to_right_bank;
        case TrackElemType::DiagLeftBankToFlat:
            return mini_rc_track_diag_left_bank_to_flat;
        case TrackElemType::DiagRightBankToFlat:
            return mini_rc_track_diag_right_bank_to_flat;
        case TrackElemType::DiagLeftBankToUp25:
            return mini_rc_track_diag_left_bank_to_25_deg_up;
        case TrackElemType::DiagRightBankToUp25:
            return mini_rc_track_diag_right_bank_to_25_deg_up;
        case TrackElemType::DiagUp25ToLeftBank:
            return mini_rc_track_diag_25_deg_up_to_left_bank;
        case TrackElemType::DiagUp25ToRightBank:
            return mini_rc_track_diag_25_deg_up_to_right_bank;
        case TrackElemType::DiagLeftBankToDown25:
            return mini_rc_track_diag_left_bank_to_25_deg_down;
        case TrackElemType::DiagRightBankToDown25:
            return mini_rc_track_diag_right_bank_to_25_deg_down;
        case TrackElemType::DiagDown25ToLeftBank:
            return mini_rc_track_diag_25_deg_down_to_left_bank;
        case TrackElemType::DiagDown25ToRightBank:
            return mini_rc_track_diag_25_deg_down_to_right_bank;
        case TrackElemType::DiagLeftBank:
            return mini_rc_track_diag_left_bank;
        case TrackElemType::DiagRightBank:
            return mini_rc_track_diag_right_bank;
        case TrackElemType::BlockBrakes:
            return mini_rc_track_block_brakes;
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
            return mini_rc_track_left_banked_quarter_turn_3_25_deg_up;
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
            return mini_rc_track_right_banked_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            return mini_rc_track_left_banked_quarter_turn_3_25_deg_down;
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
            return mini_rc_track_right_banked_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
            return mini_rc_track_left_banked_quarter_turn_5_25_deg_up;
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
            return mini_rc_track_right_banked_quarter_turn_5_25_deg_up;
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            return mini_rc_track_left_banked_quarter_turn_5_25_deg_down;
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
            return mini_rc_track_right_banked_quarter_turn_5_25_deg_down;
        case TrackElemType::Up25ToLeftBankedUp25:
            return mini_rc_track_25_deg_up_to_left_banked_25_deg_up;
        case TrackElemType::Up25ToRightBankedUp25:
            return mini_rc_track_25_deg_up_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToUp25:
            return mini_rc_track_left_banked_25_deg_up_to_25_deg_up;
        case TrackElemType::RightBankedUp25ToUp25:
            return mini_rc_track_right_banked_25_deg_up_to_25_deg_up;
        case TrackElemType::Down25ToLeftBankedDown25:
            return mini_rc_track_25_deg_down_to_left_banked_25_deg_down;
        case TrackElemType::Down25ToRightBankedDown25:
            return mini_rc_track_25_deg_down_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToDown25:
            return mini_rc_track_left_banked_25_deg_down_to_25_deg_down;
        case TrackElemType::RightBankedDown25ToDown25:
            return mini_rc_track_right_banked_25_deg_down_to_25_deg_down;
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
            return mini_rc_track_left_banked_flat_to_left_banked_25_deg_up;
        case TrackElemType::RightBankedFlatToRightBankedUp25:
            return mini_rc_track_right_banked_flat_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
            return mini_rc_track_left_banked_25_deg_up_to_left_banked_flat;
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
            return mini_rc_track_right_banked_25_deg_up_to_right_banked_flat;
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
            return mini_rc_track_left_banked_flat_to_left_banked_25_deg_down;
        case TrackElemType::RightBankedFlatToRightBankedDown25:
            return mini_rc_track_right_banked_flat_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
            return mini_rc_track_left_banked_25_deg_down_to_left_banked_flat;
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
            return mini_rc_track_right_banked_25_deg_down_to_right_banked_flat;
        case TrackElemType::FlatToLeftBankedUp25:
            return mini_rc_track_flat_to_left_banked_25_deg_up;
        case TrackElemType::FlatToRightBankedUp25:
            return mini_rc_track_flat_to_right_banked_25_deg_up;
        case TrackElemType::LeftBankedUp25ToFlat:
            return mini_rc_track_left_banked_25_deg_up_to_flat;
        case TrackElemType::RightBankedUp25ToFlat:
            return mini_rc_track_right_banked_25_deg_up_to_flat;
        case TrackElemType::FlatToLeftBankedDown25:
            return mini_rc_track_flat_to_left_banked_25_deg_down;
        case TrackElemType::FlatToRightBankedDown25:
            return mini_rc_track_flat_to_right_banked_25_deg_down;
        case TrackElemType::LeftBankedDown25ToFlat:
            return mini_rc_track_left_banked_25_deg_down_to_flat;
        case TrackElemType::RightBankedDown25ToFlat:
            return mini_rc_track_right_banked_25_deg_down_to_flat;
        case TrackElemType::LeftCurvedLiftHill:
            return mini_rc_track_left_curved_lift_hill;
        case TrackElemType::RightCurvedLiftHill:
            return mini_rc_track_right_curved_lift_hill;
        case TrackElemType::Booster:
            return mini_rc_track_booster;
    }
    return nullptr;
}
