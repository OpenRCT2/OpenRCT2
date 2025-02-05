/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../drawing/Drawing.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/TrackStyleSupports.h"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

static constexpr ImageIndex kMiniSuspendedRCFlatImages[2][kNumOrthogonalDirections] = {
    { 28491, 28492, 28493, 28494 },
    { 28507, 28508, 28509, 28510 },
};

/** rct2: 0x008AFE9C */
static void MiniSuspendedRCTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28433), { 0, 6, height + 24 }, { 32, 20, 1 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28434), { 0, 6, height + 24 }, { 32, 20, 1 });
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
                    session, direction, session.TrackColours.WithIndex(28383), { 0, 6, height + 24 }, { 32, 20, 1 });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28384), { 0, 6, height + 24 }, { 32, 20, 1 });
                break;
        }
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AFF4C, 0x008AFF5C, 0x008AFF6C */
static void MiniSuspendedRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex imageIds[4][2] = {
        { 28433, SPR_STATION_INVERTED_BAR_E_SW_NE },
        { 28434, SPR_STATION_INVERTED_BAR_E_NW_SE },
        { 28433, SPR_STATION_INVERTED_BAR_E_SW_NE },
        { 28434, SPR_STATION_INVERTED_BAR_E_NW_SE },
    };
    const bool stationDrawn = TrackPaintUtilDrawStationInverted(
        session, ride, direction, height, trackElement, STATION_VARIANT_1);
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(imageIds[direction][0]), { 0, 6, height + 24 },
        { { 0, 6, height + 24 }, { 32, 20, 3 } });
    if (stationDrawn)
    {
        PaintAddImageAsChildRotated(
            session, direction, session.SupportColours.WithIndex(imageIds[direction][1]), { 0, 6, height + 24 },
            { { 0, 6, height + 24 }, { 32, 20, 3 } });
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal);
    }
    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x008AFEAC */
static void MiniSuspendedRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28435), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28441), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28437), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28442), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28385), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28391), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28387), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28392), { 0, 6, height + 24 },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                break;
        }
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x008AFEBC */
static void MiniSuspendedRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28436), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28443), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28438), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28444), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28386), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28393), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28388), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28394), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
        }
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 64);
}

/** rct2: 0x008AFECC */
static void MiniSuspendedRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28440), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28446), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28439), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28445), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28390), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28396), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28389), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(28395), { 0, 6, height + 24 },
                    { { 0, 6, height + 32 }, { 32, 20, 1 } });
                break;
        }
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x008AFEDC */
static void MiniSuspendedRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniSuspendedRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AFEEC */
static void MiniSuspendedRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniSuspendedRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AFEFC */
static void MiniSuspendedRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    MiniSuspendedRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AFF0C */
static void MiniSuspendedRCTrackLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28406), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28411), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28416), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28401), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28405), { 0, 0, height + 24 }, { 32, 16, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28410), { 0, 0, height + 24 }, { 32, 16, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28415), { 0, 16, height + 24 }, { 32, 16, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28400), { 0, 16, height + 24 }, { 32, 16, 1 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28404), { 0, 16, height + 24 }, { 16, 16, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28409), { 16, 16, height + 24 }, { 16, 16, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28414), { 16, 0, height + 24 }, { 16, 16, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28399), { 0, 0, height + 24 }, { 16, 16, 1 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28403), { 16, 0, height + 24 }, { 16, 32, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28408), { 0, 0, height + 24 }, { 16, 32, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28413), { 0, 0, height + 24 }, { 16, 32, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28398), { 16, 0, height + 24 }, { 16, 32, 1 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28402), { 6, 0, height + 24 }, { 20, 32, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28407), { 6, 0, height + 24 }, { 20, 32, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28412), { 6, 0, height + 24 }, { 20, 32, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28397), { 6, 0, height + 24 }, { 20, 32, 1 });
                    break;
            }

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AFF1C */
static void MiniSuspendedRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    MiniSuspendedRCTrackLeftQuarterTurn5(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AFF2C */
static void MiniSuspendedRCTrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28421), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28428), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28424), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28425), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28422), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28427), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28423), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28426), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28423), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28426), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28422), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28427), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28424), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28425), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28421), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28428), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
            }

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AFF3C */
static void MiniSuspendedRCTrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28417), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28432), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28420), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28429), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28418), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28431), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28419), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28430), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28419), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28430), { 0, 0, height + 24 }, { 32, 26, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28418), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28431), { 0, 6, height + 24 }, { 32, 26, 1 });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28420), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28429), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28417), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28432), { 0, 6, height + 24 }, { 32, 20, 1 });
                    break;
            }

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AFF7C */
static void MiniSuspendedRCTrackLeftQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28452), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28455), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28458), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28449), { 0, 6, height + 24 }, { 32, 20, 3 });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28451), { 16, 0, height + 24 }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28454), { 0, 0, height + 24 }, { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28457), { 0, 16, height + 24 }, { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28448), { 16, 16, height + 24 }, { 16, 16, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28450), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28453), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28456), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28447), { 6, 0, height + 24 }, { 20, 32, 3 });
                    break;
            }

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

