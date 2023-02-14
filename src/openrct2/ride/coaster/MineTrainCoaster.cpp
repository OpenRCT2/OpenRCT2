/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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

static constexpr const uint32_t MINE_TRAIN_BLOCK_BRAKE_SW_NE_OPEN = 20060;
static constexpr const uint32_t MINE_TRAIN_BLOCK_BRAKE_NW_SE_OPEN = 20061;
static constexpr const uint32_t MINE_TRAIN_BLOCK_BRAKE_SW_NE_CLOSED = 20062;
static constexpr const uint32_t MINE_TRAIN_BLOCK_BRAKE_NW_SE_CLOSED = 20063;

static constexpr const uint32_t _MineTrainBlockBrakeImages[NumOrthogonalDirections][2] = {
    { MINE_TRAIN_BLOCK_BRAKE_SW_NE_OPEN, MINE_TRAIN_BLOCK_BRAKE_SW_NE_CLOSED },
    { MINE_TRAIN_BLOCK_BRAKE_NW_SE_OPEN, MINE_TRAIN_BLOCK_BRAKE_NW_SE_CLOSED },
    { MINE_TRAIN_BLOCK_BRAKE_SW_NE_OPEN, MINE_TRAIN_BLOCK_BRAKE_SW_NE_CLOSED },
    { MINE_TRAIN_BLOCK_BRAKE_NW_SE_OPEN, MINE_TRAIN_BLOCK_BRAKE_NW_SE_CLOSED },
};

/** rct2: 0x0071BFA4 */
static void MineTrainRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20054), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20055), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20056), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20057), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20052), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20053), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0071C154, 0x0071C164, 0x0071C174 */
static void MineTrainRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { 20064, SPR_STATION_BASE_B_SW_NE },
        { 20065, SPR_STATION_BASE_B_NW_SE },
        { 20064, SPR_STATION_BASE_B_SW_NE },
        { 20065, SPR_STATION_BASE_B_NW_SE },
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][1]), { 0, 0, height - 2 },
        { { 0, 2, height }, { 32, 28, 1 } });
    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        bool isClosed = trackElement.IsBrakeClosed();
        PaintAddImageAsChildRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_MineTrainBlockBrakeImages[direction][isClosed]),
            { 0, 0, height }, { 32, 20, 1 }, { 0, 0, height });
    }
    else
    {
        PaintAddImageAsChildRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]), { 0, 0, height },
            { 32, 20, 1 }, { 0, 0, height });
    }
    TrackPaintUtilDrawStationMetalSupports2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 3);
    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0071BFB4 */
static void MineTrainRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20102), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20103), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20104), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20105), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20074), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20075), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20076), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20077), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x0071BFC4 */
static void MineTrainRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20090), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20091), { 0, 0, height },
                { { 27, 0, height }, { 1, 32, 98 } });
            WoodenASupportsPaintSetup(session, 7, 22, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20092), { 0, 0, height },
                { { 27, 0, height }, { 1, 32, 98 } });
            WoodenASupportsPaintSetup(session, 6, 23, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20093), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x0071BFD4 */
static void MineTrainRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20094), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20095), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20096), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20097), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20066), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20067), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20068), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20069), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x0071BFE4 */
static void MineTrainRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20078), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20079), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20082), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 66 } });
            WoodenASupportsPaintSetup(session, 7, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20080), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20083), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 66 } });
            WoodenASupportsPaintSetup(session, 6, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20081), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x0071BFF4 */
static void MineTrainRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20084), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20085), { 0, 0, height },
                { { 4, 29, height - 16 }, { 24, 1, 61 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20088), { 0, 0, height },
                { { 0, 4, height }, { 32, 2, 66 } });
            WoodenASupportsPaintSetup(session, 7, 18, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20086), { 0, 0, height },
                { { 4, 29, height - 16 }, { 24, 1, 61 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20089), { 0, 0, height },
                { { 0, 4, height }, { 32, 2, 66 } });
            WoodenASupportsPaintSetup(session, 6, 19, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20087), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x0071C004 */
