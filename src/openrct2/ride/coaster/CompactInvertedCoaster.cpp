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

/** rct2: 0x008AE6E0 */
static void compact_inverted_rc_track_flat(
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
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26557), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 29 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26558), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 29 });
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
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26555), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 29 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26556), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 29 });
                break;
        }
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008AE950, 0x008AE960, 0x008AE970 */
static void compact_inverted_rc_track_station(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    static constexpr const uint32_t imageIds[4][3] = {
        { SPR_STATION_BASE_C_SW_NE, 26557, SPR_STATION_INVERTED_BAR_A_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 26558, SPR_STATION_INVERTED_BAR_A_NW_SE },
        { SPR_STATION_BASE_C_SW_NE, 26557, SPR_STATION_INVERTED_BAR_A_SW_NE },
        { SPR_STATION_BASE_C_NW_SE, 26558, SPR_STATION_INVERTED_BAR_A_NW_SE },
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
    PaintUtilPushTunnelRotated(session, direction, height, TunnelSquareInverted9);
    PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008AE6F0 */
static void compact_inverted_rc_track_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26621), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 45 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26622), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 45 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26623), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 45 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26624), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 45 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26569), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 45 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26570), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 45 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26571), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 45 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26572), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 45 });
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
                    session, MetalSupportsTubesInverted, 6, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 8, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 7, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 5, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, TunnelInverted5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008AE700 */
static void compact_inverted_rc_track_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26637), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 93 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26638), { 0, 0, height + 29 },
                    { 32, 2, 81 }, { 0, 4, height + 11 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26639), { 0, 0, height + 29 },
                    { 32, 2, 81 }, { 0, 4, height + 11 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26640), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 93 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26585), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 93 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26586), { 0, 0, height + 29 },
                    { 32, 2, 81 }, { 0, 4, height + 11 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26587), { 0, 0, height + 29 },
                    { 32, 2, 81 }, { 0, 4, height + 11 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26588), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 93 });
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, TunnelInverted5);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
}

/** rct2: 0x008AE710 */
static void compact_inverted_rc_track_flat_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26613), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26614), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26615), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26616), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26561), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26562), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26563), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26564), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
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
        PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x008AE720 */
static void compact_inverted_rc_track_25_deg_up_to_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26625), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 61 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26629), { 0, 0, height + 29 },
                    { 32, 10, 49 }, { 0, 10, height + 11 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26626), { 0, 0, height + 29 },
                    { 32, 2, 49 }, { 0, 4, height + 11 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26630), { 0, 0, height + 29 },
                    { 32, 10, 49 }, { 0, 10, height + 11 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26627), { 0, 0, height + 29 },
                    { 32, 2, 49 }, { 0, 4, height + 11 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26628), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 61 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26573), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 61 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26577), { 0, 0, height + 29 },
                    { 32, 10, 49 }, { 0, 10, height + 11 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26574), { 0, 0, height + 29 },
                    { 32, 2, 49 }, { 0, 4, height + 11 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26578), { 0, 0, height + 29 },
                    { 32, 10, 49 }, { 0, 10, height + 11 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26575), { 0, 0, height + 29 },
                    { 32, 2, 49 }, { 0, 4, height + 11 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26576), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 61 });
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TunnelInverted5);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
}

/** rct2: 0x008AE730 */
static void compact_inverted_rc_track_60_deg_up_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26631), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 61 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26635), { 0, 0, height + 29 },
                    { 32, 10, 49 }, { 0, 10, height + 11 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26632), { 0, 0, height + 29 },
                    { 32, 2, 49 }, { 0, 4, height + 11 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26636), { 0, 0, height + 29 },
                    { 32, 10, 49 }, { 0, 10, height + 11 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26633), { 0, 0, height + 29 },
                    { 32, 2, 49 }, { 0, 4, height + 11 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26634), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 61 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26579), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 61 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26583), { 0, 0, height + 29 },
                    { 32, 10, 49 }, { 0, 10, height + 11 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26580), { 0, 0, height + 29 },
                    { 32, 2, 49 }, { 0, 4, height + 11 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26584), { 0, 0, height + 29 },
                    { 32, 10, 49 }, { 0, 10, height + 11 });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26581), { 0, 0, height + 29 },
                    { 32, 2, 49 }, { 0, 4, height + 11 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26582), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 61 });
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
                    session, MetalSupportsTubesInverted, 6, 0, height + 70, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 8, 0, height + 70, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 7, 0, height + 70, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 5, 0, height + 70, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted4);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, TunnelInverted5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
}

/** rct2: 0x008AE740 */
static void compact_inverted_rc_track_25_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26617), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26618), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26619), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26620), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26565), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26566), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26567), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26568), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 37 });
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
                    session, MetalSupportsTubesInverted, 6, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 8, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 7, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 5, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, Tunnel13);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008AE750 */
static void compact_inverted_rc_track_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AE760 */
static void compact_inverted_rc_track_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AE770 */
static void compact_inverted_rc_track_flat_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AE780 */
static void compact_inverted_rc_track_25_deg_down_to_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_60_deg_up_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AE790 */
static void compact_inverted_rc_track_60_deg_down_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_25_deg_up_to_60_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AE7A0 */
static void compact_inverted_rc_track_25_deg_down_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AE7B0 */
static void compact_inverted_rc_track_left_quarter_turn_5(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26694), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26699), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26704), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26689), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26693), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26698), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26703), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26688), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26692), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26697), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26702), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26687), { 0, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26691), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 16, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26696), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26701), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26686), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 16, 0, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26690), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26695), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26700), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26685), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
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
                    PaintUtilPushTunnelRight(session, height, TunnelInverted3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelInverted3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AE7C0 */
static void compact_inverted_rc_track_right_quarter_turn_5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    compact_inverted_rc_track_left_quarter_turn_5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AE7D0 */
static void compact_inverted_rc_track_flat_to_left_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26657), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26658), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26659), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26660), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
        0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008AE7E0 */
static void compact_inverted_rc_track_flat_to_right_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26661), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26662), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26663), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26664), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
        0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008AE7F0 */
static void compact_inverted_rc_track_left_bank_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26663), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26664), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26661), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26662), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
        0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008AE800 */
static void compact_inverted_rc_track_right_bank_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26659), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26660), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26657), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26658), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
        0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008AE810 */
