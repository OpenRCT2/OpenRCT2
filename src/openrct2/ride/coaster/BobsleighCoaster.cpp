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

static constexpr const uint32_t BOBSLEIGH_BLOCK_BRAKE_SW_NE_OPEN = 14586;
static constexpr const uint32_t BOBSLEIGH_BLOCK_BRAKE_NW_SE_OPEN = 14587;
static constexpr const uint32_t BOBSLEIGH_BLOCK_BRAKE_SW_NE_CLOSED = 14588;
static constexpr const uint32_t BOBSLEIGH_BLOCK_BRAKE_NW_SE_CLOSED = 14589;

static constexpr const uint32_t _BobsleighBlockBrakeImages[NumOrthogonalDirections][2] = {
    { BOBSLEIGH_BLOCK_BRAKE_SW_NE_OPEN, BOBSLEIGH_BLOCK_BRAKE_SW_NE_CLOSED },
    { BOBSLEIGH_BLOCK_BRAKE_NW_SE_OPEN, BOBSLEIGH_BLOCK_BRAKE_NW_SE_CLOSED },
    { BOBSLEIGH_BLOCK_BRAKE_SW_NE_OPEN, BOBSLEIGH_BLOCK_BRAKE_SW_NE_CLOSED },
    { BOBSLEIGH_BLOCK_BRAKE_NW_SE_OPEN, BOBSLEIGH_BLOCK_BRAKE_NW_SE_CLOSED },
};

/** rct2: 0x006FE5B4 */
static void BobsleighRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14576), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14578), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14577), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14579), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14572), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14574), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14573), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14575), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void BobsleighRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { 14580, SPR_STATION_BASE_B_SW_NE },
        { 14581, SPR_STATION_BASE_B_NW_SE },
        { 14580, SPR_STATION_BASE_B_SW_NE },
        { 14581, SPR_STATION_BASE_B_NW_SE },
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]), { 0, 0, height },
        { 32, 20, 1 }, { 0, 6, height + 3 });
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][1]), { 0, 0, height },
        { 32, 32, 1 });
    TrackPaintUtilDrawStationMetalSupports2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 0);
    TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x006FE5C4 */
static void BobsleighRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14634), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14638), { 0, 0, height }, { 32, 1, 50 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14635), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14639), { 0, 0, height }, { 32, 1, 50 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14636), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14640), { 0, 0, height }, { 32, 1, 50 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14637), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14641), { 0, 0, height }, { 32, 1, 50 },
                    { 0, 27, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14610), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14614), { 0, 0, height }, { 32, 1, 50 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14611), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14615), { 0, 0, height }, { 32, 1, 50 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14612), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14616), { 0, 0, height }, { 32, 1, 50 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14613), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14617), { 0, 0, height }, { 32, 1, 50 },
                    { 0, 27, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x006FE5D4 */
static void BobsleighRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14618), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14622), { 0, 0, height }, { 32, 1, 42 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14619), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14623), { 0, 0, height }, { 32, 1, 42 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14620), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14624), { 0, 0, height }, { 32, 1, 42 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14621), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14625), { 0, 0, height }, { 32, 1, 42 },
                    { 0, 27, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14594), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14598), { 0, 0, height }, { 32, 1, 42 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14595), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14599), { 0, 0, height }, { 32, 1, 42 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14596), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14600), { 0, 0, height }, { 32, 1, 42 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14597), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14601), { 0, 0, height }, { 32, 1, 42 },
                    { 0, 27, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x006FE5E4 */
static void BobsleighRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14626), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14630), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14627), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14631), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14628), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14632), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14629), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14633), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14602), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14606), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14603), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14607), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14604), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14608), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14605), { 0, 0, height }, { 32, 20, 2 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14609), { 0, 0, height }, { 32, 1, 34 },
                    { 0, 27, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_12);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x006FE5F4 */