static void MineTrainRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20098), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20099), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20100), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20101), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20070), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20071), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20072), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 6, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20073), { 0, 0, height },
                    { { 0, 6, height }, { 32, 20, 1 } });
                WoodenASupportsPaintSetup(session, 7, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x0071C014 */
static void MineTrainRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    MineTrainRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0071C024 */
static void MineTrainRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    MineTrainRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0071C034 */
static void MineTrainRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    MineTrainRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0071C044 */
static void MineTrainRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    MineTrainRCTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0071C054 */
static void MineTrainRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    MineTrainRCTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0071C064 */
static void MineTrainRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    MineTrainRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0071C0B4 */
static void MineTrainRCTrackLeftQuarterTurn5(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20155), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20160), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20165), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20150), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20154), { 0, 0, height },
                        { 32, 16, 1 });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20159), { 0, 0, height },
                        { 32, 16, 1 });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20164), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20149), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20153), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20158), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20163), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20148), { 0, 0, height },
                        { 16, 16, 1 });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20152), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20157), { 0, 0, height },
                        { 16, 32, 1 });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20162), { 0, 0, height },
                        { 16, 32, 1 });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20147), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20151), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20156), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20161), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20146), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C0C4 */
static void MineTrainRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x0071C074 */
static void MineTrainRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20106), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20114), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20107), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20115), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20108), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20109), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0071C084 */
static void MineTrainRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20110), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20111), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20112), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20116), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20113), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20117), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0071C094 */
static void MineTrainRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20112), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20116), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20113), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20117), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20110), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20111), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0071C0A4 */
static void MineTrainRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20108), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20109), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20106), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20114), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20107), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20115), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0071C0D4 */
static void MineTrainRCTrackBankedLeftQuarterTurn5(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20175), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20186), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20180), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20185), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20170), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20174), { 0, 0, height },
                        { 32, 16, 1 });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20179), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20184), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20169), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20173), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20178), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20183), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20168), { 0, 0, height },
                        { 16, 16, 1 });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20172), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20177), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20182), { 0, 0, height },
                        { 16, 32, 1 });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20167), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20171), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20176), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20181), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20187), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20166), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C0E4 */
static void MineTrainRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackBankedLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x0071C204 */
static void MineTrainRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20118), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20122), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            WoodenASupportsPaintSetup(session, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20119), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20123), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            WoodenASupportsPaintSetup(session, 7, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20120), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20121), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x0071C214 */
static void MineTrainRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20124), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20125), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20126), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20128), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            WoodenASupportsPaintSetup(session, 6, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20127), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20129), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            WoodenASupportsPaintSetup(session, 7, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_8);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x0071C224 */
static void MineTrainRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20130), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20134), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            WoodenASupportsPaintSetup(session, 6, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20131), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20135), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            WoodenASupportsPaintSetup(session, 7, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20132), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20133), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x0071C234 */
static void MineTrainRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20136), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20137), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20138), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20140), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            WoodenASupportsPaintSetup(session, 6, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20139), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20141), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 34 } });
            WoodenASupportsPaintSetup(session, 7, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_14);
    }
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x0071C244 */
static void MineTrainRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    MineTrainRCTrack25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0071C254 */
static void MineTrainRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    MineTrainRCTrack25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0071C264 */
static void MineTrainRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    MineTrainRCTrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0071C274 */
static void MineTrainRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    MineTrainRCTrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0071C304 */
static void MineTrainRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20142), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20143), { 0, 0, height },
                { { 0, 27, height }, { 32, 1, 26 } });
            WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20144), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20145), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0071C314 */