/** rct2: 0x008AFF8C */
static void MiniSuspendedRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    MiniSuspendedRCTrackLeftQuarterTurn3(session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AFFAC */
static void MiniSuspendedRCTrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28475), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28479), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28483), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28487), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28476), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 32, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28480), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 34, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28484), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 32, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28488), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 32, 16, 1 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28477), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28481), { 0, 0, height + 24 },
                        { { 16, 16, height + 24 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28485), { 0, 0, height + 24 },
                        { { 16, 0, height + 24 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28489), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28478), { 0, 0, height + 24 },
                        { { 16, 16, height + 24 }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28482), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 16, 18, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28486), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28490), { 0, 0, height + 24 },
                        { { 16, 0, height + 24 }, { 16, 16, 1 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AFFBC */
static void MiniSuspendedRCTrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28459), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28463), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28467), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28471), { 0, 0, height + 24 },
                        { { 0, 6, height + 24 }, { 32, 20, 1 } });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28460), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 32, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28464), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 32, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28468), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 34, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28472), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 32, 16, 1 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28461), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28465), { 0, 0, height + 24 },
                        { { 16, 0, height + 24 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28469), { 0, 0, height + 24 },
                        { { 16, 16, height + 24 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28473), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28462), { 0, 0, height + 24 },
                        { { 16, 0, height + 24 }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28466), { 0, 0, height + 24 },
                        { { 0, 0, height + 24 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28470), { 0, 0, height + 24 },
                        { { 0, 16, height + 24 }, { 16, 18, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(28474), { 0, 0, height + 24 },
                        { { 16, 16, height + 24 }, { 16, 16, 1 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AFFCC */
static void MiniSuspendedRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MiniSuspendedRCTrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AFFDC */
static void MiniSuspendedRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    MiniSuspendedRCTrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AFF9C */
static void MiniSuspendedRCTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const auto* images = kMiniSuspendedRCFlatImages[trackElement.HasChain()];
    TrackPaintUtilDiagTilesPaint(
        session, 1, height + 24, direction, trackSequence, images, defaultDiagTileOffsets, defaultDiagBoundLengths, nullptr);

    if (trackSequence == 3)

        PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008B000C */
static void MiniSuspendedRCTrackDiag25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
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
                            session, direction, session.TrackColours.WithIndex(28522), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28506), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28519), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28503), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28521), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28505), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28520), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28504), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008AFFEC */
static void MiniSuspendedRCTrackDiagFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
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
                            session, direction, session.TrackColours.WithIndex(28514), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28498), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28511), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28495), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28513), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28497), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28512), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28496), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