static void compact_inverted_rc_track_banked_left_quarter_turn_5(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26738), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26743), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26748), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26733), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26737), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26742), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26747), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26732), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26736), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26741), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26746), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26731), { 0, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26735), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 16, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26740), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26745), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26730), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 16, 0, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26734), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26739), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26744), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26729), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
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
                    PaintUtilPushTunnelRight(session, height, TunnelInverted3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelInverted3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AE820 */
static void compact_inverted_rc_track_banked_right_quarter_turn_5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    compact_inverted_rc_track_banked_left_quarter_turn_5(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AE830 */
static void compact_inverted_rc_track_left_bank_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26665), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26666), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26667), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26668), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
        0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 6, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 8, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 7, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 5, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x008AE840 */
static void compact_inverted_rc_track_right_bank_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26669), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26670), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26671), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26672), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
        0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 6, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 8, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 7, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 5, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted5);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x008AE850 */
static void compact_inverted_rc_track_25_deg_up_to_left_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26673), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26674), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26675), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26676), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
        0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 6, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 8, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 7, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 5, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, Tunnel13);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008AE860 */
static void compact_inverted_rc_track_25_deg_up_to_right_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26677), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26678), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26679), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26680), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 37 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
        0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 6, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 8, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 7, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                MetalASupportsPaintSetup(
                    session, MetalSupportsTubesInverted, 5, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted3);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, Tunnel13);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
}

/** rct2: 0x008AE870 */
static void compact_inverted_rc_track_left_bank_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_25_deg_up_to_right_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AE880 */
static void compact_inverted_rc_track_right_bank_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_25_deg_up_to_left_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AE890 */
static void compact_inverted_rc_track_25_deg_down_to_left_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_right_bank_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AE8A0 */
static void compact_inverted_rc_track_25_deg_down_to_right_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_left_bank_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AE8B0 */
static void compact_inverted_rc_track_left_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26681), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26682), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26683), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26684), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
        0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008AE8C0 */
static void compact_inverted_rc_track_right_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_left_bank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AE8D0 */
static void compact_inverted_rc_track_left_quarter_turn_5_25_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26949), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26954), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26959), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26964), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26950), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26955), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26960), { 0, 16, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26965), { 0, 16, height + 29 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26951), { 0, 16, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26956), { 16, 16, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26961), { 16, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26966), { 0, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26952), { 16, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26957), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26962), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26967), { 16, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26953), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26958), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26963), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26968), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelInverted5);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelInverted5);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008AE8E0 */
static void compact_inverted_rc_track_right_quarter_turn_5_25_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26929), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26934), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26939), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26944), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26930), { 0, 16, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26935), { 0, 16, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26940), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26945), { 0, 0, height + 29 },
                        { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26931), { 0, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26936), { 16, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26941), { 16, 16, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26946), { 0, 16, height + 29 },
                        { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26932), { 16, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26937), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26942), { 0, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26947), { 16, 0, height + 29 },
                        { 16, 32, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26933), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26938), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26943), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26948), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 48, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelInverted5);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelInverted5);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008AE8F0 */
static void compact_inverted_rc_track_left_quarter_turn_5_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    compact_inverted_rc_track_right_quarter_turn_5_25_deg_up(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008AE900 */
static void compact_inverted_rc_track_right_quarter_turn_5_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    compact_inverted_rc_track_left_quarter_turn_5_25_deg_up(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AE910 */
static void compact_inverted_rc_track_s_bend_left(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26641), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26645), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26644), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26648), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26642), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26646), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26643), { 0, 0, height + 29 },
                        { 32, 26, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26647), { 0, 0, height + 29 },
                        { 32, 26, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 5, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 6, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26643), { 0, 0, height + 29 },
                        { 32, 26, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26647), { 0, 0, height + 29 },
                        { 32, 26, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26642), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26646), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 5, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 6, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26644), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26648), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26641), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26645), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TunnelInverted3);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TunnelInverted3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AE920 */
static void compact_inverted_rc_track_s_bend_right(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26649), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26653), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26652), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26656), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26650), { 0, 0, height + 29 },
                        { 32, 26, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26654), { 0, 0, height + 29 },
                        { 32, 26, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26651), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26655), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 8, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 7, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26651), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26655), { 0, 0, height + 29 },
                        { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26650), { 0, 0, height + 29 },
                        { 32, 26, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26654), { 0, 0, height + 29 },
                        { 32, 26, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 8, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 7, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26652), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26656), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26649), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26653), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TunnelInverted3);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TunnelInverted3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AE930 */
static void compact_inverted_rc_track_left_vertical_loop(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27017), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27025), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27024), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27032), { 0, 6, height + 29 },
                        { 32, 10, 7 }, { 0, 6, height + 45 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SegmentB4 | SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27018), { 0, 0, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27026), { 0, 14, height + 29 },
                        { 32, 2, 63 }, { 0, 29, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27023), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27031), { 0, 6, height + 29 },
                        { 2, 2, 3 }, { 29, 20, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27019), { 16, 0, height + 2 },
                        { 5, 2, 119 }, { 16, 29, height + 2 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27027), { 12, 0, height + 2 },
                        { 32, 2, 119 }, { 0, -6, height + 2 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27022), { 10, 16, height + 2 },
                        { 4, 2, 119 }, { 10, 0, height + 2 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27030), { 16, 16, height + 2 },
                        { 2, 2, 119 }, { 29, 29, height + 2 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27020), { 0, 0, height + 34 },
                        { 32, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27028), { 0, 0, height + 34 },
                        { 2, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27021), { 0, 16, height + 34 },
                        { 32, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27029), { 0, 16, height + 34 },
                        { 2, 2, 3 }, { 0, 0, height - 6 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27021), { 0, 16, height + 34 },
                        { 32, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27029), { 0, 16, height + 34 },
                        { 2, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27020), { 0, 0, height + 34 },
                        { 32, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27028), { 0, 0, height + 34 },
                        { 2, 2, 3 }, { 0, 0, height - 6 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27022), { 10, 16, height + 2 },
                        { 4, 2, 119 }, { 10, 0, height + 2 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27030), { 16, 16, height + 2 },
                        { 2, 2, 119 }, { 29, 29, height + 2 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27019), { 16, 0, height + 2 },
                        { 5, 2, 119 }, { 16, 29, height + 2 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27027), { 12, 0, height + 2 },
                        { 32, 2, 119 }, { 0, -6, height + 2 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27023), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27031), { 0, 6, height + 29 },
                        { 2, 2, 3 }, { 29, 20, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27018), { 0, 0, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27026), { 0, 14, height + 29 },
                        { 32, 2, 63 }, { 0, 29, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27024), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27032), { 0, 6, height + 29 },
                        { 32, 10, 7 }, { 0, 6, height + 45 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27017), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27025), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SegmentB4 | SegmentBC | SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, TunnelInverted4);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, TunnelInverted4);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AE940 */