static void MineTrainRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    MineTrainRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0071C0F4 */
static void MineTrainRCTrackLeftQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20250), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20255), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20260), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20265), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20251), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20256), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20261), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20266), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20252), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20257), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20262), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20267), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20253), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20258), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20263), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20268), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20254), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20259), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20264), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20269), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x0071C104 */
static void MineTrainRCTrackRightQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20230), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20235), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20240), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20245), { 0, 0, height },
                        { { 0, 2, height }, { 32, 27, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20231), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20236), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20241), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20246), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20232), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20237), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20242), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20247), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4,
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20233), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20238), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20243), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20248), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20234), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20239), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20244), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20249), { 0, 0, height },
                        { { 2, 0, height }, { 27, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x0071C114 */
static void MineTrainRCTrackLeftQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackRightQuarterTurn525DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x0071C124 */
static void MineTrainRCTrackRightQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn525DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x0071C134 */
static void MineTrainRCTrackSBendLeft(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20270), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20274), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20273), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20277), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20271), { 0, 0, height },
                        { 32, 26, 1 });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20275), { 0, 0, height },
                        { 32, 26, 1 });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20272), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20276), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20272), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20276), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20271), { 0, 0, height },
                        { 32, 26, 1 });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20275), { 0, 0, height },
                        { 32, 26, 1 });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20273), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20277), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20270), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20274), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C144 */
static void MineTrainRCTrackSBendRight(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20278), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20282), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20281), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20285), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20279), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20283), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20280), { 0, 0, height },
                        { 32, 26, 1 });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20284), { 0, 0, height },
                        { 32, 26, 1 });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20280), { 0, 0, height },
                        { 32, 26, 1 });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20284), { 0, 0, height },
                        { 32, 26, 1 });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20279), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20283), { 0, 0, height },
                        { { 0, 6, height }, { 32, 26, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20281), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20285), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20278), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20282), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C184 */
static void MineTrainRCTrackLeftQuarterTurn3(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20193), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20196), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20199), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20190), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20192), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20195), { 0, 0, height },
                        { 16, 16, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20198), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20189), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20191), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20194), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20197), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20188), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C194 */
static void MineTrainRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x0071C1A4 */
static void MineTrainRCTrackLeftQuarterTurn3Bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20205), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20212), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20208), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20211), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20202), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20204), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20207), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20210), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20201), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20203), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20206), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20209), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20213), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20200), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C1B4 */
static void MineTrainRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn3Bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x0071C1C4 */
static void MineTrainRCTrackLeftQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20225), { 0, 6, height },
                        { 32, 20, 1 });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20227), { 0, 6, height },
                        { 32, 20, 1 });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20229), { 0, 6, height },
                        { 32, 20, 1 });
                    WoodenASupportsPaintSetup(session, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20223), { 0, 6, height },
                        { 32, 20, 1 });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20224), { 6, 0, height },
                        { 20, 32, 1 });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20226), { 6, 0, height },
                        { 20, 32, 1 });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20228), { 6, 0, height },
                        { 20, 32, 1 });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20222), { 6, 0, height },
                        { 20, 32, 1 });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x0071C1D4 */
static void MineTrainRCTrackRightQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20214), { 0, 6, height },
                        { 32, 20, 1 });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20216), { 0, 6, height },
                        { 32, 20, 1 });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20218), { 0, 6, height },
                        { 32, 20, 1 });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20220), { 0, 6, height },
                        { 32, 20, 1 });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20215), { 6, 0, height },
                        { 20, 32, 1 });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20217), { 6, 0, height },
                        { 20, 32, 1 });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20219), { 6, 0, height },
                        { 20, 32, 1 });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20221), { 6, 0, height },
                        { 20, 32, 1 });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_8);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x0071C1E4 */
