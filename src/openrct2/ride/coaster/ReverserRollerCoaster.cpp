/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../drawing/Drawing.h"
#include "../../entity/EntityRegistry.h"
#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../paint/tile_element/Paint.TileElement.h"
#include "../../sprites.h"
#include "../../world/Map.h"
#include "../RideData.h"
#include "../TrackData.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
#include "../VehiclePaint.h"
namespace OpenRCT2
{
#ifndef NO_VEHICLES
    /**
     *
     *  rct2: 0x006D4453
     */
    void VehicleVisualReverser(
        PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
        const CarEntry* carEntry)
    {
        Vehicle* v1 = GetEntity<Vehicle>(vehicle->prev_vehicle_on_ride);
        Vehicle* v2 = GetEntity<Vehicle>(vehicle->next_vehicle_on_ride);
        if (v1 == nullptr || v2 == nullptr)
        {
            return;
        }
        x = (v1->x + v2->x) / 2;
        y = (v1->y + v2->y) / 2;
        z = (v1->z + v2->z) / 2;
        session.SpritePosition.x = x;
        session.SpritePosition.y = y;
        VehicleVisualDefault(session, imageDirection, z, vehicle, carEntry);
    }
#endif

    /** rct2: 0x0086E65C */
    static void ReverserRCTrackFlat(
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21520), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21521), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21504), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21505), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    static void ReverserRCTrackStation(
        PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][3] = {
            { 21506, SPR_STATION_BASE_A_SW_NE },
            { 21507, SPR_STATION_BASE_A_NW_SE },
            { 21506, SPR_STATION_BASE_A_SW_NE },
            { 21507, SPR_STATION_BASE_A_NW_SE },
        };

        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_MISC].WithIndex(imageIds[direction][1]), { 0, 0, height },
            { 32, 27, 2 }, { 0, 2, height });
        PaintAddImageAsChildRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(imageIds[direction][0]), { 0, 0, height },
            { 32, 27, 2 }, { 0, 2, height });
        WoodenASupportsPaintSetup(session, direction & 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, 9, 11);
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    /** rct2: 0x0086E66C */
    static void ReverserRCTrack25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21530), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21531), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21532), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21533), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21516), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21517), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21518), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 11, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21519), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 12, height, session.TrackColours[SCHEME_SUPPORTS]);
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

    /** rct2: 0x0086E67C */
    static void ReverserRCTrackFlatTo25DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21522), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21523), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21524), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21525), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21508), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 1, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21509), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 2, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21510), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21511), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 4, height, session.TrackColours[SCHEME_SUPPORTS]);
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

    /** rct2: 0x0086E68C */
    static void ReverserRCTrack25DegUpToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        if (trackElement.HasChain())
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21526), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21527), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21528), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21529), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
            }
        }
        else
        {
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21512), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21513), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21514), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 0, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21515), { 0, 0, height },
                        { 32, 27, 2 }, { 0, 2, height });
                    WoodenASupportsPaintSetup(session, 1, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
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

    /** rct2: 0x0086E69C */
    static void ReverserRCTrack25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        ReverserRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    /** rct2: 0x0086E6AC */
    static void ReverserRCTrackFlatTo25DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        ReverserRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    /** rct2: 0x0086E6BC */
    static void ReverserRCTrack25DegDownToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        ReverserRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    /** rct2: 0x0086E6CC */
    static void ReverserRCTrackLeftQuarterTurn5(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21543), { 0, 2, height },
                            { 32, 27, 2 });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21548), { 0, 2, height },
                            { 32, 27, 2 });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21553), { 0, 2, height },
                            { 32, 32, 2 });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21538), { 0, 2, height },
                            { 32, 32, 2 });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21542), { 0, 0, height },
                            { 32, 16, 2 });
                        WoodenASupportsPaintSetup(session, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21547), { 0, 0, height },
                            { 32, 16, 2 });
                        WoodenASupportsPaintSetup(session, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21552), { 0, 16, height },
                            { 32, 16, 2 });
                        WoodenASupportsPaintSetup(session, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21537), { 0, 16, height },
                            { 32, 16, 2 });
                        WoodenASupportsPaintSetup(session, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21541), { 0, 16, height },
                            { 16, 16, 2 });
                        WoodenASupportsPaintSetup(session, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21546), { 16, 16, height },
                            { 16, 16, 2 });
                        WoodenASupportsPaintSetup(session, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21551), { 16, 0, height },
                            { 16, 16, 2 });
                        WoodenASupportsPaintSetup(session, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21536), { 0, 0, height },
                            { 16, 16, 2 });
                        WoodenASupportsPaintSetup(session, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21540), { 16, 0, height },
                            { 16, 34, 2 });
                        WoodenASupportsPaintSetup(session, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21545), { 0, 0, height },
                            { 16, 32, 2 });
                        WoodenASupportsPaintSetup(session, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21550), { 0, 0, height },
                            { 16, 32, 2 });
                        WoodenASupportsPaintSetup(session, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21535), { 16, 0, height },
                            { 16, 32, 2 });
                        WoodenASupportsPaintSetup(session, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21539), { 2, 0, height },
                            { 32, 32, 2 });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21544), { 2, 0, height },
                            { 27, 32, 2 });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21549), { 2, 0, height },
                            { 27, 32, 2 });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21534), { 2, 0, height },
                            { 32, 32, 2 });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

    /** rct2: 0x0086E6DC */
    static void ReverserRCTrackRightQuarterTurn5(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        ReverserRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
    }

    /** rct2: 0x0086E6EC */
    static void ReverserRCTrackSBendLeft(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21566), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21570), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21569), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21573), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21567), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 0, height });
                        WoodenASupportsPaintSetup(session, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21571), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 0, height });
                        WoodenASupportsPaintSetup(session, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21568), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 6, height });
                        WoodenASupportsPaintSetup(session, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21572), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 6, height });
                        WoodenASupportsPaintSetup(session, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21568), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 6, height });
                        WoodenASupportsPaintSetup(session, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21572), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 6, height });
                        WoodenASupportsPaintSetup(session, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21567), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 0, height });
                        WoodenASupportsPaintSetup(session, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21571), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 0, height });
                        WoodenASupportsPaintSetup(session, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21569), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21573), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21566), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21570), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

    /** rct2: 0x0086E6FC */
    static void ReverserRCTrackSBendRight(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21574), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21578), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21577), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21581), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21575), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 6, height });
                        WoodenASupportsPaintSetup(session, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21579), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 6, height });
                        WoodenASupportsPaintSetup(session, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21576), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 0, height });
                        WoodenASupportsPaintSetup(session, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21580), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 0, height });
                        WoodenASupportsPaintSetup(session, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21576), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 0, height });
                        WoodenASupportsPaintSetup(session, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21580), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 0, height });
                        WoodenASupportsPaintSetup(session, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21575), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 6, height });
                        WoodenASupportsPaintSetup(session, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21579), { 0, 0, height },
                            { 32, 26, 2 }, { 0, 6, height });
                        WoodenASupportsPaintSetup(session, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21577), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21581), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21574), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21578), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

    /** rct2: 0x0086E73C */
    static void ReverserRCTrackLeftQuarterTurn3(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21559), { 0, 0, height },
                            { 32, 20, 2 }, { 0, 6, height });
                        WoodenASupportsPaintSetup(session, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21562), { 0, 0, height },
                            { 32, 20, 2 }, { 0, 6, height });
                        WoodenASupportsPaintSetup(session, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21565), { 0, 0, height },
                            { 32, 20, 2 }, { 0, 6, height });
                        WoodenASupportsPaintSetup(session, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21556), { 0, 0, height },
                            { 32, 20, 2 }, { 0, 6, height });
                        WoodenASupportsPaintSetup(session, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21558), { 0, 0, height },
                            { 16, 16, 2 }, { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21561), { 0, 0, height },
                            { 16, 16, 2 }, { 0, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21564), { 0, 0, height },
                            { 16, 16, 2 }, { 0, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21555), { 0, 0, height },
                            { 16, 16, 2 }, { 16, 16, height });
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21557), { 0, 0, height },
                            { 20, 32, 2 }, { 6, 0, height });
                        WoodenASupportsPaintSetup(session, 5, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21560), { 0, 0, height },
                            { 20, 32, 2 }, { 6, 0, height });
                        WoodenASupportsPaintSetup(session, 2, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21563), { 0, 0, height },
                            { 20, 32, 2 }, { 6, 0, height });
                        WoodenASupportsPaintSetup(session, 3, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21554), { 0, 0, height },
                            { 20, 32, 2 }, { 6, 0, height });
                        WoodenASupportsPaintSetup(session, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

    /** rct2: 0x0086E74C */
    static void ReverserRCTrackRightQuarterTurn3(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        trackSequence = mapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        ReverserRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
    }

    /** rct2: 0x0086E75C */
    static void ReverserRCTrackBrakes(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21506), { 0, 0, height }, { 32, 27, 2 },
                    { 0, 2, height });
                WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21507), { 0, 0, height }, { 32, 27, 2 },
                    { 0, 2, height });
                WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    /** rct2: 0x0086E76C */
    static void ReverserRCTrackLeftReverser(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21582), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21588), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21594), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21600), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21585), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21591), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21597), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21603), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21586), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21592), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21598), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21604), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21587), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21593), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21599), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21605), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21583), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21589), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21595), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21601), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21584), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21590), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21596), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21602), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

    /** rct2: 0x0086E77C */
    static void ReverserRCTrackRightReverser(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21596), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21602), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21584), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21590), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21599), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21605), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21587), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21593), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21598), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21604), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21586), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21592), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21597), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21603), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21585), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21591), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21595), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21601), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21583), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21589), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21594), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21600), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21582), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(21588), { 0, 0, height },
                            { 32, 27, 2 }, { 0, 2, height });
                        WoodenASupportsPaintSetup(session, 1, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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

    TRACK_PAINT_FUNCTION GetTrackPaintFunctionReverserRC(int32_t trackType)
    {
        switch (trackType)
        {
            case TrackElemType::Flat:
                return ReverserRCTrackFlat;
            case TrackElemType::EndStation:
            case TrackElemType::BeginStation:
            case TrackElemType::MiddleStation:
                return ReverserRCTrackStation;
            case TrackElemType::Up25:
                return ReverserRCTrack25DegUp;
            case TrackElemType::FlatToUp25:
                return ReverserRCTrackFlatTo25DegUp;
            case TrackElemType::Up25ToFlat:
                return ReverserRCTrack25DegUpToFlat;
            case TrackElemType::Down25:
                return ReverserRCTrack25DegDown;
            case TrackElemType::FlatToDown25:
                return ReverserRCTrackFlatTo25DegDown;
            case TrackElemType::Down25ToFlat:
                return ReverserRCTrack25DegDownToFlat;
            case TrackElemType::LeftQuarterTurn5Tiles:
                return ReverserRCTrackLeftQuarterTurn5;
            case TrackElemType::RightQuarterTurn5Tiles:
                return ReverserRCTrackRightQuarterTurn5;
            case TrackElemType::SBendLeft:
                return ReverserRCTrackSBendLeft;
            case TrackElemType::SBendRight:
                return ReverserRCTrackSBendRight;
            case TrackElemType::LeftQuarterTurn3Tiles:
                return ReverserRCTrackLeftQuarterTurn3;
            case TrackElemType::RightQuarterTurn3Tiles:
                return ReverserRCTrackRightQuarterTurn3;
            case TrackElemType::Brakes:
                return ReverserRCTrackBrakes;
            case TrackElemType::LeftReverser:
                return ReverserRCTrackLeftReverser;
            case TrackElemType::RightReverser:
                return ReverserRCTrackRightReverser;
        }
        return nullptr;
    }
} // namespace OpenRCT2
