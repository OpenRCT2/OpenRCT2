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

static constexpr const uint32_t CORKSCREW_RC_BLOCK_BRAKE_SW_NE_OPEN = 16232;
static constexpr const uint32_t CORKSCREW_RC_BLOCK_BRAKE_NW_SE_OPEN = 16233;
static constexpr const uint32_t CORKSCREW_RC_BLOCK_BRAKE_SW_NE_CLOSED = 16234;
static constexpr const uint32_t CORKSCREW_RC_BLOCK_BRAKE_NW_SE_CLOSED = 16235;

static constexpr const uint32_t _CorkscrewRCBlockBrakeImages[NumOrthogonalDirections][2] = {
    { CORKSCREW_RC_BLOCK_BRAKE_SW_NE_OPEN, CORKSCREW_RC_BLOCK_BRAKE_SW_NE_CLOSED },
    { CORKSCREW_RC_BLOCK_BRAKE_NW_SE_OPEN, CORKSCREW_RC_BLOCK_BRAKE_NW_SE_CLOSED },
    { CORKSCREW_RC_BLOCK_BRAKE_SW_NE_OPEN, CORKSCREW_RC_BLOCK_BRAKE_SW_NE_CLOSED },
    { CORKSCREW_RC_BLOCK_BRAKE_NW_SE_OPEN, CORKSCREW_RC_BLOCK_BRAKE_NW_SE_CLOSED },
};

/** rct2: 0x008A7AF8 */
static void CorkscrewRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16226), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16227), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16228), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16229), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
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
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16224), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16225), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
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

/** rct2: 0x008A7D68, 0x008A7D78, 0x008A7D88 */
static void CorkscrewRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][2] = {
        { 16236, SPR_STATION_BASE_A_SW_NE },
        { 16237, SPR_STATION_BASE_A_NW_SE },
        { 16236, SPR_STATION_BASE_A_SW_NE },
        { 16237, SPR_STATION_BASE_A_NW_SE },
    };

    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        bool isClosed = trackElement.IsBrakeClosed();
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_CorkscrewRCBlockBrakeImages[direction][isClosed]),
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
    TrackPaintUtilDrawStationMetalSupports2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 0);
    TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, 9, 11);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x008A7B08 */
static void CorkscrewRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16314), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16315), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16316), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16317), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
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
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16286), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16287), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16288), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16289), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
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

/** rct2: 0x008A7B18 */
static void CorkscrewRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16330), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16331), { 0, 0, height }, { 32, 1, 98 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16332), { 0, 0, height }, { 32, 1, 98 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16333), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16302), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16303), { 0, 0, height }, { 32, 1, 98 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16304), { 0, 0, height }, { 32, 1, 98 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16305), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008A7B28 */
static void CorkscrewRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16306), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16307), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16308), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16309), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
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
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16278), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16279), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16280), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16281), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
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

/** rct2: 0x008A7B38 */
static void CorkscrewRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16318), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16319), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16322), { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16320), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16323), { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16321), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16290), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16291), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16294), { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16292), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16295), { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16293), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A7B48 */
static void CorkscrewRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16324), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16325), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16328), { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16326), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16329), { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16327), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16296), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16297), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16300), { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16298), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16301), { 0, 0, height }, { 32, 1, 66 },
                    { 0, 27, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16299), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_2);
    }
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008A7B58 */
static void CorkscrewRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16310), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16311), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16312), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16313), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
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
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16282), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16283), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16284), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16285), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
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

/** rct2: 0x008A7B68 */
static void CorkscrewRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7B78 */
static void CorkscrewRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7B88 */
static void CorkscrewRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7B98 */
static void CorkscrewRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrack60DegUpTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7BA8 */
static void CorkscrewRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrack25DegUpTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7BB8 */
static void CorkscrewRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7BC8 */
static void CorkscrewRCTrackLeftQuarterTurn5(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16343), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16348), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16353), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16338), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16342), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16347), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16352), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16337), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16341), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16346), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16351), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16336), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16340), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16345), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16350), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16335), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16339), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16344), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16349), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16334), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
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

/** rct2: 0x008A7BD8 */
static void CorkscrewRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    CorkscrewRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7BE8 */
static void CorkscrewRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16238), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16246), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16239), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16247), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16240), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16241), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
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

/** rct2: 0x008A7BF8 */
static void CorkscrewRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16242), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16243), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16244), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16248), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16245), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16249), { 0, 0, height }, { 32, 1, 26 },
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

/** rct2: 0x008A7C08 */
static void CorkscrewRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16244), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16248), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16245), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16249), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16242), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16243), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
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

/** rct2: 0x008A7C18 */
static void CorkscrewRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16240), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16241), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16238), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16246), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16239), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16247), { 0, 0, height }, { 32, 1, 26 },
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