/** rct2: 0x008AFFFC */
static void MiniSuspendedRCTrackDiag25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
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
                            session, direction, session.TrackColours.WithIndex(28518), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28502), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28515), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28499), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28517), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28501), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28516), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28500), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008B003C */
static void MiniSuspendedRCTrackDiag25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
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
                            session, direction, session.TrackColours.WithIndex(28520), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28504), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28521), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28505), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28519), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28503), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28522), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28506), { -16, -16, height + 24 },
                            { { -16, -16, height + 40 }, { 32, 32, 1 } });
                        break;
                }
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008B001C */
static void MiniSuspendedRCTrackDiagFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
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
                            session, direction, session.TrackColours.WithIndex(28516), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28500), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28517), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28501), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28515), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28499), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28518), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28502), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }

            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x008B002C */
static void MiniSuspendedRCTrackDiag25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
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
                            session, direction, session.TrackColours.WithIndex(28512), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28496), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28513), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28497), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28511), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28495), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            if (trackElement.HasChain())
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28514), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(28498), { -16, -16, height + 24 },
                            { { -16, -16, height + 32 }, { 32, 32, 1 } });
                        break;
                }
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

TrackPaintFunction GetTrackPaintFunctionMiniSuspendedRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return MiniSuspendedRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return MiniSuspendedRCTrackStation;
        case TrackElemType::Up25:
            return MiniSuspendedRCTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return MiniSuspendedRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return MiniSuspendedRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return MiniSuspendedRCTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return MiniSuspendedRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return MiniSuspendedRCTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return MiniSuspendedRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return MiniSuspendedRCTrackRightQuarterTurn5;
        case TrackElemType::SBendLeft:
            return MiniSuspendedRCTrackSBendLeft;
        case TrackElemType::SBendRight:
            return MiniSuspendedRCTrackSBendRight;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return MiniSuspendedRCTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return MiniSuspendedRCTrackRightQuarterTurn3;
        case TrackElemType::LeftEighthToDiag:
            return MiniSuspendedRCTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return MiniSuspendedRCTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return MiniSuspendedRCTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return MiniSuspendedRCTrackRightEighthToOrthogonal;
        case TrackElemType::DiagFlat:
            return MiniSuspendedRCTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return MiniSuspendedRCTrackDiag25DegUp;
        case TrackElemType::DiagFlatToUp25:
            return MiniSuspendedRCTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return MiniSuspendedRCTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return MiniSuspendedRCTrackDiag25DegDown;
        case TrackElemType::DiagFlatToDown25:
            return MiniSuspendedRCTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return MiniSuspendedRCTrackDiag25DegDownToFlat;
        default:
            return TrackPaintFunctionDummy;
    }
}