static void compact_inverted_rc_track_right_vertical_loop(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27048), { 0, 6, height + 29 },
                        { 32, 10, 7 }, { 0, 6, height + 45 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27040), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27041), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27033), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SegmentB8 | SegmentBC | SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27047), { 0, 6, height + 29 },
                        { 2, 2, 3 }, { 29, 20, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27039), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 0, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27042), { 0, 14, height + 29 },
                        { 32, 2, 63 }, { 0, 29, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27034), { 0, 0, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27046), { 16, 16, height + 2 },
                        { 2, 2, 119 }, { 29, 29, height + 2 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27038), { 10, 16, height + 2 },
                        { 4, 2, 119 }, { 10, 0, height + 2 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27043), { 12, 0, height + 2 },
                        { 32, 2, 119 }, { 0, -6, height + 2 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27035), { 16, 0, height + 2 },
                        { 5, 2, 119 }, { 16, 29, height + 2 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27045), { 0, 16, height + 34 },
                        { 2, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27037), { 0, 16, height + 34 },
                        { 32, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27044), { 0, 0, height + 34 },
                        { 2, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27036), { 0, 0, height + 34 },
                        { 32, 2, 3 }, { 0, 0, height - 6 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27044), { 0, 0, height + 34 },
                        { 2, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27036), { 0, 0, height + 34 },
                        { 32, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27045), { 0, 16, height + 34 },
                        { 2, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27037), { 0, 16, height + 34 },
                        { 32, 2, 3 }, { 0, 0, height - 6 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27043), { 12, 0, height + 2 },
                        { 32, 2, 119 }, { 0, -6, height + 2 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27035), { 16, 0, height + 2 },
                        { 5, 2, 119 }, { 16, 29, height + 2 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27046), { 16, 16, height + 2 },
                        { 2, 2, 119 }, { 29, 29, height + 2 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27038), { 10, 16, height + 2 },
                        { 4, 2, 119 }, { 10, 0, height + 2 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27042), { 0, 14, height + 29 },
                        { 32, 2, 63 }, { 0, 29, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27034), { 0, 0, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27047), { 0, 6, height + 29 },
                        { 2, 2, 3 }, { 29, 20, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27039), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 0, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27041), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27033), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27048), { 0, 6, height + 29 },
                        { 32, 10, 7 }, { 0, 6, height + 45 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27040), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SegmentB4 | SegmentB8 | SegmentBC | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 7, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 5, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 6, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 8, 0, height + 49, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, TunnelInverted4);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, TunnelInverted4);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AE980 */
static void compact_inverted_rc_track_left_quarter_turn_3(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26710), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26713), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26716), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26707), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26709), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26712), { 0, 0, height + 29 },
                        { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26715), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26706), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26708), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26711), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26714), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26705), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
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
                    PaintUtilPushTunnelRight(session, height, TunnelInverted3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelInverted3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AE990 */
static void compact_inverted_rc_track_right_quarter_turn_3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    compact_inverted_rc_track_left_quarter_turn_3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AE9A0 */
static void compact_inverted_rc_track_left_quarter_turn_3_bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26722), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26725), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26728), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26719), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26721), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26724), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26727), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26718), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26720), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26723), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26726), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26717), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 29 });
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
                    PaintUtilPushTunnelRight(session, height, TunnelInverted3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelInverted3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AE9B0 */
static void compact_inverted_rc_track_right_quarter_turn_3_bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    compact_inverted_rc_track_left_quarter_turn_3_bank(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AE9C0 */
static void compact_inverted_rc_track_left_quarter_turn_3_25_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26924), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26926), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26928), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26922), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26923), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26925), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26927), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26921), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelInverted5);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelInverted5);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008AE9D0 */
static void compact_inverted_rc_track_right_quarter_turn_3_25_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26913), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26915), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26917), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26919), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26914), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26916), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26918), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26920), { 6, 0, height + 29 },
                        { 20, 32, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 46, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, TunnelInverted5);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, TunnelInverted5);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008AE9E0 */
static void compact_inverted_rc_track_left_quarter_turn_3_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    compact_inverted_rc_track_right_quarter_turn_3_25_deg_up(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008AE9F0 */
static void compact_inverted_rc_track_right_quarter_turn_3_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    compact_inverted_rc_track_left_quarter_turn_3_25_deg_up(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AEA00 */
static void compact_inverted_rc_track_left_twist_down_to_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26993), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26999), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26998), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27004), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26994), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27000), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26997), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27003), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26995), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27001), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26996), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27002), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TunnelInverted3);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TunnelInverted3);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEA10 */
static void compact_inverted_rc_track_right_twist_down_to_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27005), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27011), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27010), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27016), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27006), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27012), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27009), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27015), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27007), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27013), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27008), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27014), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TunnelInverted3);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TunnelInverted3);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEA20 */
static void compact_inverted_rc_track_left_twist_up_to_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26996), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27002), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26995), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27001), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26997), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27003), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26994), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27000), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26998), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27004), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26993), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26999), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TunnelInverted3);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TunnelInverted3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEA30 */
static void compact_inverted_rc_track_right_twist_up_to_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27008), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27014), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27007), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27013), { 0, 6, height },
                        { 32, 20, 3 });
                    break;
            }
            MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27009), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27015), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27006), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27012), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27010), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27016), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27005), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27011), { 0, 6, height + 29 },
                        { 32, 20, 3 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, TunnelInverted3);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, TunnelInverted3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEA40 */
static void compact_inverted_rc_track_half_loop_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27073), { 0, 6, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 45 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27081), { 0, 6, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 45 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27080), { 0, 6, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 45 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27088), { 0, 6, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 45 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 6, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 8, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 7, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 5, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted4);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27074), { 0, 0, height + 29 },
                        { 32, 2, 3 }, { 0, 29, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27082), { 0, 14, height + 29 },
                        { 32, 2, 63 }, { 0, 29, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27079), { 0, 6, height + 29 },
                        { 32, 2, 3 }, { 0, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27087), { 0, 6, height + 29 },
                        { 2, 2, 3 }, { 29, 20, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27075), { 16, 0, height + 2 },
                        { 5, 2, 119 }, { 16, 29, height + 2 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27083), { 12, 0, height + 2 },
                        { 32, 2, 119 }, { 0, -6, height + 2 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27078), { 10, 16, height + 2 },
                        { 4, 2, 119 }, { 10, 0, height + 2 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27086), { 16, 16, height + 2 },
                        { 2, 2, 119 }, { 29, 29, height + 2 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27076), { 0, 0, height + 34 },
                        { 32, 20, 3 }, { 0, 6, height + 34 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27084), { 0, 0, height + 34 },
                        { 2, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27077), { 0, 16, height + 34 },
                        { 32, 2, 3 }, { 0, 0, height - 6 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27085), { 0, 16, height + 34 },
                        { 32, 20, 3 }, { 0, 6, height + 34 });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 32, TunnelInverted3);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008AEA50 */