/** rct2: 0x008A7C28 */
static void CorkscrewRCTrackBankedLeftQuarterTurn5(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16363), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16374), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16368), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16373), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16358), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16362), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16367), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16372), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16357), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16361), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16366), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16371), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16356), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16360), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16365), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16370), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16355), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16359), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16364), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16369), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16375), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16354), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
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

/** rct2: 0x008A7C38 */
static void CorkscrewRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    CorkscrewRCTrackBankedLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7C48 */
static void CorkscrewRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16250), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16254), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16251), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16255), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16252), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16253), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
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

/** rct2: 0x008A7C58 */
static void CorkscrewRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16256), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16257), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16258), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16260), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16259), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16261), { 0, 0, height }, { 32, 1, 34 },
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

/** rct2: 0x008A7C68 */
static void CorkscrewRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16262), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16266), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16263), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16267), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16264), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16265), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
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

/** rct2: 0x008A7C78 */
static void CorkscrewRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16268), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16269), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16270), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16272), { 0, 0, height }, { 32, 1, 34 },
                { 0, 27, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16271), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16273), { 0, 0, height }, { 32, 1, 34 },
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

/** rct2: 0x008A7C88 */
static void CorkscrewRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrack25DegUpToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7C98 */
static void CorkscrewRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrack25DegUpToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7CA8 */
static void CorkscrewRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrackRightBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7CB8 */
static void CorkscrewRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrackLeftBankTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7CC8 */
static void CorkscrewRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16274), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16275), { 0, 0, height }, { 32, 1, 26 },
                { 0, 27, height });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16276), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16277), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
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

/** rct2: 0x008A7CD8 */
static void CorkscrewRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7CE8 */
static void CorkscrewRCTrackLeftQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16510), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16515), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16520), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16525), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16511), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16516), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16521), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16526), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16512), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16517), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16522), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16527), { 0, 0, height },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16513), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16518), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16523), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16528), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16514), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16519), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16524), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16529), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7CF8 */
static void CorkscrewRCTrackRightQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16490), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16495), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16500), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16505), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16491), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16496), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16501), { 0, 0, height },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16506), { 0, 0, height },
                        { 32, 16, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16492), { 0, 0, height },
                        { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16497), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16502), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16507), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16493), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16498), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16503), { 0, 0, height },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16508), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16494), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16499), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16504), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16509), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7D08 */
static void CorkscrewRCTrackLeftQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    CorkscrewRCTrackRightQuarterTurn525DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A7D18 */
static void CorkscrewRCTrackRightQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    CorkscrewRCTrackLeftQuarterTurn525DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7D28 */
static void CorkscrewRCTrackSBendLeft(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16402), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16406), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16405), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16409), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16403), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16407), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 6, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16404), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16408), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16404), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16408), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16403), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16407), { 0, 0, height },
                        { 32, 26, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16405), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16409), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16402), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16406), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
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

/** rct2: 0x008A7D38 */
static void CorkscrewRCTrackSBendRight(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16410), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16414), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16413), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16417), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16411), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16415), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16412), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16416), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16412), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16416), { 0, 0, height },
                        { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16411), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16415), { 0, 0, height },
                        { 32, 26, 3 }, { 0, 6, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16413), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16417), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16410), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16414), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
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

/** rct2: 0x008A7D48 */
static void CorkscrewRCTrackLeftVerticalLoop(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16562), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16570), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16569), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16577), { 0, 6, height },
                        { 32, 20, 7 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16563), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16571), { 0, 14, height },
                        { 32, 2, 63 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16568), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16576), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16564), { 16, 0, height },
                        { 3, 16, 119 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16572), { 12, 0, height },
                        { 3, 16, 119 }, { 12, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16567), { 10, 16, height },
                        { 4, 16, 119 }, { 10, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16575), { 16, 16, height },
                        { 2, 16, 119 }, { 16, 16, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16565), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16573), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16566), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16574), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16566), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16574), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16565), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16573), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16567), { 10, 16, height },
                        { 4, 16, 119 }, { 10, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16575), { 16, 16, height },
                        { 2, 16, 119 }, { 16, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16564), { 16, 0, height },
                        { 3, 16, 119 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16572), { 12, 0, height },
                        { 3, 16, 119 }, { 12, 0, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16568), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16576), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16563), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16571), { 0, 14, height },
                        { 32, 2, 63 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16569), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16577), { 0, 6, height },
                        { 32, 20, 7 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16562), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16570), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, TUNNEL_1);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_1);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A7D58 */
