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

/** rct2: 0x00890CB4 */
static void inverted_hairpin_rc_track_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17030), { 0, 0, height + 24 },
                    { 32, 20, 1 }, { 0, 6, height + 22 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17031), { 0, 0, height + 24 },
                    { 32, 20, 1 }, { 0, 6, height + 22 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17032), { 0, 0, height + 24 },
                    { 32, 20, 1 }, { 0, 6, height + 22 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17033), { 0, 0, height + 24 },
                    { 32, 20, 1 }, { 0, 6, height + 22 });
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
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17026), { 0, 0, height + 24 },
                    { 32, 20, 1 }, { 0, 6, height + 22 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17027), { 0, 0, height + 24 },
                    { 32, 20, 1 }, { 0, 6, height + 22 });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, Tunnel0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00890D84, 0x00890D94, 0x00890DA4 */
static void inverted_hairpin_rc_track_station(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { SPR_STATION_BASE_C_SW_NE, 17028, SPR_STATION_INVERTED_BAR_0_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 17029, SPR_STATION_INVERTED_BAR_0_NW_SE },
        { SPR_STATION_BASE_C_SW_NE, 17028, SPR_STATION_INVERTED_BAR_0_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 17029, SPR_STATION_INVERTED_BAR_0_NW_SE },
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][0]), { 0, 0, height },
        { 32, 28, 1 }, { 0, 2, height });
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][1]), { 0, 0, height + 24 },
        { 32, 20, 3 }, { 0, 6, height + 24 });
    PaintAddImageAsChildRotated(
        session, direction, session.TrackColours[SCHEME_SUPPORTS].WithIndex(imageIds[direction][2]), { 0, 6, height + 24 },
        { 32, 20, 1 }, { 0, 6, height + 24 });
    track_paint_util_draw_station_metal_supports_2(session, direction, height, session.TrackColours[SCHEME_SUPPORTS], 11);
    track_paint_util_draw_station_inverted(session, ride, direction, height, trackElement, STATION_VARIANT_1);
    PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareFlat);
    PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00890CC4 */
static void inverted_hairpin_rc_track_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17070), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17071), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17072), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17073), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17042), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17043), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17044), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17045), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 40 });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 6, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 8, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 7, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 5, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, Tunnel1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, Tunnel2);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x00890CD4 */
static void inverted_hairpin_rc_track_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17086), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 88 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17087), { 0, 0, height + 24 },
                    { 32, 2, 81 }, { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17088), { 0, 0, height + 24 },
                    { 32, 2, 81 }, { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17089), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 88 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17058), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 88 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17059), { 0, 0, height + 24 },
                    { 32, 2, 81 }, { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17060), { 0, 0, height + 24 },
                    { 32, 2, 81 }, { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17061), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 88 });
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, Tunnel1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, Tunnel2);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x00890CE4 */
static void inverted_hairpin_rc_track_flat_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17062), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17063), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17064), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17065), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17034), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17035), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17036), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17037), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 6, 0, height + 40, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 8, 0, height + 40, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 7, 0, height + 40, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 5, 0, height + 40, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, Tunnel0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, Tunnel2);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x00890CF4 */
static void inverted_hairpin_rc_track_25_deg_up_to_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17074), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 56 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17078), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17075), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17079), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17076), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17077), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 56 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17046), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 56 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17050), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17047), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17051), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17048), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17049), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 56 });
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, Tunnel1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, Tunnel2);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x00890D04 */
static void inverted_hairpin_rc_track_60_deg_up_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17080), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 56 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17084), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17081), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17085), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17082), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17083), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 56 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17052), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 56 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17056), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17053), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17057), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17054), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17055), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 56 });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 6, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 8, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 7, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 5, 0, height + 62, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, Tunnel1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, Tunnel2);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x00890D14 */
static void inverted_hairpin_rc_track_25_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17066), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17067), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17068), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17069), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17038), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17039), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17040), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17041), { 0, 0, height + 24 },
                    { 32, 20, 3 }, { 0, 6, height + 32 });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 6, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 8, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 7, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 5, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, Tunnel0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, Tunnel12);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
}