static void compact_inverted_rc_track_half_loop_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_half_loop_up(session, ride, 3 - trackSequence, direction, height, trackElement);
}

/** rct2: 0x008AEA60 */
static void compact_inverted_rc_track_left_corkscrew_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27049), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 33 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27052), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 33 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27055), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 33 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27058), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 33 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SegmentB4 | SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27050), { 0, 0, height + 14 },
                        { 20, 20, 3 }, { 6, 6, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27053), { 0, 0, height + 14 },
                        { 20, 20, 3 }, { 6, 6, height + 24 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27056), { 0, 0, height + 14 },
                        { 20, 20, 3 }, { 6, 6, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27059), { 0, 0, height + 14 },
                        { 20, 20, 3 }, { 6, 6, height + 24 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27051), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 39 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27054), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 39 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27057), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 39 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27060), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 39 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 40, TunnelInverted3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 40, TunnelInverted3);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SegmentB4 | SegmentBC | SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008AEA70 */
static void compact_inverted_rc_track_right_corkscrew_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27061), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 33 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27064), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 33 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27067), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 33 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27070), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 33 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SegmentB4 | SegmentB8 | SegmentBC | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27062), { 0, 0, height + 14 },
                        { 20, 20, 3 }, { 6, 6, height + 24 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27065), { 0, 0, height + 14 },
                        { 20, 20, 3 }, { 6, 6, height + 24 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27068), { 0, 0, height + 14 },
                        { 20, 20, 3 }, { 6, 6, height + 24 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27071), { 0, 0, height + 14 },
                        { 20, 20, 3 }, { 6, 6, height + 24 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27063), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 39 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27066), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 39 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27069), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 39 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27072), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 39 });
                    break;
            }
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 40, TunnelInverted3);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 40, TunnelInverted3);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SegmentB4 | SegmentB8 | SegmentBC | SegmentC4 | SegmentC8 | SegmentCC | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008AEA80 */
static void compact_inverted_rc_track_left_corkscrew_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_right_corkscrew_up(session, ride, 2 - trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008AEA90 */
static void compact_inverted_rc_track_right_corkscrew_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_left_corkscrew_up(session, ride, 2 - trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AEAD0 */
static void compact_inverted_rc_track_left_quarter_turn_1_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26910), { 0, 0, height + 5 }, { 28, 28, 3 },
                { 2, 2, height + 5 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26906), { 0, 0, height + 5 }, { 28, 28, 1 },
                { 2, 2, height + 104 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26911), { 0, 0, height + 5 }, { 28, 28, 3 },
                { 2, 2, height + 5 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26907), { 0, 0, height + 5 }, { 28, 28, 1 },
                { 2, 2, height + 104 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26912), { 0, 0, height + 5 }, { 28, 28, 3 },
                { 2, 2, height + 5 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26908), { 0, 0, height + 5 }, { 28, 28, 1 },
                { 2, 2, height + 104 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26909), { 0, 0, height + 5 }, { 28, 28, 3 },
                { 2, 2, height + 5 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26905), { 0, 0, height + 5 }, { 28, 28, 1 },
                { 2, 2, height + 104 });
            break;
    }
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height, -8, TunnelInverted4, +56, TunnelInverted5);
    PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008AEAB0 */
static void compact_inverted_rc_track_right_quarter_turn_1_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26901), { 0, 0, height + 5 }, { 28, 28, 3 },
                { 2, 2, height + 5 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26897), { 0, 0, height + 5 }, { 28, 28, 1 },
                { 2, 2, height + 104 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26902), { 0, 0, height + 5 }, { 28, 28, 3 },
                { 2, 2, height + 5 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26898), { 0, 0, height + 5 }, { 28, 28, 1 },
                { 2, 2, height + 104 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26903), { 0, 0, height + 5 }, { 28, 28, 3 },
                { 2, 2, height + 5 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26899), { 0, 0, height + 5 }, { 28, 28, 1 },
                { 2, 2, height + 104 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26904), { 0, 0, height + 5 }, { 28, 28, 3 },
                { 2, 2, height + 5 });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26900), { 0, 0, height + 5 }, { 28, 28, 1 },
                { 2, 2, height + 104 });
            break;
    }
    track_paint_util_right_quarter_turn_1_tile_tunnel(session, direction, height, -8, TunnelInverted4, +56, TunnelInverted5);
    PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104, 0x20);
}

/** rct2: 0x008AEAC0 */
static void compact_inverted_rc_track_left_quarter_turn_1_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_right_quarter_turn_1_60_deg_up(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
}

/** rct2: 0x008AEAE0 */
static void compact_inverted_rc_track_right_quarter_turn_1_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_left_quarter_turn_1_60_deg_up(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
}

/** rct2: 0x008AEAA0 */
static void compact_inverted_rc_track_brakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26559), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26560), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

/** rct2: 0x008AEAF0 */
static void compact_inverted_rc_track_left_quarter_banked_helix_large_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27118), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 35 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27123), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 35 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27128), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 35 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27113), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 35 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27117), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 0, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27122), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 0, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27127), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27112), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 43 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27116), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 16, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27121), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27126), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27111), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 43 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27115), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 16, 0, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27120), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 0, 0, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27125), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 0, 0, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27110), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 16, 0, height + 43 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27114), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27119), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27124), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27109), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 43 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 16, TunnelInverted3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 16, TunnelInverted3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEB00 */
static void compact_inverted_rc_track_right_quarter_banked_helix_large_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27089), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 35 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27094), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 35 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27099), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 35 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27104), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 35 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27090), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27095), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27100), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 0, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27105), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 0, height + 43 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27091), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27096), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27101), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27106), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 16, height + 43 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27092), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 16, 0, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27097), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 0, 0, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27102), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 0, 0, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27107), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 16, 0, height + 43 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27093), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27098), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27103), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27108), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 43 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 16, TunnelInverted3);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 16, TunnelInverted3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEB10 */
static void compact_inverted_rc_track_left_quarter_banked_helix_large_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27098), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27103), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27108), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27093), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 43 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27097), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 0, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27102), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 0, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27107), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27092), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 43 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27096), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 16, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27101), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27106), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27091), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 43 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27095), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 16, 0, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27100), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 0, 0, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27105), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 0, 0, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27090), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 16, 0, height + 43 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27094), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 35 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27099), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 35 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27104), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 35 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27089), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 35 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, TunnelInverted3);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, TunnelInverted3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEB20 */
static void compact_inverted_rc_track_right_quarter_banked_helix_large_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27109), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27114), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27119), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27124), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 43 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27110), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27115), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27120), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 0, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27125), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 0, height + 43 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27111), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27116), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27121), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27126), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 16, height + 43 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27112), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 16, 0, height + 43 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27117), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 0, 0, height + 43 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27122), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 0, 0, height + 43 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27127), { 0, 0, height + 29 },
                        { 16, 32, 3 }, { 16, 0, height + 43 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27113), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 35 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27118), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 35 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27123), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 35 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(27128), { 0, 0, height + 29 },
                        { 20, 32, 3 }, { 6, 0, height + 35 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, TunnelInverted3);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, TunnelInverted3);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEB30 */