static void CorkscrewRCTrackRightVerticalLoop(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16593), { 0, 6, height },
                        { 32, 20, 7 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16585), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16586), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16578), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16592), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16584), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16587), { 0, 14, height },
                        { 32, 2, 63 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16579), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16591), { 16, 16, height },
                        { 2, 16, 119 }, { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16583), { 10, 16, height },
                        { 4, 16, 119 }, { 10, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16588), { 12, 0, height },
                        { 3, 16, 119 }, { 12, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16580), { 16, 0, height },
                        { 2, 16, 119 }, { 16, 0, height });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16590), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16582), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16589), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16581), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16589), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16581), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16590), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16582), { 0, 16, height + 32 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16588), { 12, 0, height },
                        { 3, 16, 119 }, { 12, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16580), { 16, 0, height },
                        { 2, 16, 119 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16591), { 16, 16, height },
                        { 2, 16, 119 }, { 16, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16583), { 10, 16, height },
                        { 4, 16, 119 }, { 10, 16, height });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16587), { 0, 14, height },
                        { 32, 2, 63 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16579), { 0, 0, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16592), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16584), { 0, 6, height },
                        { 32, 26, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16586), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16578), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16593), { 0, 6, height },
                        { 32, 20, 7 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16585), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, TUNNEL_1);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, TUNNEL_1);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }

    TrackPaintUtilRightVerticalLoopSegments(session, direction, trackSequence);
}

/** rct2: 0x008A7D98 */
static void CorkscrewRCTrackLeftQuarterTurn3(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16381), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16384), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16387), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16378), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16380), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16383), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16386), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16377), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16379), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16382), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16385), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16376), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
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

/** rct2: 0x008A7DA8 */
static void CorkscrewRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    CorkscrewRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7DB8 */
static void CorkscrewRCTrackLeftQuarterTurn3Bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16393), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16400), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16396), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16399), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16390), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16392), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16395), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16398), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16389), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16391), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16394), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16397), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16401), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16388), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
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

/** rct2: 0x008A7DC8 */
static void CorkscrewRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    CorkscrewRCTrackLeftQuarterTurn3Bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7DD8 */
static void CorkscrewRCTrackLeftQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16541), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16543), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16545), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16539), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16540), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16542), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16544), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16538), { 6, 0, height },
                        { 20, 32, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7DE8 */
static void CorkscrewRCTrackRightQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16530), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16532), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16534), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16536), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16531), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16533), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16535), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16537), { 6, 0, height },
                        { 20, 32, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_2);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_2);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7DF8 */
static void CorkscrewRCTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    CorkscrewRCTrackRightQuarterTurn325DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A7E08 */
static void CorkscrewRCTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    CorkscrewRCTrackLeftQuarterTurn325DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7E18 */
static void CorkscrewRCTrackHalfLoopUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16594), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16602), { 0, 6, height },
                        { 32, 20, 11 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16601), { 0, 6, height },
                        { 32, 20, 9 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16609), { 0, 6, height },
                        { 32, 20, 7 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16595), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16603), { 0, 14, height },
                        { 3, 20, 63 }, { 28, 6, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16600), { 0, 6, height },
                        { 3, 20, 63 }, { 28, 6, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16608), { 0, 6, height },
                        { 32, 20, 3 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16596), { 16, 0, height },
                        { 2, 16, 119 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16604), { 12, 0, height },
                        { 3, 16, 119 }, { 12, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16599), { 10, 16, height },
                        { 4, 12, 119 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16607), { 16, 16, height },
                        { 2, 16, 119 }, { 15, 6, height });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16597), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16605), { 0, 0, height + 32 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16598), { 0, 16, height + 32 },
                        { 32, 12, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16606), { 0, 16, height + 32 },
                        { 32, 12, 3 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A7E28 */
static void CorkscrewRCTrackHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrackHalfLoopUp(session, ride, 3 - trackSequence, direction, height, trackElement);
}

/** rct2: 0x008A7E38 */
static void CorkscrewRCTrackLeftCorkscrewUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16610), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16613), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16616), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16619), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
            }

            TrackPaintUtilLeftCorkscrewUpSupports(session, direction, height);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16611), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16614), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16617), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16620), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16612), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16615), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16618), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16621), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A7E48 */
static void CorkscrewRCTrackRightCorkscrewUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16622), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16625), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16628), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16631), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 4 });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16623), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16626), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16629), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16632), { 0, 0, height },
                        { 20, 20, 3 }, { 6, 6, height + 10 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16624), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16627), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16630), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16633), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 24 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A7E58 */
static void CorkscrewRCTrackLeftCorkscrewDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrackRightCorkscrewUp(session, ride, 2 - trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A7E68 */
static void CorkscrewRCTrackRightCorkscrewDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrackLeftCorkscrewUp(session, ride, 2 - trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7E78 */
static void CorkscrewRCTrackLeftHalfBankedHelixUpSmall(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16481), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16488), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16484), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16487), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16478), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16480), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16483), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16486), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16477), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16479), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16482), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16485), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16489), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16476), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16478), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16481), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16488), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16484), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16487), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16477), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16480), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16483), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16486), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16476), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16479), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16482), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16485), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16489), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
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