/** rct2: 0x00890D24 */
static void inverted_hairpin_rc_track_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_hairpin_rc_track_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00890D34 */
static void inverted_hairpin_rc_track_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_hairpin_rc_track_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00890D44 */
static void inverted_hairpin_rc_track_flat_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_hairpin_rc_track_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00890D54 */
static void inverted_hairpin_rc_track_25_deg_down_to_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_hairpin_rc_track_60_deg_up_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00890D64 */
static void inverted_hairpin_rc_track_60_deg_down_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_hairpin_rc_track_25_deg_up_to_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00890D74 */
static void inverted_hairpin_rc_track_25_deg_down_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_hairpin_rc_track_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00890DB4 */
static void inverted_hairpin_rc_track_left_quarter_turn_3(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17123), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 22 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17126), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 22 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17129), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 22 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17120), { 0, 0, height + 24 },
                        { 32, 20, 3 }, { 0, 6, height + 22 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, Tunnel0);
            }
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17122), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 16, 0, height + 22 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17125), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 0, 0, height + 22 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17128), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 0, 16, height + 22 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17119), { 0, 0, height + 24 },
                        { 16, 16, 3 }, { 16, 16, height + 22 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17121), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 22 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17124), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 22 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17127), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 22 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17118), { 0, 0, height + 24 },
                        { 20, 32, 3 }, { 6, 0, height + 22 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, Tunnel0);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, Tunnel0);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
    }
}