static void BobsleighRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    BobsleighRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x006FE604 */
static void BobsleighRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    BobsleighRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x006FE614 */
static void BobsleighRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    BobsleighRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x006FE624 */
static void BobsleighRCTrackLeftQuarterTurn5(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14707), { 0, 2, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14727), { 0, 2, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14712), { 0, 2, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14732), { 0, 2, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14717), { 0, 2, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14737), { 0, 2, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14702), { 0, 2, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14722), { 0, 2, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14706), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14726), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14711), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14731), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14716), { 0, 16, height },
                        { 32, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14736), { 0, 16, height },
                        { 32, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14701), { 0, 16, height },
                        { 32, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14721), { 0, 16, height },
                        { 32, 16, 0 }, { 0, 16, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14705), { 0, 16, height },
                        { 16, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14725), { 0, 16, height },
                        { 16, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14710), { 16, 16, height },
                        { 16, 16, 2 }, { 16, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14730), { 16, 16, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14715), { 16, 0, height },
                        { 16, 16, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14735), { 16, 0, height },
                        { 16, 16, 0 }, { 16, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14700), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14720), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14704), { 16, 0, height },
                        { 16, 34, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14724), { 16, 0, height },
                        { 16, 34, 0 }, { 16, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14709), { 0, 0, height },
                        { 16, 32, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14729), { 0, 0, height },
                        { 16, 32, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14714), { 0, 0, height },
                        { 16, 32, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14734), { 0, 0, height },
                        { 16, 32, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14699), { 16, 0, height },
                        { 16, 32, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14719), { 16, 0, height },
                        { 16, 32, 0 }, { 16, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14703), { 2, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14723), { 2, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14708), { 2, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14728), { 2, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14713), { 2, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14733), { 2, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14698), { 2, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14718), { 2, 0, height },
                        { 30, 32, 0 }, { 6, 0, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x006FE634 */
static void BobsleighRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    BobsleighRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x006FE644 */
static void BobsleighRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14642), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14646), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14643), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14647), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14644), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14648), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14645), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14649), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x006FE654 */
static void BobsleighRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14650), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14654), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14651), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14655), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14652), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14656), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14653), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14657), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x006FE664 */
static void BobsleighRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14652), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14656), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14653), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14657), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14650), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14654), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14651), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14655), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x006FE674 */
static void BobsleighRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14644), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14648), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14645), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14649), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14642), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14646), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14643), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14647), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x006FE684 */
static void BobsleighRCTrackBankedLeftQuarterTurn5(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14747), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14767), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14752), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14772), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14757), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14777), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14742), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14762), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14746), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14766), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14751), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14771), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14756), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14776), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14741), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14761), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 16, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14745), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14765), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14750), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14770), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14755), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14775), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14740), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14760), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14744), { 0, 0, height },
                        { 16, 32, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14764), { 0, 0, height },
                        { 16, 32, 0 }, { 16, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14749), { 0, 0, height },
                        { 16, 32, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14769), { 0, 0, height },
                        { 16, 32, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14754), { 0, 0, height },
                        { 16, 32, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14774), { 0, 0, height },
                        { 16, 32, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14739), { 0, 0, height },
                        { 16, 32, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14759), { 0, 0, height },
                        { 16, 32, 0 }, { 16, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14743), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14763), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14748), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14768), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14753), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14773), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14738), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14758), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x006FE694 */