/** rct2: 0x008A7E88 */
static void CorkscrewRCTrackRightHalfBankedHelixUpSmall(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16462), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16465), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16468), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16471), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16475), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16463), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16466), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16469), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16472), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16464), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16467), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16474), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16470), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16473), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16465), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16468), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16471), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16475), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16462), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16466), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16469), { 0, 0, height },
                        { 16, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16472), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16463), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16467), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16474), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16470), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16473), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16464), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
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

/** rct2: 0x008A7E98 */
static void CorkscrewRCTrackLeftHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    CorkscrewRCTrackRightHalfBankedHelixUpSmall(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A7EA8 */
static void CorkscrewRCTrackRightHalfBankedHelixDownSmall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 4)
    {
        trackSequence -= 4;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    CorkscrewRCTrackLeftHalfBankedHelixUpSmall(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7EB8 */
static void CorkscrewRCTrackLeftHalfBankedHelixUpLarge(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16449), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16460), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16454), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16459), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16444), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16448), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16453), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16458), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16443), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16447), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16452), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16457), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16442), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16446), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16451), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16456), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16441), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16445), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16450), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16455), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16461), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16440), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16444), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16449), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16460), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16454), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16459), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16443), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16448), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16453), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16458), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16442), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16447), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16452), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16457), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16441), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16446), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16451), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16456), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16440), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16445), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16450), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16455), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16461), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
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

/** rct2: 0x008A7EC8 */
static void CorkscrewRCTrackRightHalfBankedHelixUpLarge(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16418), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16423), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16428), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16433), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16439), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16419), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16424), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16429), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16434), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16420), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16425), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16430), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16435), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16421), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16426), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16431), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16436), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16422), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height + 8 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16427), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16438), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16432), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16437), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16423), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16428), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16433), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16439), { 0, 0, height },
                        { 1, 32, 26 }, { 27, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16418), { 0, 0, height },
                        { 20, 32, 3 }, { 6, 0, height });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16424), { 0, 0, height },
                        { 16, 32, 3 }, { 16, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16429), { 0, 0, height },
                        { 16, 32, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16434), { 0, 0, height },
                        { 16, 32, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16419), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16425), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16430), { 0, 0, height },
                        { 16, 16, 1 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16435), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16420), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16426), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16431), { 0, 0, height },
                        { 32, 16, 1 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16436), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16421), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16427), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16438), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16432), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16437), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16422), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height + 8 });
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

/** rct2: 0x008A7ED8 */
static void CorkscrewRCTrackLeftHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction - 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    CorkscrewRCTrackRightHalfBankedHelixUpLarge(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A7EE8 */
static void CorkscrewRCTrackRightHalfBankedHelixDownLarge(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackSequence >= 7)
    {
        trackSequence -= 7;
        direction = (direction + 1) & 3;
    }
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    CorkscrewRCTrackLeftHalfBankedHelixUpLarge(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7F18 */
static void CorkscrewRCTrackLeftQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16555), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16559), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16556), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16560), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16557), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16561), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16554), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16558), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
    }
    TrackPaintUtilLeftQuarterTurn1TileTunnel(session, direction, height, -8, TUNNEL_1, +56, TUNNEL_2);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008A7EF8 */
static void CorkscrewRCTrackRightQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16546), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16550), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16547), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16551), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16548), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16552), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16549), { 0, 0, height }, { 28, 28, 3 },
                { 2, 2, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16553), { 0, 0, height }, { 28, 28, 1 },
                { 2, 2, height + 99 });
            break;
    }
    TrackPaintUtilRightQuarterTurn1TileTunnel(session, direction, height, -8, TUNNEL_1, +56, TUNNEL_2);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008A7F08 */
static void CorkscrewRCTrackLeftQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrackRightQuarterTurn160DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008A7F28 */
static void CorkscrewRCTrackRightQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrackLeftQuarterTurn160DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008A7F38 */
static void CorkscrewRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16230), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16231), { 0, 0, height }, { 32, 20, 3 },
                { 0, 6, height });
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

/** rct2: 0x008A7F48 */
static void CorkscrewRCTrackOnRidePhoto(
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
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16224), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16225), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16224), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16225), { 0, 0, height }, { 32, 20, 0 },
                { 0, 6, height + 3 });
            break;
    }
    TrackPaintUtilOnridePhotoPaint(session, direction, height + 3, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008A8198 */
static void CorkscrewRCTrackFlatTo60DegUpLongBase(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16788), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16792), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16796), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16800), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16789), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16793), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16797), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16801), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16790), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16794), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16798), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16802), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16791), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16795), { 0, 0, height },
                        { 32, 1, 98 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16799), { 0, 0, height },
                        { 32, 1, 98 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16803), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 18, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 24, TUNNEL_SQUARE_8);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 24, TUNNEL_SQUARE_8);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
            break;
    }
}

