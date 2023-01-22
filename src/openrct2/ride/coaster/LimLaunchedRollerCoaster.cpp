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
namespace OpenRCT2
{
    static constexpr const uint32_t LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_OPEN = 15020;
    static constexpr const uint32_t LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_OPEN = 15021;
    static constexpr const uint32_t LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_CLOSED = 15022;
    static constexpr const uint32_t LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_CLOSED = 15023;

    static constexpr const uint32_t _LimLauncherBlockBrakeImages[NumOrthogonalDirections][2] = {
        { LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_OPEN, LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_CLOSED },
        { LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_OPEN, LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_CLOSED },
        { LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_OPEN, LIM_LAUNCHED_RC_BLOCK_BRAKE_SW_NE_CLOSED },
        { LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_OPEN, LIM_LAUNCHED_RC_BLOCK_BRAKE_NW_SE_CLOSED },
    };

    /** rct2: 0x008A6D50, 0x008A6D60, 0x008A6D70 */
    static void LimLaunchedRCTrackStation(
        PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        static constexpr const uint32_t imageIds[4][2] = {
            { 15018, SPR_STATION_BASE_B_SW_NE },
            { 15019, SPR_STATION_BASE_B_NW_SE },
            { 15018, SPR_STATION_BASE_B_SW_NE },
            { 15019, SPR_STATION_BASE_B_NW_SE },
        };

        if (trackElement.GetTrackType() == TrackElemType::EndStation)
        {
            bool isClosed = trackElement.IsBrakeClosed();
            PaintAddImageAsParentRotated(
                session, direction,
                session.TrackColours[SCHEME_TRACK].WithIndex(_LimLauncherBlockBrakeImages[direction][isClosed]),
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
        TrackPaintUtilDrawStation(session, ride, direction, height, trackElement);
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_SQUARE_FLAT);
        PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    /** rct2: 0x008A65E0 */
    static void LimLaunchedRCTrackLeftVerticalLoop(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15388), { 0, 6, height },
                            { 32, 20, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15396), { 0, 6, height },
                            { 32, 20, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15395), { 0, 6, height },
                            { 32, 20, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15403), { 0, 6, height },
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
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15389), { 0, 0, height },
                            { 32, 26, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15397), { 0, 14, height },
                            { 32, 2, 63 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15394), { 0, 6, height },
                            { 32, 26, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15402), { 0, 6, height },
                            { 32, 26, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15390), { 16, 0, height },
                            { 3, 16, 119 }, { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15398), { 12, 0, height },
                            { 3, 16, 119 }, { 12, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15393), { 10, 16, height },
                            { 4, 16, 119 }, { 10, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15401), { 16, 16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15391), { 0, 0, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15399), { 0, 0, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15392), { 0, 16, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15400), { 0, 16, height + 32 },
                            { 32, 16, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15392), { 0, 16, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15400), { 0, 16, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15391), { 0, 0, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15399), { 0, 0, height + 32 },
                            { 32, 16, 3 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 7:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15393), { 10, 16, height },
                            { 4, 16, 119 }, { 10, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15401), { 16, 16, height },
                            { 2, 16, 119 }, { 16, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15390), { 16, 0, height },
                            { 3, 16, 119 }, { 16, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15398), { 12, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15394), { 0, 6, height },
                            { 32, 26, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15402), { 0, 6, height },
                            { 32, 26, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15389), { 0, 0, height },
                            { 32, 26, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15397), { 0, 14, height },
                            { 32, 2, 63 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 9:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15395), { 0, 6, height },
                            { 32, 20, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15403), { 0, 6, height },
                            { 32, 20, 7 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15388), { 0, 6, height },
                            { 32, 20, 3 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15396), { 0, 6, height },
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
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
        }
    }

    /** rct2: 0x008A65F0 */
    static void LimLaunchedRCTrackRightVerticalLoop(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15419), { 0, 6, height },
                            { 32, 20, 7 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15411), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15412), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15404), { 0, 6, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15418), { 0, 6, height },
                            { 32, 26, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15410), { 0, 6, height },
                            { 32, 26, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15413), { 0, 14, height },
                            { 32, 2, 63 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15405), { 0, 0, height },
                            { 32, 26, 3 });
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15417), { 16, 16, height },
                            { 2, 16, 119 }, { 16, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15409), { 10, 16, height },
                            { 4, 16, 119 }, { 10, 16, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15414), { 12, 0, height },
                            { 3, 16, 119 }, { 12, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15406), { 16, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15416), { 0, 16, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15408), { 0, 16, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15415), { 0, 0, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15407), { 0, 0, height + 32 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15415), { 0, 0, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15407), { 0, 0, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15416), { 0, 16, height + 32 },
                            { 32, 16, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15408), { 0, 16, height + 32 },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15414), { 12, 0, height },
                            { 3, 16, 119 }, { 12, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15406), { 16, 0, height },
                            { 2, 16, 119 }, { 16, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15417), { 16, 16, height },
                            { 2, 16, 119 }, { 16, 16, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15409), { 10, 16, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15413), { 0, 14, height },
                            { 32, 2, 63 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15405), { 0, 0, height },
                            { 32, 26, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15418), { 0, 6, height },
                            { 32, 26, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15410), { 0, 6, height },
                            { 32, 26, 3 });
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 9:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15412), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15404), { 0, 6, height },
                            { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15419), { 0, 6, height },
                            { 32, 20, 7 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15411), { 0, 6, height },
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

    /** rct2: 0x008A6D10 */
    static void LimLaunchedRCTrackLeftTwistDownToUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15758), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15782), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15761), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15785), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15764), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15788), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15767), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15791), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
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
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15759), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15783), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15762), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15786), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15765), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15789), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15768), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15792), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15760), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15784), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15763), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15787), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15766), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15790), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15769), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15793), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                }
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height - 32, TUNNEL_INVERTED_3);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height - 32, TUNNEL_INVERTED_3);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height, 0x20);
                break;
        }
    }

    /** rct2: 0x008A6D20 */
    static void LimLaunchedRCTrackRightTwistDownToUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15770), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15794), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15773), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15797), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15776), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15800), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15779), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15803), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15771), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15795), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15774), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15798), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15777), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15801), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15780), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15804), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15772), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15796), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15775), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15799), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15778), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15802), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15781), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15805), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                }
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height + 32, TUNNEL_INVERTED_3);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height + 32, TUNNEL_INVERTED_3);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height, 0x20);
                break;
        }
    }

    /** rct2: 0x008A6D30 */
    static void LimLaunchedRCTrackLeftTwistUpToDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15766), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15790), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15769), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15793), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15760), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15784), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15763), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15787), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 32, TUNNEL_INVERTED_3);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15765), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15789), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15768), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15792), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15759), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15783), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15762), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15786), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15764), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15788), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15767), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15791), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15758), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15782), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15761), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15785), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
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
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }

    /** rct2: 0x008A6D40 */
    static void LimLaunchedRCTrackRightTwistUpToDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15778), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15802), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15781), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15805), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15772), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15796), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15775), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15799), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 32, TUNNEL_INVERTED_3);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15777), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15801), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15780), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15804), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15771), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15795), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15774), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15798), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15776), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15800), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15779), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15803), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15770), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15794), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15773), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15797), { 0, 0, height },
                            { 32, 20, 0 }, { 0, 6, height + 28 });
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
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
                break;
        }
    }

    /** rct2: 0x008A6CD0 */
    static void LimLaunchedRCTrackLeftCorkscrewUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15734), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height + 4 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15737), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height + 4 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15740), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height + 4 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15743), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15735), { 0, 0, height },
                            { 20, 20, 3 }, { 6, 6, height + 10 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15738), { 0, 0, height },
                            { 20, 20, 3 }, { 6, 6, height + 10 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15741), { 0, 0, height },
                            { 20, 20, 3 }, { 6, 6, height + 10 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15744), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15736), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15739), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15742), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15745), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

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

    /** rct2: 0x008A6CE0 */
    static void LimLaunchedRCTrackRightCorkscrewUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15746), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height + 4 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15749), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height + 4 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15752), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height + 4 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15755), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15747), { 0, 0, height },
                            { 20, 20, 3 }, { 6, 6, height + 10 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15750), { 0, 0, height },
                            { 20, 20, 3 }, { 6, 6, height + 10 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15753), { 0, 0, height },
                            { 20, 20, 3 }, { 6, 6, height + 10 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15756), { 0, 0, height },
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15748), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15751), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15754), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15757), { 0, 0, height },
                            { 20, 32, 3 }, { 6, 0, height + 24 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES, 4, 0, height + 35, session.TrackColours[SCHEME_SUPPORTS]);

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

    /** rct2: 0x008A6CF0 */
    static void LimLaunchedRCTrackLeftCorkscrewDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackRightCorkscrewUp(session, ride, 2 - trackSequence, (direction + 1) & 3, height, trackElement);
    }

    /** rct2: 0x008A6D00 */
    static void LimLaunchedRCTrackRightCorkscrewDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackLeftCorkscrewUp(session, ride, 2 - trackSequence, (direction - 1) & 3, height, trackElement);
    }

    /** rct2: 0x008A6D80 */
    static void LimLaunchedRCTrackBrakes(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15018), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15019), { 0, 0, height }, { 32, 20, 3 },
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

    /** rct2: 0x008A6C10 */
    static void LimLaunchedRCTrack90DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15702), { 0, 0, height },
                            { 2, 20, 31 }, { 4, 6, height + 8 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15703), { 0, 0, height },
                            { 2, 20, 31 }, { 24, 6, height + 8 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15704), { 0, 0, height },
                            { 2, 20, 31 }, { 24, 6, height + 8 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15705), { 0, 0, height },
                            { 2, 20, 31 }, { 4, 6, height + 8 });
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

    /** rct2: 0x008A6C20 */
    static void LimLaunchedRCTrack90DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrack90DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    /** rct2: 0x008A6C30 */
    static void LimLaunchedRCTrack60DegUpTo90DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15694), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15695), { 0, 0, height },
                            { 2, 20, 55 }, { 24, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15696), { 0, 0, height },
                            { 2, 20, 55 }, { 24, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15697), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
                }
                PaintUtilSetVerticalTunnel(session, height + 56);
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
            case 1:
                break;
        }
    }

    /** rct2: 0x008A6C40 */
    static void LimLaunchedRCTrack90DegDownTo60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrack60DegUpTo90DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    /** rct2: 0x008A6C50 */
    static void LimLaunchedRCTrack90DegUpTo60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15698), { 0, 0, height }, { 6, 20, 3 },
                    { 0, 6, height + 2 });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15699), { 0, 0, height }, { 2, 20, 31 },
                    { 39, 6, height + 8 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15700), { 0, 0, height }, { 2, 20, 31 },
                    { 39, 6, height + 8 });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15701), { 0, 0, height }, { 6, 20, 3 },
                    { 0, 6, height + 2 });
                break;
        }
        switch (direction)
        {
            case 1:
                PaintUtilPushTunnelRight(session, height + 48, TUNNEL_2);
                break;
            case 2:
                PaintUtilPushTunnelLeft(session, height + 48, TUNNEL_2);
                break;
        }
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
    }

    /** rct2: 0x008A6C60 */
    static void LimLaunchedRCTrack60DegDownTo90DegDown(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15700), { 0, 0, height },
                            { 2, 20, 31 }, { 39, 6, height + 8 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15701), { 0, 0, height },
                            { 6, 20, 3 }, { 0, 6, height + 2 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15698), { 0, 0, height },
                            { 6, 20, 3 }, { 0, 6, height + 2 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15699), { 0, 0, height },
                            { 2, 20, 31 }, { 39, 6, height + 8 });
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 48, TUNNEL_2);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
                break;
            case 1:
                break;
        }
    }

    /** rct2: 0x008A6C70 */
    static void LimLaunchedRCTrack90DegToInvertedFlatQuarterLoopUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15722), { 0, 0, height },
                            { 2, 20, 31 }, { 4, 6, height + 8 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15725), { 0, 0, height },
                            { 2, 20, 31 }, { 24, 6, height + 8 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15728), { 0, 0, height },
                            { 2, 20, 31 }, { 24, 6, height + 8 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15731), { 0, 0, height },
                            { 2, 20, 31 }, { 4, 6, height + 8 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15723), { 0, 0, height },
                            { 2, 20, 31 }, { -8, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15726), { 0, 0, height },
                            { 2, 20, 63 }, { 24, 6, height + 8 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15729), { 0, 0, height },
                            { 2, 20, 63 }, { 24, 6, height + 8 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15732), { 0, 0, height },
                            { 2, 20, 31 }, { -8, 6, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15724), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15727), { 0, 0, height },
                            { 2, 20, 31 }, { 24, 6, height + 8 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15730), { 0, 0, height },
                            { 2, 20, 31 }, { 24, 6, height + 8 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15733), { 0, 0, height },
                            { 32, 20, 3 }, { 0, 6, height + 24 });
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 16, TUNNEL_0);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    /** rct2: 0x008A6C80 */
    static void LimLaunchedRCTrackInvertedFlatTo90DegQuarterLoopDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrack90DegToInvertedFlatQuarterLoopUp(session, ride, 2 - trackSequence, direction, height, trackElement);
    }

    /** rct2: 0x008A6D90 */
    static void LimLaunchedRCTrackBlockBrakes(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        bool isClosed = trackElement.IsBrakeClosed();
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(_LimLauncherBlockBrakeImages[direction][isClosed]),
            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });

        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
        }
        PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
    }

    static void LimLaunchedRCTrackLeftQuarterTurn190DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15706), { 0, 0, height },
                            { 2, 20, 63 }, { 4, 6, height + 8 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15707), { 0, 0, height },
                            { 2, 20, 63 }, { 4, 6, height + 8 });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15715), { 0, 0, height },
                            { 2, 20, 63 }, { 24, 6, height + 8 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15716), { 0, 0, height },
                            { 2, 2, 63 }, { 24, 24, height + 8 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15709), { 0, 0, height },
                            { 2, 20, 63 }, { 4, 6, height + 8 });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15717), { 0, 0, height },
                            { 2, 20, 63 }, { 24, 6, height + 8 });
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

    /** rct2: 0x008A6CA0 */
    static void LimLaunchedRCTrackRightQuarterTurn190DegUp(
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
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15710), { 0, 0, height },
                            { 2, 20, 63 }, { 4, 6, height + 8 });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15718), { 0, 0, height },
                            { 2, 20, 63 }, { 24, 6, height + 8 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15719), { 0, 0, height },
                            { 2, 2, 63 }, { 24, 24, height + 8 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15712), { 0, 0, height },
                            { 2, 20, 63 }, { 4, 6, height + 8 });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15720), { 0, 0, height },
                            { 2, 20, 63 }, { 24, 6, height + 8 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15713), { 0, 0, height },
                            { 2, 20, 63 }, { 4, 6, height + 8 });
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

    /** rct2: 0x008A6CB0 */
    static void LimLaunchedRCTrackLeftQuarterTurn190DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackRightQuarterTurn190DegUp(session, ride, trackSequence, (direction + 1) & 3, height, trackElement);
    }

    /** rct2: 0x008A6CC0 */
    static void LimLaunchedRCTrackRightQuarterTurn190DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackLeftQuarterTurn190DegUp(session, ride, trackSequence, (direction - 1) & 3, height, trackElement);
    }

    static void LimLaunchedRCTrackLeftBarrelRollUpToDown(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 0),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 1),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 4, height + 1, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 6),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 7),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height + 1, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 12),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 13),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 0, height + 1, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 18),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 19),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
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
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 2),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 3),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 8),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 9),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 14),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 15),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 20),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 21),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 4),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 5),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 10),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 11),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 16),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 17),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 22),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 23),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                }
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackRightBarrelRollUpToDown(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 24),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 25),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 0, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 30),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 31),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 0, height + 1, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 36),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 37),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 4, height + 1, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 42),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 43),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 4, height + 1, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
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
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 26),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 27),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 32),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 33),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 38),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 39),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 44),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 45),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 28 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 28),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 29),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 34),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 35),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 40),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 41),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 46),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 47),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 44 });
                        break;
                }
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_INVERTED_3);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_INVERTED_3);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackLeftBarrelRollDownToUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackLeftBarrelRollUpToDown(session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void LimLaunchedRCTrackRightBarrelRollDownToUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackRightBarrelRollUpToDown(session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void LimLaunchedRCTrackHalfLoopUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 0),
                            { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 4),
                            { 0, 6, height }, { 32, 20, 11 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 8),
                            { 0, 6, height }, { 32, 20, 9 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 12),
                            { 0, 6, height }, { 32, 20, 7 });
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 1),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 5),
                            { 0, 14, height }, { 3, 20, 63 }, { 28, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 9),
                            { 0, 6, height }, { 3, 20, 63 }, { 28, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 13),
                            { 0, 6, height }, { 32, 20, 3 });
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 2),
                            { 16, 0, height }, { 2, 16, 119 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 6),
                            { 12, 0, height }, { 3, 16, 119 }, { 12, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 10),
                            { 10, 16, height }, { 4, 12, 119 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 14),
                            { 16, 16, height }, { 2, 16, 119 }, { 15, 6, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 168, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 3),
                            { 0, 0, height + 32 }, { 32, 16, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 7),
                            { 0, 0, height + 32 }, { 32, 16, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 11),
                            { 0, 16, height + 32 }, { 32, 12, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 15),
                            { 0, 16, height + 32 }, { 32, 12, 3 });
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

    static void LimLaunchedRCTrackHalfLoopDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackHalfLoopUp(session, ride, 3 - trackSequence, direction, height, trackElement);
    }

    static void LimLaunchedRCTrackFlatTo60DegUpLongBase(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 0),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 4),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 8),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 12),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 1),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 5),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 9),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 13),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 2),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 6),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 10),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 14),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 10, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 3),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 7),
                            { 0, 0, height }, { 32, 1, 98 }, { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 11),
                            { 0, 0, height }, { 32, 1, 98 }, { 0, 27, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 15),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 19, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height + 24, TUNNEL_2);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height + 24, TUNNEL_2);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 80, 0x20);
                break;
        }
    }
    static void LimLaunchedRCTrack60DegUpToFlatLongBase(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 16),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 20),
                            { 0, 0, height }, { 32, 1, 98 }, { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 24),
                            { 0, 0, height }, { 32, 1, 98 }, { 0, 27, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 28),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_1);
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 17),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 21),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 25),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 29),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 18, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 18),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 22),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 26),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 30),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 13, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 19),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 23),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 27),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 31),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetup(
                        session, METAL_SUPPORTS_TUBES, 4, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
                }
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackFlatTo60DegDownLongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrack60DegUpToFlatLongBase(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void LimLaunchedRCTrack60DegDownToFlatLongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackFlatTo60DegUpLongBase(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void LimLaunchedRCTrackLeftLargeCorkscrewUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 0),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 4 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 5),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 4 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 10),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 4 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 15),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 4 });
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
                PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 1),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 6, 34, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 6),
                            { 0, 0, height }, { 26, 1, 32 }, { 0, 29, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 8, 22, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 11),
                            { 0, 0, height }, { 20, 20, 3 }, { 0, 6, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 7, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 16),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 5, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 2),
                            { 0, 0, height }, { 48, 1, 64 }, { 0, 31, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 7),
                            { 0, 0, height }, { 28, 28, 1 }, { 2, 2, height + 40 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 12),
                            { 0, 0, height }, { 28, 28, 1 }, { 2, 2, height + 48 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 17),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 3:
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 3),
                            { 0, 0, height }, { 28, 28, 0 }, { 2, 2, height + 50 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 5, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 8),
                            { 0, 0, height }, { 28, 28, 0 }, { 2, 2, height + 50 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 6, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 13),
                            { 0, 0, height }, { 24, 28, 0 }, { 2, 2, height + 50 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 8, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 18),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 7, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 4),
                            { 0, 0, height }, { 28, 28, 1 }, { 2, 2, height + 40 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 9),
                            { 0, 0, height }, { 28, 28, 1 }, { 2, 2, height + 40 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 14),
                            { 0, 0, height }, { 26, 28, 1 }, { 2, 2, height + 40 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 19),
                            { 0, 0, height }, { 26, 28, 1 }, { 2, 2, height + 40 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4, direction),
                    0xFFFF, 0);
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

    static void LimLaunchedRCTrackRightLargeCorkscrewUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 20),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 4 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 25),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 4 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 30),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 4 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 35),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 4 });
                        break;
                }

                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 2, height, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_D4 | SEGMENT_C4 | SEGMENT_BC | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 21),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 6, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 26),
                            { 0, 0, height }, { 20, 20, 3 }, { 0, 6, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 8, 24, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 31),
                            { 0, 0, height }, { 26, 1, 32 }, { 0, 29, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 7, 22, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 36),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 5, 34, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 22),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 27),
                            { 0, 0, height }, { 28, 28, 1 }, { 2, 2, height + 48 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 32),
                            { 0, 0, height }, { 28, 28, 1 }, { 2, 2, height + 48 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 37),
                            { 0, 0, height }, { 48, 1, 64 }, { 0, 31, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 3:
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 23),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 8, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 28),
                            { 0, 0, height }, { 24, 28, 0 }, { 2, 2, height + 50 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 7, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 33),
                            { 0, 0, height }, { 28, 28, 0 }, { 2, 2, height + 50 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 5, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 38),
                            { 0, 0, height }, { 28, 28, 0 }, { 2, 2, height + 50 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 6, 0, height + 56, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 24),
                            { 0, 0, height }, { 26, 28, 1 }, { 2, 2, height + 40 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 29),
                            { 0, 0, height }, { 26, 28, 1 }, { 2, 2, height + 40 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 34),
                            { 0, 0, height }, { 28, 28, 1 }, { 2, 2, height + 40 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 39),
                            { 0, 0, height }, { 28, 28, 1 }, { 2, 2, height + 40 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4, direction),
                    0xFFFF, 0);
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

    static void LimLaunchedRCTrackLeftLargeCorkscrewDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackRightLargeCorkscrewUp(session, ride, 5 - trackSequence, (direction + 1) & 3, height, trackElement);
    }

    static void LimLaunchedRCTrackRightLargeCorkscrewDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackLeftLargeCorkscrewUp(session, ride, 5 - trackSequence, (direction - 1) & 3, height, trackElement);
    }

    static void LimLaunchedRCTrackLeftMediumHalfLoopUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 0),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 5),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 10),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 15),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }

                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 1),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 6),
                            { 0, 0, height }, { 0, 32, 64 }, { 0, 31, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 11),
                            { 0, 0, height }, { 40, 0, 64 }, { 0, 32, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 16),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_B4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 2),
                            { 0, 0, height }, { 32, 32, 0 }, { 0, 0, height + 2 });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 5, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 7),
                            { 0, 0, height }, { 1, 32, 96 }, { 29, 0, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 6, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 12),
                            { 0, 0, height }, { 1, 32, 96 }, { 31, 0, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 8, 18, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 17),
                            { 0, 0, height }, { 32, 32, 0 }, { 0, 0, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 7, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 144, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 3),
                            { 0, 0, height }, { 0, 32, 160 }, { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 8),
                            { 0, 0, height }, { 0, 32, 160 }, { 30, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 13),
                            { 0, 0, height }, { 1, 32, 160 }, { 29, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 18),
                            { 0, 0, height }, { 1, 32, 160 }, { 0, 0, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 144, 0x20);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 4),
                            { 0, 0, height }, { 32, 32, 1 }, { 0, 2, height + 48 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 9),
                            { 0, 0, height }, { 32, 32, 1 }, { 0, 2, height + 48 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 14),
                            { 0, 0, height }, { 32, 32, 1 }, { 0, 0, height + 48 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 19),
                            { 0, 0, height }, { 32, 32, 1 }, { 0, 0, height + 48 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_CC | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC, direction),
                    0xFFFF, 0);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 16, TUNNEL_0);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackRightMediumHalfLoopUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 20),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 25),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 30),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 35),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }

                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 7, height, session.TrackColours[SCHEME_SUPPORTS]);

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 21),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 26),
                            { 0, 0, height }, { 40, 0, 64 }, { 0, 32, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 31),
                            { 0, 0, height }, { 0, 32, 64 }, { 0, 31, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 36),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_CC | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 22),
                            { 0, 0, height }, { 32, 32, 0 }, { 0, 0, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 8, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 27),
                            { 0, 0, height }, { 0, 32, 96 }, { 30, 0, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 7, 18, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 32),
                            { 0, 0, height }, { 0, 32, 96 }, { 29, 0, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 5, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 37),
                            { 0, 0, height }, { 32, 32, 0 }, { 0, 0, height + 2 });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 6, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_D4 | SEGMENT_C4 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 144, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 23),
                            { 0, 0, height }, { 0, 32, 160 }, { -12, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 28),
                            { 0, 0, height }, { 0, 32, 160 }, { 29, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 33),
                            { 0, 0, height }, { 0, 32, 160 }, { 30, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 38),
                            { 0, 0, height }, { 1, 32, 160 }, { 2, 0, height });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 144, 0x20);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 24),
                            { 0, 0, height }, { 32, 32, 1 }, { 0, 0, height + 48 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 29),
                            { 0, 0, height }, { 32, 32, 1 }, { 0, 0, height + 48 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 34),
                            { 0, 0, height }, { 32, 32, 1 }, { 0, 0, height + 48 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 39),
                            { 0, 0, height }, { 32, 32, 1 }, { 0, 0, height + 48 });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_B4, direction),
                    0xFFFF, 0);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 16, TUNNEL_0);
                }
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackLeftMediumHalfLoopDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackRightMediumHalfLoopUp(session, ride, 4 - trackSequence, direction, height, trackElement);
    }

    static void LimLaunchedRCTrackRightMediumHalfLoopDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackLeftMediumHalfLoopUp(session, ride, 4 - trackSequence, direction, height, trackElement);
    }

    static void LimLaunchedRCTrackLeftZeroGRollUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 0),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 1),
                            { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height + 28 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 4),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 5),
                            { 0, 0, height }, { 32, 1, 32 }, { 0, 31, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 8),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 12),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }

                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 2),
                            { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height + 28 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 6),
                            { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height + 28 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 9),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 13),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4 | SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 3),
                            { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height + 28 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 7),
                            { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height + 28 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 10),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 11),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 33 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 14),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 15),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 33 });
                        break;
                }
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4 | SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackRightZeroGRollUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 16),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 14, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 20),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 8, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 24),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 25),
                            { 0, 0, height }, { 32, 1, 32 }, { 0, 31, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 6, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 28),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 29),
                            { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height + 28 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 17),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 21),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 26),
                            { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height + 28 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 30),
                            { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height + 28 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 18),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 19),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 33 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 22),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 23),
                            { 0, 0, height }, { 32, 20, 0 }, { 0, 6, height + 33 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 27),
                            { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height + 28 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 31),
                            { 0, 0, height }, { 32, 20, 1 }, { 0, 6, height + 28 });
                        break;
                }
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_C0 | SEGMENT_D4 | SEGMENT_BC | SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction),
                    0xFFFF, 0);
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES, 4, 0, height + 38, session.TrackColours[SCHEME_SUPPORTS]);
                PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackLeftZeroGRollDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackLeftZeroGRollUp(session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void LimLaunchedRCTrackRightZeroGRollDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackRightZeroGRollUp(session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void LimLaunchedRCTrackLeftLargeZeroGRollUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 0),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 5),
                            { 0, 0, height }, { 32, 0, 96 }, { 0, 30, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 9),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 10),
                            { 0, 0, height }, { 32, 0, 96 }, { 0, 30, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 15),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 25, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 1),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 2),
                            { 0, 0, height }, { 32, 0, 96 }, { 0, 30, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 6),
                            { 0, 0, height }, { 32, 0, 64 }, { 0, 30, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 11),
                            { 0, 0, height }, { 32, 0, 64 }, { 0, 2, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 16),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 3),
                            { 0, 0, height }, { 32, 0, 64 }, { 0, 30, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 7),
                            { 0, 0, height }, { 32, 0, 48 }, { 0, 30, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 12),
                            { 0, 0, height }, { 32, 0, 48 }, { 0, 2, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 17),
                            { 0, 0, height }, { 32, 0, 32 }, { 0, 2, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 18),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 40 });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4 | SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B8 | SEGMENT_C8 | SEGMENT_B4 | SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 4),
                            { 0, 0, height }, { 26, 0, 20 }, { 0, 26, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 7, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 8),
                            { 0, 0, height }, { 26, 0, 20 }, { 0, 26, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 5, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 13),
                            { 0, 0, height }, { 32, 0, 32 }, { 0, 2, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 14),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 40 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 6, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 19),
                            { 0, 0, height }, { 32, 10, 20 }, { 0, 18, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 8, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackRightLargeZeroGRollUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 20),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 25),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 26),
                            { 0, 0, height }, { 32, 0, 96 }, { 0, 30, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 31),
                            { 0, 0, height }, { 32, 0, 96 }, { 0, 30, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 35),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 25, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 21),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 27),
                            { 0, 0, height }, { 32, 0, 64 }, { 0, 2, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 32),
                            { 0, 0, height }, { 32, 0, 64 }, { 0, 30, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 36),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 37),
                            { 0, 0, height }, { 32, 0, 96 }, { 0, 30, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 22),
                            { 0, 0, height }, { 32, 0, 32 }, { 0, 2, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 23),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 40 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 28),
                            { 0, 0, height }, { 32, 0, 48 }, { 0, 2, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 33),
                            { 0, 0, height }, { 32, 0, 48 }, { 0, 30, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 38),
                            { 0, 0, height }, { 32, 0, 64 }, { 0, 30, height });
                        break;
                }

                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 24),
                            { 0, 0, height }, { 32, 10, 20 }, { 0, 18, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 7, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 29),
                            { 0, 0, height }, { 32, 0, 32 }, { 0, 2, height });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 30),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height + 40 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 5, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 34),
                            { 0, 0, height }, { 26, 0, 20 }, { 0, 26, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 6, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 39),
                            { 0, 0, height }, { 26, 0, 20 }, { 0, 26, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 8, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                switch (direction)
                {
                    case 1:
                        PaintUtilPushTunnelRight(session, height + 8, TUNNEL_0);
                        break;
                    case 2:
                        PaintUtilPushTunnelLeft(session, height + 8, TUNNEL_0);
                        break;
                }
                PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackLeftLargeZeroGRollDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackLeftLargeZeroGRollUp(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void LimLaunchedRCTrackRightLargeZeroGRollDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackRightLargeZeroGRollUp(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void LimLaunchedRCTrackLeftBankToLeftQuarterTurn325DegUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 0),
                            { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 2),
                            { 0, 6, height }, { 32, 20, 3 });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 3),
                            { 0, 6, height }, { 32, 1, 26 }, { 0, 27, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 5),
                            { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 7),
                            { 0, 6, height }, { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 1),
                            { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 4),
                            { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 6),
                            { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 8),
                            { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES, 4, 8, height - 6, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 2:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_2);
                        break;
                    case 3:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_2);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackRightBankToRightQuarterTurn325DegUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 9),
                            { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 11),
                            { 0, 6, height }, { 32, 20, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 13),
                            { 0, 6, height }, { 32, 20, 3 });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 14),
                            { 0, 6, height }, { 32, 1, 26 }, { 0, 27, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 16),
                            { 0, 6, height }, { 32, 20, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 10),
                            { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 12),
                            { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 15),
                            { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 17),
                            { 6, 0, height }, { 20, 32, 3 }, { 0, 6, height - 6 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES, 4, 8, height - 6, session.TrackColours[SCHEME_SUPPORTS]);
                switch (direction)
                {
                    case 0:
                        PaintUtilPushTunnelRight(session, height, TUNNEL_2);
                        break;
                    case 1:
                        PaintUtilPushTunnelLeft(session, height, TUNNEL_2);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackLeftQuarterTurn325DegDownToLeftBank(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 12),
                            { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 15),
                            { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 17),
                            { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 10),
                            { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES, 4, 8, height - 6, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 11),
                            { 6, 0, height }, { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 13),
                            { 6, 0, height }, { 20, 32, 3 });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 14),
                            { 6, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 16),
                            { 6, 0, height }, { 20, 32, 3 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 9),
                            { 6, 0, height }, { 20, 32, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackRightQuarterTurn325DegDownToRightBank(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 8),
                            { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 1),
                            { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 4),
                            { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 6),
                            { 0, 6, height }, { 32, 20, 3 }, { 0, 6, height - 6 });
                        break;
                }
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES, 4, 8, height - 6, session.TrackColours[SCHEME_SUPPORTS]);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_2);
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 2:
                PaintUtilSetGeneralSupportHeight(session, height + 48, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 7),
                            { 6, 0, height }, { 20, 32, 3 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 0),
                            { 6, 0, height }, { 20, 32, 3 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 2),
                            { 6, 0, height }, { 20, 32, 3 });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 3),
                            { 6, 0, height }, { 1, 32, 26 }, { 27, 0, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 5),
                            { 6, 0, height }, { 20, 32, 3 });
                        break;
                }
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
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
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackLeftLargeHalfLoopUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 0),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 7),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 14),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 21),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 1),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 8),
                            { 0, 0, height }, { 32, 20, 9 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 15),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 22),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 2),
                            { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 9),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 0, height + 70 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 16),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 16, height + 70 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 23),
                            { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 3),
                            { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 5, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 10),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 0, height + 200 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 6, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 17),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 16, height + 200 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 8, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 24),
                            { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 7, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 224, 0x20);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 4),
                            { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 11),
                            { 0, 0, height }, { 16, 16, 0 }, { 0, 16, height + 110 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 18),
                            { 0, 0, height }, { 16, 16, 0 }, { 0, 0, height + 100 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 25),
                            { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 128, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 5),
                            { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 12),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 16, height + 200 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 19),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 0, height + 200 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 26),
                            { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 224, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 6),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 16, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 13),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 16, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 20),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 0, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 27),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 0, height + 32 });
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackRightLargeHalfLoopUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 28),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 35),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 42),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 49),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 29),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 36),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 43),
                            { 0, 0, height }, { 32, 20, 9 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 9, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 50),
                            { 0, 0, height }, { 32, 20, 3 }, { 0, 6, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 4, 15, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 30),
                            { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 37),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 16, height + 70 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 44),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 0, height + 70 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 51),
                            { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 31),
                            { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 8, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 38),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 16, height + 200 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 7, 0, height + 28, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 45),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 0, height + 200 });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 5, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 52),
                            { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                        MetalASupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 6, 28, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 224, 0x20);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 32),
                            { 0, 0, height }, { 16, 16, 3 }, { 16, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 39),
                            { 0, 0, height }, { 16, 16, 0 }, { 0, 0, height + 100 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 46),
                            { 0, 0, height }, { 16, 16, 0 }, { 0, 16, height + 110 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 53),
                            { 0, 0, height }, { 16, 16, 3 }, { 16, 16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 128, 0x20);
                break;
            case 5:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 33),
                            { 0, 0, height }, { 32, 16, 3 }, { 0, 0, height });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 40),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 0, height + 200 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 47),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 16, height + 200 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 54),
                            { 0, 0, height }, { 32, 16, 3 }, { 0, 16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 224, 0x20);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 34),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 0, height + 32 });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 41),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 0, height + 32 });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 48),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 16, height + 32 });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 55),
                            { 0, 0, height }, { 32, 16, 0 }, { 0, 16, height + 32 });
                        break;
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        SEGMENT_B4 | SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0, direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackRightLargeHalfLoopDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackRightLargeHalfLoopUp(session, ride, 6 - trackSequence, direction, height, trackElement);
    }

    static void LimLaunchedRCTrackLeftLargeHalfLoopDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackLeftLargeHalfLoopUp(session, ride, 6 - trackSequence, direction, height, trackElement);
    }

    static void LimLaunchedRCTrackFlatTo60DegUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 0),
                    { 0, 0, height }, { 32, 27, 4 }, { 0, 2, height });
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 2),
                    { 0, 0, height }, { 1, 24, 43 }, { 29, 4, height + 2 });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 1),
                    { 0, 0, height }, { 32, 2, 43 }, { 0, 4, height });
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES, 4, 0, height + 4, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 4),
                    { 0, 0, height }, { 1, 24, 43 }, { 29, 4, height + 2 });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 3),
                    { 0, 0, height }, { 32, 2, 43 }, { 0, 4, height });
                MetalASupportsPaintSetup(
                    session, METAL_SUPPORTS_TUBES, 4, 0, height + 4, session.TrackColours[SCHEME_SUPPORTS]);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 5),
                    { 0, 0, height }, { 32, 27, 4 }, { 0, 2, height });
                MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 3, height, session.TrackColours[SCHEME_SUPPORTS]);
                break;
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, TUNNEL_0);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_2);
        }
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
    }

    static void LimLaunchedRCTrack60DegUpToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 6),
                    { 0, 0, height }, { 32, 27, 4 }, { 0, 2, height });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 7),
                    { 0, 0, height }, { 32, 2, 43 }, { 0, 4, height });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 8),
                    { 0, 0, height }, { 1, 24, 43 }, { 29, 4, height + 2 });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 10),
                    { 0, 0, height }, { 1, 24, 43 }, { 29, 4, height + 2 });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 9),
                    { 0, 0, height }, { 32, 2, 43 }, { 0, 4, height });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 11),
                    { 0, 0, height }, { 32, 27, 4 }, { 0, 2, height });
                break;
        }
        MetalASupportsPaintSetup(session, METAL_SUPPORTS_TUBES, 4, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, TUNNEL_1);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 24, TUNNEL_0);
        }
        PaintUtilSetSegmentSupportHeight(
            session, PaintUtilRotateSegments(SEGMENT_C4 | SEGMENT_CC | SEGMENT_D0, direction), 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
    }

    static void LimLaunchedRCTrackFlatTo60DegDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrack60DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void LimLaunchedRCTrack60DegDownToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        LimLaunchedRCTrackFlatTo60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
    }

    static void LimLaunchedRCTrackDiagFlatTo60DegUp(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 15),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 12),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 14),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 13),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 7, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackDiag60DegUpToFlat(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 19),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 16),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 18),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 17),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 20, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackDiagFlatTo60DegDown(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 17),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 18),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 16),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 16, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 19),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
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
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackDiag60DegDownToFlat(
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
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 13),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_BC | SEGMENT_C4 | SEGMENT_CC | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 1:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 14),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B4 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 2:
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 12),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_C0 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_D4, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
            case 3:
                switch (direction)
                {
                    case 0:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 1, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours[SCHEME_TRACK].WithIndex(SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 15),
                            { -16, -16, height }, { 32, 32, 4 }, { -16, -16, height });
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 0, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 2:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 2, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                    case 3:
                        MetalBSupportsPaintSetup(
                            session, METAL_SUPPORTS_TUBES, 3, 5, height, session.TrackColours[SCHEME_SUPPORTS]);
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_D0, direction), 0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64, 0x20);
                break;
        }
    }

    static void LimLaunchedRCTrackBooster(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement)
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15012), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15014), { 0, 0, height }, { 32, 1, 26 },
                    { 0, 27, height });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15013), { 0, 0, height }, { 32, 20, 3 },
                    { 0, 6, height });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours[SCHEME_TRACK].WithIndex(15015), { 0, 0, height }, { 32, 1, 26 },
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

    TRACK_PAINT_FUNCTION GetTrackPaintFunctionLimLaunchedRC(int32_t trackType)
    {
        switch (trackType)
        {
            // Use Looping Roller Coaster
            case TrackElemType::Flat:
            case TrackElemType::Up25:
            case TrackElemType::Up60:
            case TrackElemType::FlatToUp25:
            case TrackElemType::Up25ToUp60:
            case TrackElemType::Up60ToUp25:
            case TrackElemType::Up25ToFlat:
            case TrackElemType::Down25:
            case TrackElemType::Down60:
            case TrackElemType::FlatToDown25:
            case TrackElemType::Down25ToDown60:
            case TrackElemType::Down60ToDown25:
            case TrackElemType::Down25ToFlat:
            case TrackElemType::LeftQuarterTurn5Tiles:
            case TrackElemType::RightQuarterTurn5Tiles:
            case TrackElemType::FlatToLeftBank:
            case TrackElemType::FlatToRightBank:
            case TrackElemType::LeftBankToFlat:
            case TrackElemType::RightBankToFlat:
            case TrackElemType::BankedLeftQuarterTurn5Tiles:
            case TrackElemType::BankedRightQuarterTurn5Tiles:
            case TrackElemType::LeftBankToUp25:
            case TrackElemType::RightBankToUp25:
            case TrackElemType::Up25ToLeftBank:
            case TrackElemType::Up25ToRightBank:
            case TrackElemType::LeftBankToDown25:
            case TrackElemType::RightBankToDown25:
            case TrackElemType::Down25ToLeftBank:
            case TrackElemType::Down25ToRightBank:
            case TrackElemType::LeftBank:
            case TrackElemType::RightBank:
            case TrackElemType::LeftQuarterTurn5TilesUp25:
            case TrackElemType::RightQuarterTurn5TilesUp25:
            case TrackElemType::LeftQuarterTurn5TilesDown25:
            case TrackElemType::RightQuarterTurn5TilesDown25:
            case TrackElemType::SBendLeft:
            case TrackElemType::SBendRight:
            case TrackElemType::LeftQuarterTurn3Tiles:
            case TrackElemType::RightQuarterTurn3Tiles:
            case TrackElemType::LeftBankedQuarterTurn3Tiles:
            case TrackElemType::RightBankedQuarterTurn3Tiles:
            case TrackElemType::LeftQuarterTurn3TilesUp25:
            case TrackElemType::RightQuarterTurn3TilesUp25:
            case TrackElemType::LeftQuarterTurn3TilesDown25:
            case TrackElemType::RightQuarterTurn3TilesDown25:
            case TrackElemType::LeftHalfBankedHelixUpSmall:
            case TrackElemType::RightHalfBankedHelixUpSmall:
            case TrackElemType::LeftHalfBankedHelixDownSmall:
            case TrackElemType::RightHalfBankedHelixDownSmall:
            case TrackElemType::LeftHalfBankedHelixUpLarge:
            case TrackElemType::RightHalfBankedHelixUpLarge:
            case TrackElemType::LeftHalfBankedHelixDownLarge:
            case TrackElemType::RightHalfBankedHelixDownLarge:
            case TrackElemType::LeftQuarterTurn1TileUp60:
            case TrackElemType::RightQuarterTurn1TileUp60:
            case TrackElemType::LeftQuarterTurn1TileDown60:
            case TrackElemType::RightQuarterTurn1TileDown60:
            case TrackElemType::Up25LeftBanked:
            case TrackElemType::Up25RightBanked:
            case TrackElemType::OnRidePhoto:
            case TrackElemType::Down25LeftBanked:
            case TrackElemType::Down25RightBanked:
            case TrackElemType::LeftEighthToDiag:
            case TrackElemType::RightEighthToDiag:
            case TrackElemType::LeftEighthToOrthogonal:
            case TrackElemType::RightEighthToOrthogonal:
            case TrackElemType::LeftEighthBankToDiag:
            case TrackElemType::RightEighthBankToDiag:
            case TrackElemType::LeftEighthBankToOrthogonal:
            case TrackElemType::RightEighthBankToOrthogonal:
            case TrackElemType::DiagFlat:
            case TrackElemType::DiagUp25:
            case TrackElemType::DiagUp60:
            case TrackElemType::DiagFlatToUp25:
            case TrackElemType::DiagUp25ToUp60:
            case TrackElemType::DiagUp60ToUp25:
            case TrackElemType::DiagUp25ToFlat:
            case TrackElemType::DiagDown25:
            case TrackElemType::DiagDown60:
            case TrackElemType::DiagFlatToDown25:
            case TrackElemType::DiagDown25ToDown60:
            case TrackElemType::DiagDown60ToDown25:
            case TrackElemType::DiagDown25ToFlat:
            case TrackElemType::DiagFlatToLeftBank:
            case TrackElemType::DiagFlatToRightBank:
            case TrackElemType::DiagLeftBankToFlat:
            case TrackElemType::DiagRightBankToFlat:
            case TrackElemType::DiagLeftBankToUp25:
            case TrackElemType::DiagRightBankToUp25:
            case TrackElemType::DiagUp25ToLeftBank:
            case TrackElemType::DiagUp25ToRightBank:
            case TrackElemType::DiagLeftBankToDown25:
            case TrackElemType::DiagRightBankToDown25:
            case TrackElemType::DiagDown25ToLeftBank:
            case TrackElemType::DiagDown25ToRightBank:
            case TrackElemType::DiagLeftBank:
            case TrackElemType::DiagRightBank:
            case TrackElemType::LeftBankedQuarterTurn3TileUp25:
            case TrackElemType::RightBankedQuarterTurn3TileUp25:
            case TrackElemType::LeftBankedQuarterTurn3TileDown25:
            case TrackElemType::RightBankedQuarterTurn3TileDown25:
            case TrackElemType::LeftBankedQuarterTurn5TileUp25:
            case TrackElemType::RightBankedQuarterTurn5TileUp25:
            case TrackElemType::LeftBankedQuarterTurn5TileDown25:
            case TrackElemType::RightBankedQuarterTurn5TileDown25:
            case TrackElemType::Up25ToLeftBankedUp25:
            case TrackElemType::Up25ToRightBankedUp25:
            case TrackElemType::LeftBankedUp25ToUp25:
            case TrackElemType::RightBankedUp25ToUp25:
            case TrackElemType::Down25ToLeftBankedDown25:
            case TrackElemType::Down25ToRightBankedDown25:
            case TrackElemType::LeftBankedDown25ToDown25:
            case TrackElemType::RightBankedDown25ToDown25:
            case TrackElemType::LeftBankedFlatToLeftBankedUp25:
            case TrackElemType::RightBankedFlatToRightBankedUp25:
            case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
            case TrackElemType::RightBankedUp25ToRightBankedFlat:
            case TrackElemType::LeftBankedFlatToLeftBankedDown25:
            case TrackElemType::RightBankedFlatToRightBankedDown25:
            case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
            case TrackElemType::RightBankedDown25ToRightBankedFlat:
            case TrackElemType::FlatToLeftBankedUp25:
            case TrackElemType::FlatToRightBankedUp25:
            case TrackElemType::LeftBankedUp25ToFlat:
            case TrackElemType::RightBankedUp25ToFlat:
            case TrackElemType::FlatToLeftBankedDown25:
            case TrackElemType::FlatToRightBankedDown25:
            case TrackElemType::LeftBankedDown25ToFlat:
            case TrackElemType::RightBankedDown25ToFlat:
                return GetTrackPaintFunctionLoopingRC(trackType);
            case TrackElemType::EndStation:
            case TrackElemType::BeginStation:
            case TrackElemType::MiddleStation:
                return LimLaunchedRCTrackStation;
            case TrackElemType::LeftVerticalLoop:
                return LimLaunchedRCTrackLeftVerticalLoop;
            case TrackElemType::RightVerticalLoop:
                return LimLaunchedRCTrackRightVerticalLoop;
            case TrackElemType::LeftTwistDownToUp:
                return LimLaunchedRCTrackLeftTwistDownToUp;
            case TrackElemType::RightTwistDownToUp:
                return LimLaunchedRCTrackRightTwistDownToUp;
            case TrackElemType::LeftTwistUpToDown:
                return LimLaunchedRCTrackLeftTwistUpToDown;
            case TrackElemType::RightTwistUpToDown:
                return LimLaunchedRCTrackRightTwistUpToDown;
            case TrackElemType::LeftCorkscrewUp:
                return LimLaunchedRCTrackLeftCorkscrewUp;
            case TrackElemType::RightCorkscrewUp:
                return LimLaunchedRCTrackRightCorkscrewUp;
            case TrackElemType::LeftCorkscrewDown:
                return LimLaunchedRCTrackLeftCorkscrewDown;
            case TrackElemType::RightCorkscrewDown:
                return LimLaunchedRCTrackRightCorkscrewDown;
            case TrackElemType::Brakes:
                return LimLaunchedRCTrackBrakes;
            case TrackElemType::Up90:
                return LimLaunchedRCTrack90DegUp;
            case TrackElemType::Down90:
                return LimLaunchedRCTrack90DegDown;
            case TrackElemType::Up60ToUp90:
                return LimLaunchedRCTrack60DegUpTo90DegUp;
            case TrackElemType::Down90ToDown60:
                return LimLaunchedRCTrack90DegDownTo60DegDown;
            case TrackElemType::Up90ToUp60:
                return LimLaunchedRCTrack90DegUpTo60DegUp;
            case TrackElemType::Down60ToDown90:
                return LimLaunchedRCTrack60DegDownTo90DegDown;
            case TrackElemType::Up90ToInvertedFlatQuarterLoop:
                return LimLaunchedRCTrack90DegToInvertedFlatQuarterLoopUp;
            case TrackElemType::InvertedFlatToDown90QuarterLoop:
                return LimLaunchedRCTrackInvertedFlatTo90DegQuarterLoopDown;
            case TrackElemType::BlockBrakes:
                return LimLaunchedRCTrackBlockBrakes;
            case TrackElemType::LeftQuarterTurn1TileUp90:
                return LimLaunchedRCTrackLeftQuarterTurn190DegUp;
            case TrackElemType::RightQuarterTurn1TileUp90:
                return LimLaunchedRCTrackRightQuarterTurn190DegUp;
            case TrackElemType::LeftQuarterTurn1TileDown90:
                return LimLaunchedRCTrackLeftQuarterTurn190DegDown;
            case TrackElemType::RightQuarterTurn1TileDown90:
                return LimLaunchedRCTrackRightQuarterTurn190DegDown;
            case TrackElemType::FlatToUp60LongBase:
                return LimLaunchedRCTrackFlatTo60DegUpLongBase;
            case TrackElemType::Up60ToFlatLongBase:
                return LimLaunchedRCTrack60DegUpToFlatLongBase;
            case TrackElemType::FlatToDown60LongBase:
                return LimLaunchedRCTrackFlatTo60DegDownLongBase;
            case TrackElemType::Down60ToFlatLongBase:
                return LimLaunchedRCTrack60DegDownToFlatLongBase;
            case TrackElemType::FlatToUp60:
                return LimLaunchedRCTrackFlatTo60DegUp;
            case TrackElemType::Up60ToFlat:
                return LimLaunchedRCTrack60DegUpToFlat;
            case TrackElemType::FlatToDown60:
                return LimLaunchedRCTrackFlatTo60DegDown;
            case TrackElemType::Down60ToFlat:
                return LimLaunchedRCTrack60DegDownToFlat;
            case TrackElemType::DiagFlatToUp60:
                return LimLaunchedRCTrackDiagFlatTo60DegUp;
            case TrackElemType::DiagUp60ToFlat:
                return LimLaunchedRCTrackDiag60DegUpToFlat;
            case TrackElemType::DiagFlatToDown60:
                return LimLaunchedRCTrackDiagFlatTo60DegDown;
            case TrackElemType::DiagDown60ToFlat:
                return LimLaunchedRCTrackDiag60DegDownToFlat;
            case TrackElemType::LeftBarrelRollUpToDown:
                return LimLaunchedRCTrackLeftBarrelRollUpToDown;
            case TrackElemType::RightBarrelRollUpToDown:
                return LimLaunchedRCTrackRightBarrelRollUpToDown;
            case TrackElemType::LeftBarrelRollDownToUp:
                return LimLaunchedRCTrackLeftBarrelRollDownToUp;
            case TrackElemType::RightBarrelRollDownToUp:
                return LimLaunchedRCTrackRightBarrelRollDownToUp;
            case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
                return LimLaunchedRCTrackLeftBankToLeftQuarterTurn325DegUp;
            case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
                return LimLaunchedRCTrackRightBankToRightQuarterTurn325DegUp;
            case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
                return LimLaunchedRCTrackLeftQuarterTurn325DegDownToLeftBank;
            case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
                return LimLaunchedRCTrackRightQuarterTurn325DegDownToRightBank;
            case TrackElemType::HalfLoopUp:
                return LimLaunchedRCTrackHalfLoopUp;
            case TrackElemType::HalfLoopDown:
                return LimLaunchedRCTrackHalfLoopDown;
            case TrackElemType::LeftMediumHalfLoopUp:
                return LimLaunchedRCTrackLeftMediumHalfLoopUp;
            case TrackElemType::RightMediumHalfLoopUp:
                return LimLaunchedRCTrackRightMediumHalfLoopUp;
            case TrackElemType::LeftMediumHalfLoopDown:
                return LimLaunchedRCTrackLeftMediumHalfLoopDown;
            case TrackElemType::RightMediumHalfLoopDown:
                return LimLaunchedRCTrackRightMediumHalfLoopDown;
            case TrackElemType::LeftLargeHalfLoopUp:
                return LimLaunchedRCTrackLeftLargeHalfLoopUp;
            case TrackElemType::RightLargeHalfLoopUp:
                return LimLaunchedRCTrackRightLargeHalfLoopUp;
            case TrackElemType::RightLargeHalfLoopDown:
                return LimLaunchedRCTrackRightLargeHalfLoopDown;
            case TrackElemType::LeftLargeHalfLoopDown:
                return LimLaunchedRCTrackLeftLargeHalfLoopDown;
            case TrackElemType::LeftLargeCorkscrewUp:
                return LimLaunchedRCTrackLeftLargeCorkscrewUp;
            case TrackElemType::RightLargeCorkscrewUp:
                return LimLaunchedRCTrackRightLargeCorkscrewUp;
            case TrackElemType::LeftLargeCorkscrewDown:
                return LimLaunchedRCTrackLeftLargeCorkscrewDown;
            case TrackElemType::RightLargeCorkscrewDown:
                return LimLaunchedRCTrackRightLargeCorkscrewDown;
            case TrackElemType::LeftZeroGRollUp:
                return LimLaunchedRCTrackLeftZeroGRollUp;
            case TrackElemType::RightZeroGRollUp:
                return LimLaunchedRCTrackRightZeroGRollUp;
            case TrackElemType::LeftZeroGRollDown:
                return LimLaunchedRCTrackLeftZeroGRollDown;
            case TrackElemType::RightZeroGRollDown:
                return LimLaunchedRCTrackRightZeroGRollDown;
            case TrackElemType::LeftLargeZeroGRollUp:
                return LimLaunchedRCTrackLeftLargeZeroGRollUp;
            case TrackElemType::RightLargeZeroGRollUp:
                return LimLaunchedRCTrackRightLargeZeroGRollUp;
            case TrackElemType::LeftLargeZeroGRollDown:
                return LimLaunchedRCTrackLeftLargeZeroGRollDown;
            case TrackElemType::RightLargeZeroGRollDown:
                return LimLaunchedRCTrackRightLargeZeroGRollDown;
            case TrackElemType::Booster:
                return LimLaunchedRCTrackBooster;
        }
        return nullptr;
    }
} // namespace OpenRCT2