static void compact_inverted_rc_track_on_ride_photo(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26555), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26556), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 8, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26555), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, ImageId(SPR_STATION_BASE_D, COLOUR_BLACK), { 0, 0, height }, { 32, 32, 1 });
            MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 6, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 7, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26556), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
    }
    track_paint_util_onride_photo_paint(session, direction, height + 3, trackElement);
    PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
    PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
}

/** rct2: 0x008AEDB0 */
static void compact_inverted_rc_track_90_deg_up(
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
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26609), { 0, 0, height + 29 },
                            { 32, 20, 3 }, { 0, 6, height + 61 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26610), { 0, 0, height + 29 },
                            { 32, 2, 31 }, { 0, 4, height + 11 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26611), { 0, 0, height + 29 },
                            { 32, 2, 31 }, { 0, 4, height + 11 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26612), { 0, 0, height + 29 },
                            { 32, 20, 3 }, { 0, 6, height + 61 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26605), { 0, 0, height + 29 },
                            { 32, 20, 3 }, { 0, 6, height + 61 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26606), { 0, 0, height + 29 },
                            { 32, 2, 31 }, { 0, 4, height + 11 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26607), { 0, 0, height + 29 },
                            { 32, 2, 31 }, { 0, 4, height + 11 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26608), { 0, 0, height + 29 },
                            { 32, 20, 3 }, { 0, 6, height + 61 });
                        break;
                }
            }
            PaintUtilSetVerticalTunnel(session, height + 32);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008AEDC0 */
static void compact_inverted_rc_track_90_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_90_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AED70 */
static void compact_inverted_rc_track_60_deg_up_to_90_deg_up(
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
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26597), { 0, 0, height + 29 },
                            { 32, 20, 3 }, { 0, 6, height + 85 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26598), { 0, 0, height + 29 },
                            { 32, 2, 55 }, { 0, 4, height + 11 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26599), { 0, 0, height + 29 },
                            { 32, 2, 55 }, { 0, 4, height + 11 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26600), { 0, 0, height + 29 },
                            { 32, 20, 3 }, { 0, 6, height + 85 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26589), { 0, 0, height + 29 },
                            { 32, 20, 3 }, { 0, 6, height + 85 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26590), { 0, 0, height + 29 },
                            { 32, 2, 55 }, { 0, 4, height + 11 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26591), { 0, 0, height + 29 },
                            { 32, 2, 55 }, { 0, 4, height + 11 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26592), { 0, 0, height + 29 },
                            { 32, 20, 3 }, { 0, 6, height + 85 });
                        break;
                }
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, TunnelInverted4);
            }
            PaintUtilSetVerticalTunnel(session, height + 56);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008AED80 */
static void compact_inverted_rc_track_90_deg_down_to_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    compact_inverted_rc_track_60_deg_up_to_90_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AED90 */
static void compact_inverted_rc_track_90_deg_up_to_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26601), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 85 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26602), { 0, 0, height + 29 },
                    { 32, 2, 55 }, { 0, 4, height + 11 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26603), { 0, 0, height + 29 },
                    { 32, 2, 55 }, { 0, 4, height + 11 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26604), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 85 });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26593), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 85 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26594), { 0, 0, height + 29 },
                    { 32, 2, 55 }, { 0, 4, height + 11 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26595), { 0, 0, height + 29 },
                    { 32, 2, 55 }, { 0, 4, height + 11 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26596), { 0, 0, height + 29 },
                    { 32, 20, 3 }, { 0, 6, height + 85 });
                break;
        }
    }
    switch (direction)
    {
        case 1:
            PaintUtilPushTunnelRight(session, height + 48, TunnelInverted5);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 48, TunnelInverted5);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 96, 0x20);
}

/** rct2: 0x008AEDA0 */
static void compact_inverted_rc_track_60_deg_down_to_90_deg_down(
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
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26603), { 0, 0, height + 29 },
                            { 32, 2, 55 }, { 0, 4, height + 11 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26604), { 0, 0, height + 29 },
                            { 32, 20, 3 }, { 0, 6, height + 85 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26601), { 0, 0, height + 29 },
                            { 32, 20, 3 }, { 0, 6, height + 85 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26602), { 0, 0, height + 29 },
                            { 32, 2, 55 }, { 0, 4, height + 11 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26595), { 0, 0, height + 29 },
                            { 32, 2, 55 }, { 0, 4, height + 11 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26596), { 0, 0, height + 29 },
                            { 32, 20, 3 }, { 0, 6, height + 85 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26593), { 0, 0, height + 29 },
                            { 32, 20, 3 }, { 0, 6, height + 85 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26594), { 0, 0, height + 29 },
                            { 32, 2, 55 }, { 0, 4, height + 11 });
                        break;
                }
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 48, TunnelInverted5);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 96, 0x20);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008AEB40 */
static void compact_inverted_rc_track_left_eighth_to_diag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26765), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26769), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26773), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26777), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26766), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26770), { 0, 0, height + 29 },
                        { 34, 16, 3 }, { 0, 0, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26774), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26778), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26767), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26771), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26775), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26779), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26768), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26772), { 0, 0, height + 29 },
                        { 16, 18, 3 }, { 0, 16, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26776), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26780), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEB50 */