/** rct2: 0x008A81A8 */
static void CorkscrewRCTrack60DegUpToFlatLongBase(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16804), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16808), { 0, 0, height },
                        { 32, 1, 98 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16812), { 0, 0, height },
                        { 32, 1, 98 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16816), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_7);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16805), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16809), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16813), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16817), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16806), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16810), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16814), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16818), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16807), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16811), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16815), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16819), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
            {
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
            }
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_SQUARE_FLAT);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
            break;
    }
}

/** rct2: 0x008A81B8 */
static void CorkscrewRCTrack60DegDownToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrackFlatTo60DegUpLongBase(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A81C8 */
static void CorkscrewRCTrackFlatTo60DegDownLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    CorkscrewRCTrack60DegUpToFlatLongBase(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7F68 */
static void CorkscrewRCTrackLeftEighthToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16740), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16744), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16748), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16752), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16741), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16745), { 0, 0, height },
                        { 34, 16, 3 }, { 0, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16749), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16753), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16742), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16746), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16750), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16754), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16743), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16747), { 0, 0, height },
                        { 16, 18, 3 }, { 0, 16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16751), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16755), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7F78 */
static void CorkscrewRCTrackRightEighthToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16724), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16728), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16732), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16736), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16725), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16729), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16733), { 0, 0, height },
                        { 34, 16, 3 }, { 0, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16737), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16726), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16730), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16734), { 0, 0, height },
                        { 28, 28, 3 }, { 4, 4, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16738), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16727), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16731), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16735), { 0, 0, height },
                        { 16, 18, 3 }, { 0, 16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16739), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7F88 */
static void CorkscrewRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    CorkscrewRCTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7F98 */
static void CorkscrewRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    CorkscrewRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008A7FA8 */
static void CorkscrewRCTrackLeftEighthBankToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16772), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16776), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16780), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16784), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16773), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16777), { 0, 0, height },
                        { 34, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16781), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16785), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16774), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 16, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16778), { 0, 0, height },
                        { 16, 16, 0 }, { 16, 16, height + 27 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16782), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16786), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16775), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16779), { 0, 0, height },
                        { 16, 18, 0 }, { 0, 16, height + 27 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16783), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16787), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7FB8 */
static void CorkscrewRCTrackRightEighthBankToDiag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16756), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16760), { 0, 0, height },
                        { 32, 20, 3 }, { 0, 6, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16764), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16768), { 0, 0, height },
                        { 32, 1, 26 }, { 0, 27, height });
                    break;
            }
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16757), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16761), { 0, 0, height },
                        { 32, 16, 3 }, { 0, 16, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16765), { 0, 0, height },
                        { 34, 16, 0 }, { 0, 0, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16769), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16758), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16762), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16766), { 0, 0, height },
                        { 28, 28, 0 }, { 4, 4, height + 27 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16770), { 0, 0, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16759), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16763), { 0, 0, height },
                        { 16, 16, 3 }, { 0, 0, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16767), { 0, 0, height },
                        { 16, 18, 0 }, { 0, 16, height + 27 });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16771), { 0, 0, height },
                        { 16, 16, 3 }, { 16, 16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

/** rct2: 0x008A7FC8 */
static void CorkscrewRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    CorkscrewRCTrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008A7FD8 */
static void CorkscrewRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    CorkscrewRCTrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008A7F58 */
static void CorkscrewRCTrackDiagFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16699), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16637), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16696), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16634), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16698), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16636), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16697), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16635), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A8008 */
static void CorkscrewRCTrackDiag25DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16711), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16649), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16708), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16646), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16710), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16648), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16709), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16647), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A8068 */
static void CorkscrewRCTrackDiag60DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16723), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16661), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16720), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16658), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16722), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16660), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16721), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16659), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 32, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 36, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008A7FE8 */
static void CorkscrewRCTrackDiagFlatTo25DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16703), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16641), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16700), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16638), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16702), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16640), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16701), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16639), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A8048 */
static void CorkscrewRCTrackDiag25DegUpTo60DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16715), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16653), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16712), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16650), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16714), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16652), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16713), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16651), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A8058 */
static void CorkscrewRCTrackDiag60DegUpTo25DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16719), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16657), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16716), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16654), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16718), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16656), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16717), { -16, -16, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16655), { -16, -16, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 21, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A7FF8 */
static void CorkscrewRCTrackDiag25DegUpToFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16707), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16645), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16704), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16642), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16706), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16644), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16705), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16643), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A8038 */
static void CorkscrewRCTrackDiag25DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16709), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16647), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16710), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16648), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16708), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16646), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16711), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16649), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A8098 */
static void CorkscrewRCTrackDiag60DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16721), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16659), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16722), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16660), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16720), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16658), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16723), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16661), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
            break;
    }
}

/** rct2: 0x008A8018 */
static void CorkscrewRCTrackDiagFlatTo25DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16705), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16643), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16706), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16644), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16704), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16642), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16707), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16645), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A8078 */
static void CorkscrewRCTrackDiag25DegDownTo60DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16717), { -16, -16, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16655), { -16, -16, height },
                            { 16, 16, 3 }, { 0, 0, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16718), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16656), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16716), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16654), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16719), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16657), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 17, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A8088 */