static void BobsleighRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    BobsleighRCTrackBankedLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x006FE6A4 */
static void BobsleighRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14674), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14678), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14675), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14679), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14676), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14680), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14677), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14681), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x006FE6B4 */
static void BobsleighRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14682), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14686), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14683), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14687), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14684), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14688), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14685), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14689), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x006FE6C4 */
static void BobsleighRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14658), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14662), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14659), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14663), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14660), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14664), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14661), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14665), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_12);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x006FE6D4 */
static void BobsleighRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14666), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14670), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14667), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14671), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14668), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14672), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14669), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14673), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_12);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x006FE6E4 */
static void BobsleighRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    BobsleighRCTrack25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x006FE6F4 */
static void BobsleighRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    BobsleighRCTrack25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x006FE704 */
static void BobsleighRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    BobsleighRCTrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x006FE714 */
static void BobsleighRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    BobsleighRCTrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x006FE724 */
static void BobsleighRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14690), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14694), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14691), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14695), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14692), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14696), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14693), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14697), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x006FE734 */
static void BobsleighRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    BobsleighRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x006FE744 */
static void BobsleighRCTrackSBendLeft(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14826), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14842), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14830), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14846), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14829), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14845), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14833), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14849), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14827), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14843), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 0, height + 27 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14831), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14847), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 0, height + 27 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14828), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14844), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 6, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14832), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14848), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 6, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14828), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14844), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 6, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14832), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14848), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 6, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14827), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14843), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 0, height + 27 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14831), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14847), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 0, height + 27 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14829), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14845), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14833), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14849), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14826), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14842), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14830), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14846), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x006FE754 */
static void BobsleighRCTrackSBendRight(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14834), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14850), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14838), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14854), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14837), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14853), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14841), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14857), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14835), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14851), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 6, height + 27 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14839), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14855), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 6, height + 27 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14836), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14852), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14840), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14856), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14836), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14852), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14840), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14856), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14835), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14851), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 6, height + 27 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14839), { 0, 0, height },
                        { 32, 26, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14855), { 0, 0, height },
                        { 32, 26, 0 }, { 0, 6, height + 27 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14837), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14853), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14841), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14857), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14834), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14850), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14838), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14854), { 0, 0, height },
                        { 32, 27, 0 }, { 0, 2, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x006FE794 */
static void BobsleighRCTrackLeftQuarterTurn3(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14783), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14795), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14786), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14798), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14789), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14801), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14780), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14792), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14782), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14794), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14785), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14797), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14788), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14800), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14779), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14791), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14781), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14793), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14784), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14796), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14787), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14799), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14778), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14790), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x006FE7A4 */
static void BobsleighRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    BobsleighRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x006FE7B4 */
static void BobsleighRCTrackLeftQuarterTurn3Bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14807), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14819), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14810), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14822), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14813), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14825), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14804), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14816), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14806), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14818), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14809), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14821), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14812), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14824), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14803), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14815), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14805), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14817), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14808), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14820), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14811), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14823), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14802), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14814), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x006FE7C4 */
static void BobsleighRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    BobsleighRCTrackLeftQuarterTurn3Bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x006FE7D4 */
static void BobsleighRCTrackLeftHalfBankedHelixUpSmall(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14887), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14899), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14890), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14902), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14893), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14905), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14884), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height + 8 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14896), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14886), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14898), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14889), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14901), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14892), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14904), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14883), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14895), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14885), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14897), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14888), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14900), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14891), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14903), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14882), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14894), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14884), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14896), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14887), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14899), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14890), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14902), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14893), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14905), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14883), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14895), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14886), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14898), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14889), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14901), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14892), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14904), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14882), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height + 8 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14894), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14885), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14897), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14888), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14900), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14891), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14903), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x006FE7E4 */
static void BobsleighRCTrackRightHalfBankedHelixUpSmall(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14858), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14870), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14861), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14873), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14864), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14876), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14867), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14879), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14859), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14871), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14862), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14874), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14865), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14877), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14868), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14880), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14860), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height + 8 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14872), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14863), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14875), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14866), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14878), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14869), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14881), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14861), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14873), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14864), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14876), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14867), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14879), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14858), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14870), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14862), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14874), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14865), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14877), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14868), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14880), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14859), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14871), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14863), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14875), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14866), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14878), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14869), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14881), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14860), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height + 8 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14872), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x006FE7F4 */
static void BobsleighRCTrackLeftHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    BobsleighRCTrackRightHalfBankedHelixUpSmall(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x006FE804 */
static void BobsleighRCTrackRightHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    BobsleighRCTrackLeftHalfBankedHelixUpSmall(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x006FE814 */
static void BobsleighRCTrackLeftHalfBankedHelixUpLarge(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14955), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14975), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14960), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14980), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14965), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14985), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14950), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14970), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14954), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14974), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14959), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14979), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14964), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14984), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14949), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14969), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 16, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14953), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14973), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14958), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14978), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14963), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14983), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14948), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14968), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14952), { 0, 0, height },
                        { 16, 32, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14972), { 0, 0, height },
                        { 16, 32, 0 }, { 16, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14957), { 0, 0, height },
                        { 16, 32, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14977), { 0, 0, height },
                        { 16, 32, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14962), { 0, 0, height },
                        { 16, 32, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14982), { 0, 0, height },
                        { 16, 32, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14947), { 0, 0, height },
                        { 16, 32, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14967), { 0, 0, height },
                        { 16, 32, 0 }, { 16, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14951), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14971), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14956), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14976), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14961), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14981), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14946), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height + 8 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14966), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14950), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14970), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14955), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14975), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14960), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14980), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14965), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14985), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14949), { 0, 0, height },
                        { 16, 32, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14969), { 0, 0, height },
                        { 16, 32, 0 }, { 16, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14954), { 0, 0, height },
                        { 16, 32, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14974), { 0, 0, height },
                        { 16, 32, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14959), { 0, 0, height },
                        { 16, 32, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14979), { 0, 0, height },
                        { 16, 32, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14964), { 0, 0, height },
                        { 16, 32, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14984), { 0, 0, height },
                        { 16, 32, 0 }, { 16, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14948), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14968), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14953), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14973), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14958), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14978), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14963), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14983), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 16, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14947), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14967), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14952), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14972), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14957), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14977), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14962), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14982), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14946), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height + 8 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14966), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14951), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14971), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14956), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14976), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14961), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14981), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x006FE824 */