static void MineTrainRCTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackRightQuarterTurn325DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x0071C1F4 */
static void MineTrainRCTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackLeftQuarterTurn325DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x0071C284 */
static void MineTrainRCTrackLeftHalfBankedHelixUpSmall(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20305), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20312), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20308), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20311), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20302), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_D4, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20304), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20307), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20310), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20301), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20303), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20306), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20309), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20313), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20300), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_CC, direction), 48, 0x20);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20302), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20305), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20312), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20308), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20311), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20301), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20304), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20307), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20310), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20300), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20303), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20306), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20309), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20313), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C294 */
static void MineTrainRCTrackRightHalfBankedHelixUpSmall(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20286), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20289), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20292), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20295), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20299), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20287), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20290), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20293), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20296), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20288), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20291), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20298), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20294), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20297), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 48, 0x20);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20289), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20292), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20295), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20299), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20286), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_CC, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20290), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20293), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20296), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20287), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20291), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20298), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20294), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20297), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20288), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_D4, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C2A4 */
static void MineTrainRCTrackLeftHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackRightHalfBankedHelixUpSmall(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x0071C2B4 */
static void MineTrainRCTrackRightHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MineTrainRCTrackLeftHalfBankedHelixUpSmall(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x0071C2C4 */
static void MineTrainRCTrackLeftHalfBankedHelixUpLarge(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20345), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20356), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20350), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20355), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20340), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_D4, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20344), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20349), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20354), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20339), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20343), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20348), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20178), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20353), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20338), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_D0, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20342), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20347), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20177), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20352), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20337), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20341), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20346), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20351), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20357), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20336), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_CC, direction), 48, 0x20);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20340), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20345), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20356), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20350), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20355), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 8:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20339), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20344), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20349), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20354), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20338), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20343), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20348), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20178), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20353), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 11:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20337), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20342), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20347), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20177), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20352), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20336), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20341), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20346), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20351), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20357), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C2D4 */
static void MineTrainRCTrackRightHalfBankedHelixUpLarge(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20314), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20319), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20324), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20329), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20335), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C8, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20315), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20320), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20325), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20330), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20316), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20321), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20326), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20331), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_D4, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC, direction), 48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20317), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20322), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20327), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20332), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20318), { 0, 0, height },
                        { { 6, 0, height + 8 }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20323), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20334), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20328), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20333), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_CC, direction), 48, 0x20);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20319), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20324), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20329), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20335), { 0, 0, height },
                        { { 27, 0, height }, { 1, 32, 26 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20314), { 0, 0, height },
                        { { 6, 0, height }, { 20, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_CC, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 8:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20320), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20325), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20330), { 0, 0, height },
                        { { 0, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20315), { 0, 0, height },
                        { { 16, 0, height }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20321), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20326), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20331), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20316), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_D0, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 11:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_CC | SEGMENT_D4, direction), 48, 0x20);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 12:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20322), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20327), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20332), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20317), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20323), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20334), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20328), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20333), { 0, 0, height },
                        { { 0, 6, height }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20318), { 0, 0, height },
                        { { 0, 6, height + 8 }, { 32, 20, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_D4, direction), 48, 0x20);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C2E4 */
static void MineTrainRCTrackLeftHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackRightHalfBankedHelixUpLarge(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x0071C2F4 */
static void MineTrainRCTrackRightHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MineTrainRCTrackLeftHalfBankedHelixUpLarge(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x0071C324 */
static void MineTrainRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20058), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20059), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x0071C334 */
static void MineTrainRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20052), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20053), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20052), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20053), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 1 } });
            WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }
    TrackPaintUtilOnridePhotoPaint(session, direction, height + 2, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x0071C354 */
static void MineTrainRCTrackLeftEighthToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20452), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20456), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20460), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20464), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20453), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20457), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20461), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20465), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20454), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20458), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20462), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20466), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20455), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20459), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20463), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20467), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C364 */
static void MineTrainRCTrackRightEighthToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20436), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20440), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20444), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20448), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20437), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20441), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20445), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20449), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20438), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20442), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20446), { 0, 0, height },
                        { { 4, 4, height }, { 28, 28, 1 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20450), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20439), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20443), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20447), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20451), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C374 */