static void CorkscrewRCTrackDiag60DegDownTo25DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16713), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16651), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16714), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16652), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16712), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16650), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16715), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16653), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008A8028 */
static void CorkscrewRCTrackDiag25DegDownToFlat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16701), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16639), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16702), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16640), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16700), { -16, -16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16638), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16703), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16641), { -16, -16, height },
                            { 32, 32, 3 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A80C8 */
static void CorkscrewRCTrackDiagFlatToLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16669), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16666), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16670), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16668), { -16, -16, height },
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
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16667), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A80D8 */
static void CorkscrewRCTrackDiagFlatToRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16674), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16671), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16673), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16675), { -16, -16, height },
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
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16672), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A80E8 */
static void CorkscrewRCTrackDiagLeftBankToFlat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16672), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16673), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16675), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16671), { -16, -16, height },
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
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16674), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A80F8 */
static void CorkscrewRCTrackDiagRightBankToFlat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16667), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16668), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16666), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16670), { -16, -16, height },
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
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16669), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A8128 */
static void CorkscrewRCTrackDiagLeftBankTo25DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16689), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16686), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16690), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16688), { -16, -16, height },
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
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16687), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A8138 */
static void CorkscrewRCTrackDiagRightBankTo25DegUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16694), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16691), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16693), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16695), { -16, -16, height },
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
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16692), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A8108 */
static void CorkscrewRCTrackDiag25DegUpToLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16679), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16676), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16680), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16678), { -16, -16, height },
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
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16677), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A8118 */
static void CorkscrewRCTrackDiag25DegUpToRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16684), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16681), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16683), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16685), { -16, -16, height },
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
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16682), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
            break;
    }
}

/** rct2: 0x008A8148 */
static void CorkscrewRCTrackDiagLeftBankTo25DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16682), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16683), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16685), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16681), { -16, -16, height },
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
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16684), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A8158 */
static void CorkscrewRCTrackDiagRightBankTo25DegDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16677), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16678), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16676), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16680), { -16, -16, height },
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
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16679), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008A8168 */
static void CorkscrewRCTrackDiag25DegDownToLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16692), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16693), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16695), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16691), { -16, -16, height },
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
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16694), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A8178 */
static void CorkscrewRCTrackDiag25DegDownToRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16687), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16688), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16686), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16690), { -16, -16, height },
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
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16689), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008A80A8 */
static void CorkscrewRCTrackDiagLeftBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16665), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16662), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16664), { -16, -16, height },
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
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16663), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A80B8 */
static void CorkscrewRCTrackDiagRightBank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16663), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16664), { -16, -16, height },
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16662), { -16, -16, height },
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
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(16665), { -16, -16, height },
                        { 32, 32, 3 }, { -16, -16, height });
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x008A8188 */
static void CorkscrewRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isClosed = trackElement.IsBrakeClosed();
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_CorkscrewRCBlockBrakeImages[direction][isClosed]),
        { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
    MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void CorkscrewRCTrackBooster(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (!IsCsgLoaded())
    {
        CorkscrewRCTrackBrakes(session, ride, trackSequence, direction, height, trackElement);
        return;
    }

    uint32_t sprite_ne_sw_behind = SPR_CSG_BEGIN + 56213;
    uint32_t sprite_nw_se_behind = SPR_CSG_BEGIN + 56214;
    uint32_t sprite_ne_sw_after = SPR_CSG_BEGIN + 56215;
    uint32_t sprite_nw_se_after = SPR_CSG_BEGIN + 56216;

    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(sprite_ne_sw_behind), { 0, 0, height },
                { 32, 20, 3 }, { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(sprite_ne_sw_after), { 0, 0, height },
                { 32, 1, 26 }, { 0, 27, height });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(sprite_nw_se_behind), { 0, 0, height },
                { 32, 20, 3 }, { 0, 6, height });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(sprite_nw_se_after), { 0, 0, height },
                { 32, 1, 26 }, { 0, 27, height });
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

static void CorkscrewRCTrackLeftTwistDownToUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26972), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26978), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26971), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26977), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26973), { 0, 6, height + 8 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26979), { 0, 6, height + 8 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26970), { 0, 6, height + 8 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26976), { 0, 6, height + 8 },
                        { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26974), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26980), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26969), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26975), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 26, TUNNEL_0);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 26, TUNNEL_0);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

static void CorkscrewRCTrackRightTwistDownToUp(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26984), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26990), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26983), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26989), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26985), { 0, 6, height + 8 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26991), { 0, 6, height + 8 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26982), { 0, 6, height + 8 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26988), { 0, 6, height + 8 },
                        { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26986), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26992), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26981), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26987), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 26, TUNNEL_0);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 26, TUNNEL_0);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