static void compact_inverted_rc_track_right_eighth_to_diag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26749), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26753), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26757), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26761), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26750), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26754), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26758), { 0, 0, height + 29 },
                        { 34, 16, 3 }, { 0, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26762), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 0, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26751), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26755), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26759), { 0, 0, height + 29 },
                        { 28, 28, 3 }, { 4, 4, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26763), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26752), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26756), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26760), { 0, 0, height + 29 },
                        { 16, 18, 3 }, { 0, 16, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26764), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEB60 */
static void compact_inverted_rc_track_left_eighth_to_orthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    compact_inverted_rc_track_right_eighth_to_diag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AEB70 */
static void compact_inverted_rc_track_right_eighth_to_orthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    compact_inverted_rc_track_left_eighth_to_diag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008AED30 */
static void compact_inverted_rc_track_left_eighth_bank_to_diag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26853), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26857), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26861), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26865), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26854), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26858), { 0, 0, height + 29 },
                        { 34, 16, 3 }, { 0, 0, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26862), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26866), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26855), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26859), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26863), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26867), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26856), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26860), { 0, 0, height + 29 },
                        { 16, 18, 3 }, { 0, 16, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26864), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26868), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SegmentBC | SegmentC0 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AED40 */
static void compact_inverted_rc_track_right_eighth_bank_to_diag(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26837), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26841), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26845), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26849), { 0, 0, height + 29 },
                        { 32, 20, 3 }, { 0, 6, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC4 | SegmentC8 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
            MetalASupportsPaintSetup(
                session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26838), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26842), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 16, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26846), { 0, 0, height + 29 },
                        { 34, 16, 3 }, { 0, 0, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26850), { 0, 0, height + 29 },
                        { 32, 16, 3 }, { 0, 0, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(SegmentBC | SegmentC0 | SegmentC4 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26839), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26843), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26847), { 0, 0, height + 29 },
                        { 28, 28, 3 }, { 4, 4, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26851), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF,
                0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26840), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 0, height + 29 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26844), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 29 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26848), { 0, 0, height + 29 },
                        { 16, 18, 3 }, { 0, 16, height + 29 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26852), { 0, 0, height + 29 },
                        { 16, 16, 3 }, { 16, 16, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentCC | SegmentD0 | SegmentD4, direction),
                0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AED50 */
static void compact_inverted_rc_track_left_eighth_bank_to_orthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    compact_inverted_rc_track_right_eighth_bank_to_diag(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

/** rct2: 0x008AED60 */
static void compact_inverted_rc_track_right_eighth_bank_to_orthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    compact_inverted_rc_track_left_eighth_bank_to_diag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement);
}

/** rct2: 0x008AEB80 */
static void compact_inverted_rc_track_diag_flat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26812), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 29 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26784), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 29 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26809), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 29 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26781), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 29 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26811), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 29 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26783), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 29 });
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
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26810), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 29 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26782), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 29 });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEBB0 */
static void compact_inverted_rc_track_diag_25_deg_up(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26824), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26796), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26821), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26793), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26823), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26795), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26822), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26794), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AEC10 */
static void compact_inverted_rc_track_diag_60_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26808), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 93 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26805), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 93 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26807), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 93 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26806), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 32, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 36, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 32, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 36, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
    }
}

/** rct2: 0x008AEB90 */
static void compact_inverted_rc_track_diag_flat_to_25_deg_up(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26816), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26788), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26813), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26785), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26815), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26787), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26814), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26786), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008AEBF0 */
static void compact_inverted_rc_track_diag_25_deg_up_to_60_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26800), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26797), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26799), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26798), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 16, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 16, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 16, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 16, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008AEC00 */
static void compact_inverted_rc_track_diag_60_deg_up_to_25_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26804), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26801), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26803), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26802), { -16, -16, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 61 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 21, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 21, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 21, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 21, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008AEBA0 */
static void compact_inverted_rc_track_diag_25_deg_up_to_flat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26820), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26792), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26817), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26789), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26819), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26791), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26818), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26790), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AEBE0 */
static void compact_inverted_rc_track_diag_25_deg_down(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26822), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26794), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26823), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26795), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26821), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26793), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26824), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26796), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 45 });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 50, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AEC40 */
static void compact_inverted_rc_track_diag_60_deg_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26806), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26807), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 93 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26805), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 93 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26808), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 93 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 24, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 28, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 24, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 28, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 120, 0x20);
            break;
    }
}

/** rct2: 0x008AEBC0 */
static void compact_inverted_rc_track_diag_flat_to_25_deg_down(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26818), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26790), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26819), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26791), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26817), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26789), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
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
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26820), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26792), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008AEC20 */
static void compact_inverted_rc_track_diag_25_deg_down_to_60_deg_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26802), { -16, -16, height + 29 },
                        { 16, 16, 3 }, { 0, 0, height + 61 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26803), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26801), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26804), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 17, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 17, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 17, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 17, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008AEC30 */
static void compact_inverted_rc_track_diag_60_deg_down_to_25_deg_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26798), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26799), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26797), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26800), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 61 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 8, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 8, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 8, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalBSupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 8, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
            break;
    }
}

/** rct2: 0x008AEBD0 */
static void compact_inverted_rc_track_diag_25_deg_down_to_flat(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26814), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26786), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26815), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26787), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26813), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26785), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26816), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26788), { -16, -16, height + 29 },
                            { 32, 32, 3 }, { -16, -16, height + 37 });
                        break;
                }
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008AEC70 */
static void compact_inverted_rc_track_diag_flat_to_left_bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26876), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26873), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26875), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26874), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF,
                0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEC80 */
static void compact_inverted_rc_track_diag_flat_to_right_bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26880), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26877), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26879), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26878), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF,
                0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEC90 */
static void compact_inverted_rc_track_diag_left_bank_to_flat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26878), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26879), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26877), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26880), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF,
                0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AECA0 */
static void compact_inverted_rc_track_diag_right_bank_to_flat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26874), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26875), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26873), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26876), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF,
                0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AECD0 */
static void compact_inverted_rc_track_diag_left_bank_to_25_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26892), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26889), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26891), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26890), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008AECE0 */
static void compact_inverted_rc_track_diag_right_bank_to_25_deg_up(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26896), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26893), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26895), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26894), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008AECB0 */
static void compact_inverted_rc_track_diag_25_deg_up_to_left_bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26884), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26881), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26883), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26882), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AECC0 */
static void compact_inverted_rc_track_diag_25_deg_up_to_right_bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26888), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26885), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26887), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26886), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
            break;
    }
}

/** rct2: 0x008AECF0 */
static void compact_inverted_rc_track_diag_left_bank_to_25_deg_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26886), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26887), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26885), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26888), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008AED00 */
static void compact_inverted_rc_track_diag_right_bank_to_25_deg_down(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26882), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26883), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26881), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26884), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 44, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
}