// clang-format off
const TrackElementMetalSupports kMiniSuspendedCoasterMetalSupports[EnumValue(TrackElemType::Count)] = {
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Flat
    { { { kMetalSupportDrawAll, MetalSupportPlace::BottomRightSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { kMetalSupportDrawAll, MetalSupportPlace::TopLeftSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // EndStation
    { { { kMetalSupportDrawAll, MetalSupportPlace::BottomRightSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { kMetalSupportDrawAll, MetalSupportPlace::TopLeftSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BeginStation
    { { { kMetalSupportDrawAll, MetalSupportPlace::BottomRightSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { kMetalSupportDrawAll, MetalSupportPlace::TopLeftSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MiddleStation
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::TopRightSide, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::TopRightSide, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60ToUp25
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::TopRightSide, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToFlat
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::BottomLeftSide, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::BottomLeftSide, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60ToDown25
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::BottomLeftSide, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToFlat
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn5Tiles
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn5Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BankedLeftQuarterTurn5Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BankedRightQuarterTurn5Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn5TilesUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn5TilesUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn5TilesDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn5TilesDown25
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { EnumsToFlags(MetalSupportFlags::drawDirection0, MetalSupportFlags::drawDirection1), MetalSupportPlace::TopLeftSide, { 30, 30, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { EnumsToFlags(MetalSupportFlags::drawDirection2, MetalSupportFlags::drawDirection3), MetalSupportPlace::BottomRightSide, { 0, 0, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // SBendLeft
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { EnumsToFlags(MetalSupportFlags::drawDirection0, MetalSupportFlags::drawDirection1), MetalSupportPlace::BottomRightSide, { 30, 30, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { EnumsToFlags(MetalSupportFlags::drawDirection2, MetalSupportFlags::drawDirection3), MetalSupportPlace::TopLeftSide, { 0, 0, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // SBendRight
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftVerticalLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightVerticalLoop
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn3Tiles
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn3Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedQuarterTurn3Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedQuarterTurn3Tiles
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn3TilesUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn3TilesUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn3TilesDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn3TilesDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn1Tile
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn1Tile
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftTwistDownToUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightTwistDownToUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftTwistUpToDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightTwistUpToDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // HalfLoopUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // HalfLoopDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftCorkscrewUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightCorkscrewUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftCorkscrewDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightCorkscrewDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // TowerBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // TowerSection
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToUp25Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToUp60Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60ToUp25Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToFlatCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToDown25Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToDown60Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60ToDown25Covered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToFlatCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn5TilesCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn5TilesCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // SBendLeftCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // SBendRightCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn3TilesCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn3TilesCovered
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftHalfBankedHelixUpSmall
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightHalfBankedHelixUpSmall
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftHalfBankedHelixDownSmall
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightHalfBankedHelixDownSmall
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftHalfBankedHelixUpLarge
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightHalfBankedHelixUpLarge
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftHalfBankedHelixDownLarge
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightHalfBankedHelixDownLarge
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn1TileUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn1TileUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn1TileDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn1TileDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Brakes
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Booster
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Maze
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterBankedHelixLargeUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterBankedHelixLargeUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterBankedHelixLargeDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterBankedHelixLargeDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterHelixLargeUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterHelixLargeUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterHelixLargeDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterHelixLargeDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25LeftBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25RightBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Waterfall
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Rapids
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // OnRidePhoto
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25LeftBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25RightBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Watersplash
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToUp60LongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60ToFlatLongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Whirlpool
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60ToFlatLongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToDown60LongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // CableLiftHill
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // ReverseFreefallSlope
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // ReverseFreefallVertical
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60ToUp90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down90ToDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up90ToUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60ToDown90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BrakeForDrop
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::BottomCorner, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToDiag
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToDiag
    { { { kMetalSupportDrawAll, MetalSupportPlace::RightCorner, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToOrthogonal
    { { { kMetalSupportDrawAll, MetalSupportPlace::RightCorner, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToOrthogonal
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToDiag
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToDiag
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToOrthogonal
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToOrthogonal
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 30, 30, 30, 30 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 42, 42, 42, 42 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 34, 34, 34, 34 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp60ToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::noLongCrossbeam), MetalSupportPlace::LeftCorner, { 36, 36, 36, 36 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 42, 42, 42, 42 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 36, 36, 36, 36 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown60ToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 34, 34, 34, 34 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp60ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown60ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LogFlumeReverser
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // SpinningTunnel
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBarrelRollUpToDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBarrelRollUpToDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBarrelRollDownToUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBarrelRollDownToUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankToLeftQuarterTurn3TilesUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankToRightQuarterTurn3TilesUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn3TilesDown25ToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn3TilesDown25ToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // PoweredLift
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftLargeHalfLoopUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightLargeHalfLoopUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftLargeHalfLoopDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightLargeHalfLoopDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerTwistUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerTwistUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerTwistDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerTwistDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlyerHalfLoopUninvertedUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlyerHalfLoopInvertedDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerCorkscrewUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerCorkscrewUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerCorkscrewDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerCorkscrewDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // HeartLineTransferUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // HeartLineTransferDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftHeartLineRoll
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightHeartLineRoll
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MinigolfHoleA
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MinigolfHoleB
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MinigolfHoleC
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MinigolfHoleD
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MinigolfHoleE
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MultiDimInvertedFlatToDown90QuarterLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up90ToInvertedFlatQuarterLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // InvertedFlatToDown90QuarterLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftCurvedLiftHill
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightCurvedLiftHill
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftReverser
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightReverser
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // AirThrustTopCap
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // AirThrustVerticalDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // AirThrustVerticalDownToLevel
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BlockBrakes
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedQuarterTurn3TileUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedQuarterTurn3TileUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedQuarterTurn3TileDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedQuarterTurn3TileDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedQuarterTurn5TileUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedQuarterTurn5TileUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedQuarterTurn5TileDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedQuarterTurn5TileDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToLeftBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToRightBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedUp25ToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedUp25ToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToLeftBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToRightBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedDown25ToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedDown25ToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedFlatToLeftBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedFlatToRightBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedUp25ToLeftBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedUp25ToRightBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedFlatToLeftBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedFlatToRightBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedDown25ToLeftBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedDown25ToRightBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToLeftBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToRightBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedUp25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedUp25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToLeftBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToRightBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedDown25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedDown25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn1TileUp90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn1TileUp90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn1TileDown90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn1TileDown90
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MultiDimUp90ToInvertedFlatQuarterLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MultiDimFlatToDown90QuarterLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MultiDimInvertedUp90ToFlatQuarterLoop
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RotationControlToggle
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack1x4A
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack2x2
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack4x4
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack2x4
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack1x5
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack1x1A
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack1x4B
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack1x1B
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack1x4C
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatTrack3x3
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftLargeCorkscrewUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightLargeCorkscrewUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftLargeCorkscrewDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightLargeCorkscrewDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftMediumHalfLoopUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightMediumHalfLoopUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftMediumHalfLoopDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightMediumHalfLoopDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftZeroGRollUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightZeroGRollUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftZeroGRollDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightZeroGRollDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftLargeZeroGRollUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightLargeZeroGRollUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftLargeZeroGRollDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightLargeZeroGRollDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerLargeHalfLoopUninvertedUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerLargeHalfLoopUninvertedUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerLargeHalfLoopInvertedDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerLargeHalfLoopInvertedDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerLargeHalfLoopInvertedUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerLargeHalfLoopInvertedUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftFlyerLargeHalfLoopUninvertedDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightFlyerLargeHalfLoopUninvertedDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlyerHalfLoopInvertedUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlyerHalfLoopUninvertedDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToDiagUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToDiagUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToDiagDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToDiagDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToOrthogonalUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToOrthogonalUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToOrthogonalDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToOrthogonalDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToLeftBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToRightBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedUp25ToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedUp25ToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToLeftBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToRightBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedDown25ToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedDown25ToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedFlatToLeftBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedFlatToRightBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedUp25ToLeftBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedUp25ToRightBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedFlatToLeftBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedFlatToRightBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedDown25ToLeftBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedDown25ToRightBankedFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToLeftBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToRightBankedUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedUp25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedUp25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToLeftBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToRightBankedDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankedDown25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankedDown25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25LeftBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25RightBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25LeftBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25RightBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToDiagUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToDiagUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToDiagDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToDiagDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToOrthogonalUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToOrthogonalUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToOrthogonalDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToOrthogonalDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagBrakes
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagBlockBrakes
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25Brakes
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagBooster
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToUp60LongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp60ToFlatLongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToDown60LongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown60ToFlatLongBase
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthDiveLoopUpToOrthogonal
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthDiveLoopUpToOrthogonal
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthDiveLoopDownToDiag
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthDiveLoopDownToDiag
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 42, 42, 42, 42 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25Brakes
};
// clang-format on