static void CorkscrewRCTrackLeftTwistUpToDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26969), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26975), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26974), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26980), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26970), { 0, 6, height + 8 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26976), { 0, 6, height + 8 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26973), { 0, 6, height + 8 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26979), { 0, 6, height + 8 },
                        { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26971), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26977), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26972), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26978), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 26, TUNNEL_0);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 26, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

static void CorkscrewRCTrackRightTwistUpToDown(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26981), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26987), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26986), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26992), { 0, 6, height - 8 },
                        { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26982), { 0, 6, height + 8 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26988), { 0, 6, height + 8 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26985), { 0, 6, height + 8 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26991), { 0, 6, height + 8 },
                        { 32, 20, 3 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26983), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26989), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26984), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26990), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, METAL_SUPPORTS_TUBES_INVERTED, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 26, TUNNEL_0);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 26, TUNNEL_0);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionCorkscrewRC(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return CorkscrewRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return CorkscrewRCTrackStation;
        case TrackElemType::Up25:
            return CorkscrewRCTrack25DegUp;
        case TrackElemType::Up60:
            return CorkscrewRCTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return CorkscrewRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return CorkscrewRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return CorkscrewRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return CorkscrewRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return CorkscrewRCTrack25DegDown;
        case TrackElemType::Down60:
            return CorkscrewRCTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return CorkscrewRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return CorkscrewRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return CorkscrewRCTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return CorkscrewRCTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return CorkscrewRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return CorkscrewRCTrackRightQuarterTurn5;
        case TrackElemType::FlatToLeftBank:
            return CorkscrewRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return CorkscrewRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return CorkscrewRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return CorkscrewRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return CorkscrewRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return CorkscrewRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankToUp25:
            return CorkscrewRCTrackLeftBankTo25DegUp;
        case TrackElemType::RightBankToUp25:
            return CorkscrewRCTrackRightBankTo25DegUp;
        case TrackElemType::Up25ToLeftBank:
            return CorkscrewRCTrack25DegUpToLeftBank;
        case TrackElemType::Up25ToRightBank:
            return CorkscrewRCTrack25DegUpToRightBank;
        case TrackElemType::LeftBankToDown25:
            return CorkscrewRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return CorkscrewRCTrackRightBankTo25DegDown;
        case TrackElemType::Down25ToLeftBank:
            return CorkscrewRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return CorkscrewRCTrack25DegDownToRightBank;
        case TrackElemType::LeftBank:
            return CorkscrewRCTrackLeftBank;
        case TrackElemType::RightBank:
            return CorkscrewRCTrackRightBank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return CorkscrewRCTrackLeftQuarterTurn525DegUp;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return CorkscrewRCTrackRightQuarterTurn525DegUp;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return CorkscrewRCTrackLeftQuarterTurn525DegDown;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return CorkscrewRCTrackRightQuarterTurn525DegDown;
        case TrackElemType::SBendLeft:
            return CorkscrewRCTrackSBendLeft;
        case TrackElemType::SBendRight:
            return CorkscrewRCTrackSBendRight;
        case TrackElemType::LeftVerticalLoop:
            return CorkscrewRCTrackLeftVerticalLoop;
        case TrackElemType::RightVerticalLoop:
            return CorkscrewRCTrackRightVerticalLoop;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return CorkscrewRCTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return CorkscrewRCTrackRightQuarterTurn3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return CorkscrewRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return CorkscrewRCTrackRightQuarterTurn3Bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return CorkscrewRCTrackLeftQuarterTurn325DegUp;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return CorkscrewRCTrackRightQuarterTurn325DegUp;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return CorkscrewRCTrackLeftQuarterTurn325DegDown;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return CorkscrewRCTrackRightQuarterTurn325DegDown;
        case TrackElemType::HalfLoopUp:
            return CorkscrewRCTrackHalfLoopUp;
        case TrackElemType::HalfLoopDown:
            return CorkscrewRCTrackHalfLoopDown;
        case TrackElemType::LeftCorkscrewUp:
            return CorkscrewRCTrackLeftCorkscrewUp;
        case TrackElemType::RightCorkscrewUp:
            return CorkscrewRCTrackRightCorkscrewUp;
        case TrackElemType::LeftCorkscrewDown:
            return CorkscrewRCTrackLeftCorkscrewDown;
        case TrackElemType::RightCorkscrewDown:
            return CorkscrewRCTrackRightCorkscrewDown;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
            return CorkscrewRCTrackLeftHalfBankedHelixUpSmall;
        case TrackElemType::RightHalfBankedHelixUpSmall:
            return CorkscrewRCTrackRightHalfBankedHelixUpSmall;
        case TrackElemType::LeftHalfBankedHelixDownSmall:
            return CorkscrewRCTrackLeftHalfBankedHelixDownSmall;
        case TrackElemType::RightHalfBankedHelixDownSmall:
            return CorkscrewRCTrackRightHalfBankedHelixDownSmall;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
            return CorkscrewRCTrackLeftHalfBankedHelixUpLarge;
        case TrackElemType::RightHalfBankedHelixUpLarge:
            return CorkscrewRCTrackRightHalfBankedHelixUpLarge;
        case TrackElemType::LeftHalfBankedHelixDownLarge:
            return CorkscrewRCTrackLeftHalfBankedHelixDownLarge;
        case TrackElemType::RightHalfBankedHelixDownLarge:
            return CorkscrewRCTrackRightHalfBankedHelixDownLarge;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return CorkscrewRCTrackLeftQuarterTurn160DegUp;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return CorkscrewRCTrackRightQuarterTurn160DegUp;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return CorkscrewRCTrackLeftQuarterTurn160DegDown;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return CorkscrewRCTrackRightQuarterTurn160DegDown;
        case TrackElemType::Brakes:
            return CorkscrewRCTrackBrakes;
        case TrackElemType::OnRidePhoto:
            return CorkscrewRCTrackOnRidePhoto;
        case TrackElemType::FlatToUp60LongBase:
            return CorkscrewRCTrackFlatTo60DegUpLongBase;
        case TrackElemType::Up60ToFlatLongBase:
            return CorkscrewRCTrack60DegUpToFlatLongBase;
        case TrackElemType::Down60ToFlatLongBase:
            return CorkscrewRCTrack60DegDownToFlatLongBase;
        case TrackElemType::FlatToDown60LongBase:
            return CorkscrewRCTrackFlatTo60DegDownLongBase;
        case TrackElemType::LeftEighthToDiag:
            return CorkscrewRCTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return CorkscrewRCTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return CorkscrewRCTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return CorkscrewRCTrackRightEighthToOrthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return CorkscrewRCTrackLeftEighthBankToDiag;
        case TrackElemType::RightEighthBankToDiag:
            return CorkscrewRCTrackRightEighthBankToDiag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return CorkscrewRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return CorkscrewRCTrackRightEighthBankToOrthogonal;
        case TrackElemType::DiagFlat:
            return CorkscrewRCTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return CorkscrewRCTrackDiag25DegUp;
        case TrackElemType::DiagUp60:
            return CorkscrewRCTrackDiag60DegUp;
        case TrackElemType::DiagFlatToUp25:
            return CorkscrewRCTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToUp60:
            return CorkscrewRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::DiagUp60ToUp25:
            return CorkscrewRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return CorkscrewRCTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return CorkscrewRCTrackDiag25DegDown;
        case TrackElemType::DiagDown60:
            return CorkscrewRCTrackDiag60DegDown;
        case TrackElemType::DiagFlatToDown25:
            return CorkscrewRCTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToDown60:
            return CorkscrewRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::DiagDown60ToDown25:
            return CorkscrewRCTrackDiag60DegDownTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return CorkscrewRCTrackDiag25DegDownToFlat;
        case TrackElemType::DiagFlatToLeftBank:
            return CorkscrewRCTrackDiagFlatToLeftBank;
        case TrackElemType::DiagFlatToRightBank:
            return CorkscrewRCTrackDiagFlatToRightBank;
        case TrackElemType::DiagLeftBankToFlat:
            return CorkscrewRCTrackDiagLeftBankToFlat;
        case TrackElemType::DiagRightBankToFlat:
            return CorkscrewRCTrackDiagRightBankToFlat;
        case TrackElemType::DiagLeftBankToUp25:
            return CorkscrewRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::DiagRightBankToUp25:
            return CorkscrewRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::DiagUp25ToLeftBank:
            return CorkscrewRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::DiagUp25ToRightBank:
            return CorkscrewRCTrackDiag25DegUpToRightBank;
        case TrackElemType::DiagLeftBankToDown25:
            return CorkscrewRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::DiagRightBankToDown25:
            return CorkscrewRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::DiagDown25ToLeftBank:
            return CorkscrewRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::DiagDown25ToRightBank:
            return CorkscrewRCTrackDiag25DegDownToRightBank;
        case TrackElemType::DiagLeftBank:
            return CorkscrewRCTrackDiagLeftBank;
        case TrackElemType::DiagRightBank:
            return CorkscrewRCTrackDiagRightBank;
        case TrackElemType::BlockBrakes:
            return CorkscrewRCTrackBlockBrakes;

        case TrackElemType::Booster:
            return CorkscrewRCTrackBooster;
        case TrackElemType::LeftTwistDownToUp:
            return CorkscrewRCTrackLeftTwistDownToUp;
        case TrackElemType::RightTwistDownToUp:
            return CorkscrewRCTrackRightTwistDownToUp;
        case TrackElemType::LeftTwistUpToDown:
            return CorkscrewRCTrackLeftTwistUpToDown;
        case TrackElemType::RightTwistUpToDown:
            return CorkscrewRCTrackRightTwistUpToDown;
    }
    return nullptr;
}