/** rct2: 0x008AED10 */
static void compact_inverted_rc_track_diag_25_deg_down_to_left_bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26894), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26895), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26893), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26896), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008AED20 */
static void compact_inverted_rc_track_diag_25_deg_down_to_right_bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26890), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentBC | SegmentC4 | SegmentCC | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26891), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentC4 | SegmentC8 | SegmentCC, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26889), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26892), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 37 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF, 0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 42, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
            break;
    }
}

/** rct2: 0x008AEC50 */
static void compact_inverted_rc_track_diag_left_bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26872), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26869), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26871), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26870), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB8 | SegmentC0 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF,
                0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEC60 */
static void compact_inverted_rc_track_diag_right_bank(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26870), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26871), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26869), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC0 | SegmentC4 | SegmentD0 | SegmentD4, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26872), { -16, -16, height + 29 },
                        { 32, 32, 3 }, { -16, -16, height + 29 });
                    break;
            }

            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentB4 | SegmentB8 | SegmentC4 | SegmentC8 | SegmentD0, direction), 0xFFFF,
                0);
            switch (direction)
            {
                case 0:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 1, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 0, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 2, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    MetalASupportsPaintSetup(
                        session, MetalSupportsTubesInverted, 3, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
            break;
    }
}

/** rct2: 0x008AEAA0 */
static void compact_inverted_rc_track_block_brakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26559), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(26560), { 0, 0, height + 29 }, { 32, 20, 3 },
                { 0, 6, height + 29 });
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentC4 | SegmentCC | SegmentD0, direction), 0xFFFF, 0);
    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        MetalASupportsPaintSetup(session, MetalSupportsTubesInverted, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilPushTunnelRotated(session, direction, height, TunnelInverted3);
    PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
}