static void MineTrainRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MineTrainRCTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0071C384 */
static void MineTrainRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MineTrainRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x0071C394 */
static void MineTrainRCTrackLeftEighthBankToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20484), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20488), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20492), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20496), { 0, 0, height },
                        { { 0, 27, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20485), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20489), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 34, 16, 0 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20493), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20497), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20486), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20490), { 0, 0, height },
                        { { 16, 16, height + 27 }, { 16, 16, 0 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20494), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20498), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20487), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20491), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 16, 18, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20495), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20499), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C3A4 */
static void MineTrainRCTrackRightEighthBankToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20468), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20472), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20476), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20480), { 0, 0, height },
                        { { 0, 27, height }, { 32, 1, 26 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20469), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20473), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20477), { 0, 0, height },
                        { { 0, 0, height + 27 }, { 34, 16, 0 } });
                    WoodenASupportsPaintSetup(session, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20481), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20470), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20474), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20478), { 0, 0, height },
                        { { 4, 4, height + 27 }, { 28, 28, 0 } });
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20482), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20471), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20475), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20479), { 0, 0, height },
                        { { 0, 16, height + 27 }, { 16, 18, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20483), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C3B4 */
static void MineTrainRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MineTrainRCTrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x0071C3C4 */
static void MineTrainRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MineTrainRCTrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x0071C344 */
static void MineTrainRCTrackDiagFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20389), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20361), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20386), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20358), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20388), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20360), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20387), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20359), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C414 */
static void MineTrainRCTrackDiag25DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20401), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20373), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20398), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20370), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20400), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20372), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20399), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20371), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x0071C474 */
static void MineTrainRCTrackDiag60DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20385), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20382), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20384), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20383), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x0071C3F4 */
static void MineTrainRCTrackDiagFlatTo25DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20393), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20365), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20390), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20362), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20392), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20364), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20391), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20363), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x0071C454 */
static void MineTrainRCTrackDiag25DegUpTo60DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20377), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20374), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20376), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20375), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x0071C464 */
static void MineTrainRCTrackDiag60DegUpTo25DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20381), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20378), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20380), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20379), { -16, -16, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x0071C404 */
static void MineTrainRCTrackDiag25DegUpToFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20397), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20369), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20394), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20366), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20396), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20368), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20395), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20367), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x0071C444 */
static void MineTrainRCTrackDiag25DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20399), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20371), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20400), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20372), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20398), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20370), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20401), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20373), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x0071C4A4 */
static void MineTrainRCTrackDiag60DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20383), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20384), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20382), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20385), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x0071C424 */
static void MineTrainRCTrackDiagFlatTo25DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20395), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20367), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20396), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20368), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20394), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20366), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20397), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20369), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x0071C484 */
static void MineTrainRCTrackDiag25DegDownTo60DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20379), { -16, -16, height },
                        { { 0, 0, height }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20380), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20378), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20381), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x0071C494 */