static void BobsleighRCTrackRightHalfBankedHelixUpLarge(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14906), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14926), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14911), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14931), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14916), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14936), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14921), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14941), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14907), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14927), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14912), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14932), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14917), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14937), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14922), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14942), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14908), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14928), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14913), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14933), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14918), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14938), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14923), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14943), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 16, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14909), { 0, 0, height },
                        { 16, 32, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14929), { 0, 0, height },
                        { 16, 32, 0 }, { 16, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14914), { 0, 0, height },
                        { 16, 32, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14934), { 0, 0, height },
                        { 16, 32, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14919), { 0, 0, height },
                        { 16, 32, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14939), { 0, 0, height },
                        { 16, 32, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14924), { 0, 0, height },
                        { 16, 32, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14944), { 0, 0, height },
                        { 16, 32, 0 }, { 16, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14910), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height + 8 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14930), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14915), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14935), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14920), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14940), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14925), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14945), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14911), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14931), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14916), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14936), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14921), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14941), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14906), { 0, 0, height },
                        { 20, 32, 2 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14926), { 0, 0, height },
                        { 20, 32, 0 }, { 6, 0, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14912), { 0, 0, height },
                        { 16, 32, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14932), { 0, 0, height },
                        { 16, 32, 0 }, { 16, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14917), { 0, 0, height },
                        { 16, 32, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14937), { 0, 0, height },
                        { 16, 32, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14922), { 0, 0, height },
                        { 16, 32, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14942), { 0, 0, height },
                        { 16, 32, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14907), { 0, 0, height },
                        { 16, 32, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14927), { 0, 0, height },
                        { 16, 32, 0 }, { 16, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14913), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14933), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14918), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14938), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14923), { 0, 0, height },
                        { 16, 16, 2 }, { 16, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14943), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14908), { 0, 0, height },
                        { 16, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14928), { 0, 0, height },
                        { 16, 16, 0 }, { 0, 0, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14914), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14934), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14919), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14939), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14924), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14944), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14909), { 0, 0, height },
                        { 32, 16, 2 }, { 0, 16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14929), { 0, 0, height },
                        { 32, 16, 0 }, { 0, 16, height + 27 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14915), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14935), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14920), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14940), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14925), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14945), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14910), { 0, 0, height },
                        { 32, 20, 2 }, { 0, 6, height + 8 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14930), { 0, 0, height },
                        { 32, 20, 0 }, { 0, 6, height + 27 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x006FE834 */
static void BobsleighRCTrackLeftHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    BobsleighRCTrackRightHalfBankedHelixUpLarge(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x006FE844 */
static void BobsleighRCTrackRightHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    BobsleighRCTrackLeftHalfBankedHelixUpLarge(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x006FE854 */
static void BobsleighRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14582), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14584), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14583), { 0, 0, height }, { 32, 20, 2 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14585), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x006FE864 */
static void BobsleighRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isClosed = trackElement.IsBrakeClosed();
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction,
                session.TrackColours[SCHEME_TRACK].WithIndex(_BobsleighBlockBrakeImages[direction][isClosed]), { 0, 0, height },
                { 32, 20, 2 }, { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14590), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction,
                session.TrackColours[SCHEME_TRACK].WithIndex(_BobsleighBlockBrakeImages[direction][isClosed]), { 0, 0, height },
                { 32, 20, 2 }, { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14591), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
    }
    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    }
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void BobsleighRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14572), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14574), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14573), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14575), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14572), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14574), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });

            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14573), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(14575), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });

            break;
    }
    TrackPaintUtilOnridePhotoPaint(session, direction, height + 3, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionBobsleighRC(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return BobsleighRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return BobsleighRCTrackStation;
        case TrackElemType::Up25:
            return BobsleighRCTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return BobsleighRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return BobsleighRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return BobsleighRCTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return BobsleighRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return BobsleighRCTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return BobsleighRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return BobsleighRCTrackRightQuarterTurn5;
        case TrackElemType::FlatToLeftBank:
            return BobsleighRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return BobsleighRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return BobsleighRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return BobsleighRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return BobsleighRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return BobsleighRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankToUp25:
            return BobsleighRCTrackLeftBankTo25DegUp;
        case TrackElemType::RightBankToUp25:
            return BobsleighRCTrackRightBankTo25DegUp;
        case TrackElemType::Up25ToLeftBank:
            return BobsleighRCTrack25DegUpToLeftBank;
        case TrackElemType::Up25ToRightBank:
            return BobsleighRCTrack25DegUpToRightBank;
        case TrackElemType::LeftBankToDown25:
            return BobsleighRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return BobsleighRCTrackRightBankTo25DegDown;
        case TrackElemType::Down25ToLeftBank:
            return BobsleighRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return BobsleighRCTrack25DegDownToRightBank;
        case TrackElemType::LeftBank:
            return BobsleighRCTrackLeftBank;
        case TrackElemType::RightBank:
            return BobsleighRCTrackRightBank;
        case TrackElemType::SBendLeft:
            return BobsleighRCTrackSBendLeft;
        case TrackElemType::SBendRight:
            return BobsleighRCTrackSBendRight;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return BobsleighRCTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return BobsleighRCTrackRightQuarterTurn3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return BobsleighRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return BobsleighRCTrackRightQuarterTurn3Bank;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return BobsleighRCTrackLeftHalfBankedHelixUpSmall;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return BobsleighRCTrackRightHalfBankedHelixUpSmall;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return BobsleighRCTrackLeftHalfBankedHelixDownSmall;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return BobsleighRCTrackRightHalfBankedHelixDownSmall;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return BobsleighRCTrackLeftHalfBankedHelixUpLarge;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return BobsleighRCTrackRightHalfBankedHelixUpLarge;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return BobsleighRCTrackLeftHalfBankedHelixDownLarge;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return BobsleighRCTrackRightHalfBankedHelixDownLarge;
        case TrackElemType::Brakes:
            return BobsleighRCTrackBrakes;
        case TrackElemType::BlockBrakes:
            return BobsleighRCTrackBlockBrakes;

        case TrackElemType::OnRidePhoto:
            return BobsleighRCTrackOnRidePhoto;
    }
    return nullptr;
}