TRACK_PAINT_FUNCTION get_track_paint_function_compact_inverted_rc(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return compact_inverted_rc_track_flat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return compact_inverted_rc_track_station;
        case TrackElemType::Up25:
            return compact_inverted_rc_track_25_deg_up;
        case TrackElemType::Up60:
            return compact_inverted_rc_track_60_deg_up;
        case TrackElemType::FlatToUp25:
            return compact_inverted_rc_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToUp60:
            return compact_inverted_rc_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return compact_inverted_rc_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return compact_inverted_rc_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return compact_inverted_rc_track_25_deg_down;
        case TrackElemType::Down60:
            return compact_inverted_rc_track_60_deg_down;
        case TrackElemType::FlatToDown25:
            return compact_inverted_rc_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToDown60:
            return compact_inverted_rc_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return compact_inverted_rc_track_60_deg_down_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return compact_inverted_rc_track_25_deg_down_to_flat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return compact_inverted_rc_track_left_quarter_turn_5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return compact_inverted_rc_track_right_quarter_turn_5;
        case TrackElemType::FlatToLeftBank:
            return compact_inverted_rc_track_flat_to_left_bank;
        case TrackElemType::FlatToRightBank:
            return compact_inverted_rc_track_flat_to_right_bank;
        case TrackElemType::LeftBankToFlat:
            return compact_inverted_rc_track_left_bank_to_flat;
        case TrackElemType::RightBankToFlat:
            return compact_inverted_rc_track_right_bank_to_flat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return compact_inverted_rc_track_banked_left_quarter_turn_5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return compact_inverted_rc_track_banked_right_quarter_turn_5;
        case TrackElemType::LeftBankToUp25:
            return compact_inverted_rc_track_left_bank_to_25_deg_up;
        case TrackElemType::RightBankToUp25:
            return compact_inverted_rc_track_right_bank_to_25_deg_up;
        case TrackElemType::Up25ToLeftBank:
            return compact_inverted_rc_track_25_deg_up_to_left_bank;
        case TrackElemType::Up25ToRightBank:
            return compact_inverted_rc_track_25_deg_up_to_right_bank;
        case TrackElemType::LeftBankToDown25:
            return compact_inverted_rc_track_left_bank_to_25_deg_down;
        case TrackElemType::RightBankToDown25:
            return compact_inverted_rc_track_right_bank_to_25_deg_down;
        case TrackElemType::Down25ToLeftBank:
            return compact_inverted_rc_track_25_deg_down_to_left_bank;
        case TrackElemType::Down25ToRightBank:
            return compact_inverted_rc_track_25_deg_down_to_right_bank;
        case TrackElemType::LeftBank:
            return compact_inverted_rc_track_left_bank;
        case TrackElemType::RightBank:
            return compact_inverted_rc_track_right_bank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return compact_inverted_rc_track_left_quarter_turn_5_25_deg_up;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return compact_inverted_rc_track_right_quarter_turn_5_25_deg_up;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return compact_inverted_rc_track_left_quarter_turn_5_25_deg_down;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return compact_inverted_rc_track_right_quarter_turn_5_25_deg_down;
        case TrackElemType::SBendLeft:
            return compact_inverted_rc_track_s_bend_left;
        case TrackElemType::SBendRight:
            return compact_inverted_rc_track_s_bend_right;
        case TrackElemType::LeftVerticalLoop:
            return compact_inverted_rc_track_left_vertical_loop;
        case TrackElemType::RightVerticalLoop:
            return compact_inverted_rc_track_right_vertical_loop;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return compact_inverted_rc_track_left_quarter_turn_3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return compact_inverted_rc_track_right_quarter_turn_3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return compact_inverted_rc_track_left_quarter_turn_3_bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return compact_inverted_rc_track_right_quarter_turn_3_bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return compact_inverted_rc_track_left_quarter_turn_3_25_deg_up;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return compact_inverted_rc_track_right_quarter_turn_3_25_deg_up;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return compact_inverted_rc_track_left_quarter_turn_3_25_deg_down;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return compact_inverted_rc_track_right_quarter_turn_3_25_deg_down;
        case TrackElemType::LeftTwistDownToUp:
            return compact_inverted_rc_track_left_twist_down_to_up;
        case TrackElemType::RightTwistDownToUp:
            return compact_inverted_rc_track_right_twist_down_to_up;
        case TrackElemType::LeftTwistUpToDown:
            return compact_inverted_rc_track_left_twist_up_to_down;
        case TrackElemType::RightTwistUpToDown:
            return compact_inverted_rc_track_right_twist_up_to_down;
        case TrackElemType::HalfLoopUp:
            return compact_inverted_rc_track_half_loop_up;
        case TrackElemType::HalfLoopDown:
            return compact_inverted_rc_track_half_loop_down;
        case TrackElemType::LeftCorkscrewUp:
            return compact_inverted_rc_track_left_corkscrew_up;
        case TrackElemType::RightCorkscrewUp:
            return compact_inverted_rc_track_right_corkscrew_up;
        case TrackElemType::LeftCorkscrewDown:
            return compact_inverted_rc_track_left_corkscrew_down;
        case TrackElemType::RightCorkscrewDown:
            return compact_inverted_rc_track_right_corkscrew_down;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return compact_inverted_rc_track_left_quarter_turn_1_60_deg_up;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return compact_inverted_rc_track_right_quarter_turn_1_60_deg_up;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return compact_inverted_rc_track_left_quarter_turn_1_60_deg_down;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return compact_inverted_rc_track_right_quarter_turn_1_60_deg_down;
        case TrackElemType::Brakes:
            return compact_inverted_rc_track_brakes;
        case TrackElemType::LeftQuarterBankedHelixLargeUp:
            return compact_inverted_rc_track_left_quarter_banked_helix_large_up;
        case TrackElemType::RightQuarterBankedHelixLargeUp:
            return compact_inverted_rc_track_right_quarter_banked_helix_large_up;
        case TrackElemType::LeftQuarterBankedHelixLargeDown:
            return compact_inverted_rc_track_left_quarter_banked_helix_large_down;
        case TrackElemType::RightQuarterBankedHelixLargeDown:
            return compact_inverted_rc_track_right_quarter_banked_helix_large_down;
        case TrackElemType::OnRidePhoto:
            return compact_inverted_rc_track_on_ride_photo;
        case TrackElemType::Up90:
            return compact_inverted_rc_track_90_deg_up;
        case TrackElemType::Down90:
            return compact_inverted_rc_track_90_deg_down;
        case TrackElemType::Up60ToUp90:
            return compact_inverted_rc_track_60_deg_up_to_90_deg_up;
        case TrackElemType::Down90ToDown60:
            return compact_inverted_rc_track_90_deg_down_to_60_deg_down;
        case TrackElemType::Up90ToUp60:
            return compact_inverted_rc_track_90_deg_up_to_60_deg_up;
        case TrackElemType::Down60ToDown90:
            return compact_inverted_rc_track_60_deg_down_to_90_deg_down;
        case TrackElemType::LeftEighthToDiag:
            return compact_inverted_rc_track_left_eighth_to_diag;
        case TrackElemType::RightEighthToDiag:
            return compact_inverted_rc_track_right_eighth_to_diag;
        case TrackElemType::LeftEighthToOrthogonal:
            return compact_inverted_rc_track_left_eighth_to_orthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return compact_inverted_rc_track_right_eighth_to_orthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return compact_inverted_rc_track_left_eighth_bank_to_diag;
        case TrackElemType::RightEighthBankToDiag:
            return compact_inverted_rc_track_right_eighth_bank_to_diag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return compact_inverted_rc_track_left_eighth_bank_to_orthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return compact_inverted_rc_track_right_eighth_bank_to_orthogonal;
        case TrackElemType::DiagFlat:
            return compact_inverted_rc_track_diag_flat;
        case TrackElemType::DiagUp25:
            return compact_inverted_rc_track_diag_25_deg_up;
        case TrackElemType::DiagUp60:
            return compact_inverted_rc_track_diag_60_deg_up;
        case TrackElemType::DiagFlatToUp25:
            return compact_inverted_rc_track_diag_flat_to_25_deg_up;
        case TrackElemType::DiagUp25ToUp60:
            return compact_inverted_rc_track_diag_25_deg_up_to_60_deg_up;
        case TrackElemType::DiagUp60ToUp25:
            return compact_inverted_rc_track_diag_60_deg_up_to_25_deg_up;
        case TrackElemType::DiagUp25ToFlat:
            return compact_inverted_rc_track_diag_25_deg_up_to_flat;
        case TrackElemType::DiagDown25:
            return compact_inverted_rc_track_diag_25_deg_down;
        case TrackElemType::DiagDown60:
            return compact_inverted_rc_track_diag_60_deg_down;
        case TrackElemType::DiagFlatToDown25:
            return compact_inverted_rc_track_diag_flat_to_25_deg_down;
        case TrackElemType::DiagDown25ToDown60:
            return compact_inverted_rc_track_diag_25_deg_down_to_60_deg_down;
        case TrackElemType::DiagDown60ToDown25:
            return compact_inverted_rc_track_diag_60_deg_down_to_25_deg_down;
        case TrackElemType::DiagDown25ToFlat:
            return compact_inverted_rc_track_diag_25_deg_down_to_flat;
        case TrackElemType::DiagFlatToLeftBank:
            return compact_inverted_rc_track_diag_flat_to_left_bank;
        case TrackElemType::DiagFlatToRightBank:
            return compact_inverted_rc_track_diag_flat_to_right_bank;
        case TrackElemType::DiagLeftBankToFlat:
            return compact_inverted_rc_track_diag_left_bank_to_flat;
        case TrackElemType::DiagRightBankToFlat:
            return compact_inverted_rc_track_diag_right_bank_to_flat;
        case TrackElemType::DiagLeftBankToUp25:
            return compact_inverted_rc_track_diag_left_bank_to_25_deg_up;
        case TrackElemType::DiagRightBankToUp25:
            return compact_inverted_rc_track_diag_right_bank_to_25_deg_up;
        case TrackElemType::DiagUp25ToLeftBank:
            return compact_inverted_rc_track_diag_25_deg_up_to_left_bank;
        case TrackElemType::DiagUp25ToRightBank:
            return compact_inverted_rc_track_diag_25_deg_up_to_right_bank;
        case TrackElemType::DiagLeftBankToDown25:
            return compact_inverted_rc_track_diag_left_bank_to_25_deg_down;
        case TrackElemType::DiagRightBankToDown25:
            return compact_inverted_rc_track_diag_right_bank_to_25_deg_down;
        case TrackElemType::DiagDown25ToLeftBank:
            return compact_inverted_rc_track_diag_25_deg_down_to_left_bank;
        case TrackElemType::DiagDown25ToRightBank:
            return compact_inverted_rc_track_diag_25_deg_down_to_right_bank;
        case TrackElemType::DiagLeftBank:
            return compact_inverted_rc_track_diag_left_bank;
        case TrackElemType::DiagRightBank:
            return compact_inverted_rc_track_diag_right_bank;
        case TrackElemType::BlockBrakes:
            return compact_inverted_rc_track_block_brakes;
    }
    return nullptr;
}