static void MineTrainRCTrackDiag60DegDownTo25DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20375), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20376), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20374), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20377), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x0071C434 */
static void MineTrainRCTrackDiag25DegDownToFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20391), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20363), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20392), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20364), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20390), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20362), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20393), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20365), { -16, -16, height },
                            { { -16, -16, height }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x0071C4D4 */
static void MineTrainRCTrackDiagFlatToLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20409), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20406), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20410), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20408), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20407), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C4B4 */
static void MineTrainRCTrackDiagFlatToRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20414), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20411), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20413), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20415), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20412), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C4C4 */
static void MineTrainRCTrackDiagLeftBankToFlat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20412), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20413), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20415), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20411), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20414), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C4E4 */
static void MineTrainRCTrackDiagRightBankToFlat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20407), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20408), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20406), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20410), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20409), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C514 */
static void MineTrainRCTrackDiagLeftBankTo25DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20429), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20426), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20430), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20428), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20427), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x0071C524 */
static void MineTrainRCTrackDiagRightBankTo25DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20434), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20431), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20433), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20435), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20432), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x0071C4F4 */
static void MineTrainRCTrackDiag25DegUpToLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20419), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20416), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20420), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20418), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20417), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x0071C504 */
static void MineTrainRCTrackDiag25DegUpToRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20424), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20421), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20423), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20425), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20422), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x0071C534 */
static void MineTrainRCTrackDiagLeftBankTo25DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20422), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20423), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20425), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20421), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20424), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x0071C544 */
static void MineTrainRCTrackDiagRightBankTo25DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20417), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20418), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenBSupportsPaintSetup(session, 10, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenBSupportsPaintSetup(session, 11, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20416), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20420), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    WoodenBSupportsPaintSetup(session, 8, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenBSupportsPaintSetup(session, 9, 0, height + 16, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20419), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x0071C554 */
static void MineTrainRCTrackDiag25DegDownToLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20432), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20433), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20435), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20431), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20434), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x0071C564 */
static void MineTrainRCTrackDiag25DegDownToRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20427), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20428), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20426), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20430), { -16, -16, height },
                        { { -16, -16, height + 35 }, { 32, 32, 0 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20429), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x0071C3D4 */
static void MineTrainRCTrackDiagLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20405), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20402), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20404), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20403), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C3E4 */
static void MineTrainRCTrackDiagRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20403), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20404), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    WoodenASupportsPaintSetup(session, 10, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    WoodenASupportsPaintSetup(session, 11, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20402), { -16, -16, height },
                        { { -16, -16, height + 27 }, { 32, 32, 0 } });
                    WoodenASupportsPaintSetup(session, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    WoodenASupportsPaintSetup(session, 9, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(20405), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x0071C574 */
static void MineTrainRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isClosed = trackElement.IsBrakeClosed();
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_MineTrainBlockBrakeImages[direction][isClosed]),
        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
    WoodenASupportsPaintSetup(session, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionMineTrainRC(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return MineTrainRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return MineTrainRCTrackStation;
        case TrackElemType::Up25:
            return MineTrainRCTrack25DegUp;
        case TrackElemType::Up60:
            return MineTrainRCTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return MineTrainRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return MineTrainRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return MineTrainRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return MineTrainRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return MineTrainRCTrack25DegDown;
        case TrackElemType::Down60:
            return MineTrainRCTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return MineTrainRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return MineTrainRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return MineTrainRCTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return MineTrainRCTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return MineTrainRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return MineTrainRCTrackRightQuarterTurn5;
        case TrackElemType::FlatToLeftBank:
            return MineTrainRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return MineTrainRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return MineTrainRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return MineTrainRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return MineTrainRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return MineTrainRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankToUp25:
            return MineTrainRCTrackLeftBankTo25DegUp;
        case TrackElemType::RightBankToUp25:
            return MineTrainRCTrackRightBankTo25DegUp;
        case TrackElemType::Up25ToLeftBank:
            return MineTrainRCTrack25DegUpToLeftBank;
        case TrackElemType::Up25ToRightBank:
            return MineTrainRCTrack25DegUpToRightBank;
        case TrackElemType::LeftBankToDown25:
            return MineTrainRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return MineTrainRCTrackRightBankTo25DegDown;
        case TrackElemType::Down25ToLeftBank:
            return MineTrainRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return MineTrainRCTrack25DegDownToRightBank;
        case TrackElemType::LeftBank:
            return MineTrainRCTrackLeftBank;
        case TrackElemType::RightBank:
            return MineTrainRCTrackRightBank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return MineTrainRCTrackLeftQuarterTurn525DegUp;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return MineTrainRCTrackRightQuarterTurn525DegUp;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return MineTrainRCTrackLeftQuarterTurn525DegDown;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return MineTrainRCTrackRightQuarterTurn525DegDown;
        case TrackElemType::SBendLeft:
            return MineTrainRCTrackSBendLeft;
        case TrackElemType::SBendRight:
            return MineTrainRCTrackSBendRight;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return MineTrainRCTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return MineTrainRCTrackRightQuarterTurn3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return MineTrainRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return MineTrainRCTrackRightQuarterTurn3Bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return MineTrainRCTrackLeftQuarterTurn325DegUp;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return MineTrainRCTrackRightQuarterTurn325DegUp;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return MineTrainRCTrackLeftQuarterTurn325DegDown;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return MineTrainRCTrackRightQuarterTurn325DegDown;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return MineTrainRCTrackLeftHalfBankedHelixUpSmall;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return MineTrainRCTrackRightHalfBankedHelixUpSmall;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return MineTrainRCTrackLeftHalfBankedHelixDownSmall;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return MineTrainRCTrackRightHalfBankedHelixDownSmall;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return MineTrainRCTrackLeftHalfBankedHelixUpLarge;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return MineTrainRCTrackRightHalfBankedHelixUpLarge;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return MineTrainRCTrackLeftHalfBankedHelixDownLarge;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return MineTrainRCTrackRightHalfBankedHelixDownLarge;
        case TrackElemType::Brakes:
            return MineTrainRCTrackBrakes;
        case TrackElemType::OnRidePhoto:
            return MineTrainRCTrackOnRidePhoto;
        case TrackElemType::LeftEighthToDiag:
            return MineTrainRCTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return MineTrainRCTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return MineTrainRCTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return MineTrainRCTrackRightEighthToOrthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return MineTrainRCTrackLeftEighthBankToDiag;
        case TrackElemType::RightEighthBankToDiag:
            return MineTrainRCTrackRightEighthBankToDiag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return MineTrainRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return MineTrainRCTrackRightEighthBankToOrthogonal;
        case TrackElemType::DiagFlat:
            return MineTrainRCTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return MineTrainRCTrackDiag25DegUp;
        case TrackElemType::DiagUp60:
            return MineTrainRCTrackDiag60DegUp;
        case TrackElemType::DiagFlatToUp25:
            return MineTrainRCTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToUp60:
            return MineTrainRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::DiagUp60ToUp25:
            return MineTrainRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return MineTrainRCTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return MineTrainRCTrackDiag25DegDown;
        case TrackElemType::DiagDown60:
            return MineTrainRCTrackDiag60DegDown;
        case TrackElemType::DiagFlatToDown25:
            return MineTrainRCTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToDown60:
            return MineTrainRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::DiagDown60ToDown25:
            return MineTrainRCTrackDiag60DegDownTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return MineTrainRCTrackDiag25DegDownToFlat;
        case TrackElemType::DiagFlatToLeftBank:
            return MineTrainRCTrackDiagFlatToLeftBank;
        case TrackElemType::DiagFlatToRightBank:
            return MineTrainRCTrackDiagFlatToRightBank;
        case TrackElemType::DiagLeftBankToFlat:
            return MineTrainRCTrackDiagLeftBankToFlat;
        case TrackElemType::DiagRightBankToFlat:
            return MineTrainRCTrackDiagRightBankToFlat;
        case TrackElemType::DiagLeftBankToUp25:
            return MineTrainRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::DiagRightBankToUp25:
            return MineTrainRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::DiagUp25ToLeftBank:
            return MineTrainRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::DiagUp25ToRightBank:
            return MineTrainRCTrackDiag25DegUpToRightBank;
        case TrackElemType::DiagLeftBankToDown25:
            return MineTrainRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::DiagRightBankToDown25:
            return MineTrainRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::DiagDown25ToLeftBank:
            return MineTrainRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::DiagDown25ToRightBank:
            return MineTrainRCTrackDiag25DegDownToRightBank;
        case TrackElemType::DiagLeftBank:
            return MineTrainRCTrackDiagLeftBank;
        case TrackElemType::DiagRightBank:
            return MineTrainRCTrackDiagRightBank;
        case TrackElemType::BlockBrakes:
            return MineTrainRCTrackBlockBrakes;
    }
    return nullptr;
}