/** rct2: 0x00890DC4 */
static void inverted_hairpin_rc_track_right_quarter_turn_3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    inverted_hairpin_rc_track_left_quarter_turn_3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00890DD4 */
static void inverted_hairpin_rc_track_left_quarter_turn_3_25_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17141), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17143), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17145), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17139), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, Tunnel1);
            }
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17140), { 6, 0, height + 24 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17142), { 6, 0, height + 24 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17144), { 6, 0, height + 24 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17138), { 6, 0, height + 24 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, Tunnel2);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, Tunnel2);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x00890DE4 */
static void inverted_hairpin_rc_track_right_quarter_turn_3_25_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17130), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17132), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17134), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17136), { 0, 6, height + 24 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, Tunnel1);
            }
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17131), { 6, 0, height + 24 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17133), { 6, 0, height + 24 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17135), { 6, 0, height + 24 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17137), { 6, 0, height + 24 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, Tunnel2);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, Tunnel2);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x00890DF4 */
static void inverted_hairpin_rc_track_left_quarter_turn_3_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    inverted_hairpin_rc_track_right_quarter_turn_3_25_deg_up(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x00890E04 */
static void inverted_hairpin_rc_track_right_quarter_turn_3_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    inverted_hairpin_rc_track_left_quarter_turn_3_25_deg_up(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00890E64 */
static void inverted_hairpin_rc_track_left_quarter_turn_1(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17114), { 0, 0, height + 24 }, { 26, 24, 3 },
                { 6, 2, height + 24 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17115), { 0, 0, height + 24 }, { 26, 26, 3 },
                { 0, 0, height + 24 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17116), { 0, 0, height + 24 }, { 24, 26, 3 },
                { 2, 6, height + 24 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17117), { 0, 0, height + 24 }, { 24, 24, 3 },
                { 6, 6, height + 24 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
    MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, Tunnel0);
            break;
        case 2:
            PaintUtilPushTunnelRight(session, height, Tunnel0);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, Tunnel0);
            PaintUtilPushTunnelLeft(session, height, Tunnel0);
            break;
    }
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00890E74 */
static void inverted_hairpin_rc_track_right_quarter_turn_1(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_hairpin_rc_track_left_quarter_turn_1(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x00890E24 */
static void inverted_hairpin_rc_track_flat_to_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17102), { 0, 0, height + 24 },
                    { 30, 20, 3 }, { 0, 6, height + 56 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17106), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17103), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17107), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17104), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17105), { 0, 0, height + 24 },
                    { 30, 20, 3 }, { 0, 6, height + 56 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17090), { 0, 0, height + 24 },
                    { 30, 20, 3 }, { 0, 6, height + 56 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17094), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17091), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17095), { 0, 0, height + 24 },
                    { 32, 10, 49 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17092), { 0, 0, height + 24 },
                    { 32, 2, 49 }, { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17093), { 0, 0, height + 24 },
                    { 30, 20, 3 }, { 0, 6, height + 56 });
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, Tunnel0);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, Tunnel2);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x00890E34 */
static void inverted_hairpin_rc_track_60_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17108), { 0, 0, height + 24 },
                    { 30, 20, 3 }, { 0, 6, height + 48 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17112), { 0, 0, height + 24 },
                    { 32, 10, 41 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17109), { 0, 0, height + 24 },
                    { 32, 2, 41 }, { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17113), { 0, 0, height + 24 },
                    { 32, 10, 41 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17110), { 0, 0, height + 24 },
                    { 32, 2, 41 }, { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17111), { 0, 0, height + 24 },
                    { 30, 20, 3 }, { 0, 6, height + 48 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17096), { 0, 0, height + 24 },
                    { 30, 20, 3 }, { 0, 6, height + 48 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17100), { 0, 0, height + 24 },
                    { 32, 10, 41 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17097), { 0, 0, height + 24 },
                    { 32, 2, 41 }, { 0, 4, height + 6 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17101), { 0, 0, height + 24 },
                    { 32, 10, 41 }, { 0, 10, height + 6 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17098), { 0, 0, height + 24 },
                    { 32, 2, 41 }, { 0, 4, height + 6 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17099), { 0, 0, height + 24 },
                    { 30, 20, 3 }, { 0, 6, height + 48 });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    switch (direction)
    {
        case 0:
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 6, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 1:
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 8, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 2:
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 7, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
            break;
        case 3:
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 5, 0, height + 54, session.TrackColours[SCHEME_SUPPORTS]);
            break;
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, Tunnel1);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, Tunnel0);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x00890E44 */
static void inverted_hairpin_rc_track_flat_to_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_hairpin_rc_track_60_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00890E54 */
static void inverted_hairpin_rc_track_60_deg_down_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    inverted_hairpin_rc_track_flat_to_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x00890E14 */
static void inverted_hairpin_rc_track_brakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17028), { 0, 0, height + 24 }, { 32, 20, 1 },
                { 0, 6, height + 22 });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17029), { 0, 0, height + 24 }, { 32, 20, 1 },
                { 0, 6, height + 22 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, Tunnel0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

/** rct2: 0x00890E84 */
static void inverted_hairpin_rc_track_block_brakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17028), { 0, 0, height + 24 }, { 32, 20, 1 },
                { 0, 6, height + 22 });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(17029), { 0, 0, height + 24 }, { 32, 20, 1 },
                { 0, 6, height + 22 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height + 30, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, Tunnel0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_inverted_hairpin_rc(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return inverted_hairpin_rc_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return inverted_hairpin_rc_track_station;
        case TrackElemType::Up25:
            return inverted_hairpin_rc_track_25_deg_up;
        case TrackElemType::Up60:
            return inverted_hairpin_rc_track_60_deg_up;
        case TrackElemType::FlatToUp25:
            return inverted_hairpin_rc_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToUp60:
            return inverted_hairpin_rc_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return inverted_hairpin_rc_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return inverted_hairpin_rc_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return inverted_hairpin_rc_track_25_deg_down;
        case TrackElemType::Down60:
            return inverted_hairpin_rc_track_60_deg_down;
        case TrackElemType::FlatToDown25:
            return inverted_hairpin_rc_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToDown60:
            return inverted_hairpin_rc_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return inverted_hairpin_rc_track_60_deg_down_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return inverted_hairpin_rc_track_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return inverted_hairpin_rc_track_left_quarter_turn_3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return inverted_hairpin_rc_track_right_quarter_turn_3;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return inverted_hairpin_rc_track_left_quarter_turn_3_25_deg_up;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return inverted_hairpin_rc_track_right_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return inverted_hairpin_rc_track_left_quarter_turn_3_25_deg_down;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return inverted_hairpin_rc_track_right_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftQuarterTurn1Tile:
            return inverted_hairpin_rc_track_left_quarter_turn_1;
        case TrackElemType::RightQuarterTurn1Tile:
            return inverted_hairpin_rc_track_right_quarter_turn_1;
        case TrackElemType::FlatToUp60:
            return inverted_hairpin_rc_track_flat_to_60_deg_up;
        case TrackElemType::Up60ToFlat:
            return inverted_hairpin_rc_track_60_deg_up_to_flat;
        case TrackElemType::FlatToDown60:
            return inverted_hairpin_rc_track_flat_to_60_deg_down;
        case TrackElemType::Down60ToFlat:
            return inverted_hairpin_rc_track_60_deg_down_to_flat;
        case TrackElemType::Brakes:
            return inverted_hairpin_rc_track_brakes;
        case TrackElemType::BlockBrakes:
            return inverted_hairpin_rc_track_block_brakes;
    }
    return nullptr;
}
