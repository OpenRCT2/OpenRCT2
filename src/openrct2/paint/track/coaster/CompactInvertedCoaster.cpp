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
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Inverted;

static constexpr ImageIndex kCompactInvertedDiagFlatImages[2][kNumOrthogonalDirections] = {
    { 26781, 26782, 26783, 26784 },
    { 26809, 26810, 26811, 26812 },
};

static constexpr ImageIndex kCompactInvertedDiagBrakeImages[kNumOrthogonalDirections] = {
    SPR_G2_SLC_DIAG_BRAKES,
    SPR_G2_SLC_DIAG_BRAKES + 1,
    SPR_G2_SLC_DIAG_BRAKES,
    SPR_G2_SLC_DIAG_BRAKES + 1,
};

static constexpr ImageIndex kCompactInvertedDiagBlockBrakeImages[2][kNumOrthogonalDirections] = {
    {
        SPR_G2_SLC_DIAG_BRAKES,
        SPR_G2_SLC_DIAG_BRAKES + 1,
        SPR_G2_SLC_DIAG_BRAKES,
        SPR_G2_SLC_DIAG_BRAKES + 1,
    },
    {
        SPR_G2_SLC_DIAG_BRAKES,
        SPR_G2_SLC_DIAG_BRAKES + 2,
        SPR_G2_SLC_DIAG_BRAKES,
        SPR_G2_SLC_DIAG_BRAKES + 2,
    },
};

/** rct2: 0x008AE6E0 */
static void CompactInvertedRCTrackFlat(
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
                    session, direction, session.TrackColours.WithIndex(26557), { 0, 0, height + 29 },
                    { { 0, 6, height + 29 }, { 32, 20, 3 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26558), { 0, 0, height + 29 },
                    { { 0, 6, height + 29 }, { 32, 20, 3 } });
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
                    session, direction, session.TrackColours.WithIndex(26555), { 0, 0, height + 29 },
                    { { 0, 6, height + 29 }, { 32, 20, 3 } });
                break;
            case 1:
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26556), { 0, 0, height + 29 },
                    { { 0, 6, height + 29 }, { 32, 20, 3 } });
                break;
        }
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AE950, 0x008AE960, 0x008AE970 */
static void CompactInvertedRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex imageIds[4][2] = {
        { 26557, SPR_STATION_INVERTED_BAR_A_SW_NE },
        { 26558, SPR_STATION_INVERTED_BAR_A_NW_SE },
        { 26557, SPR_STATION_INVERTED_BAR_A_SW_NE },
        { 26558, SPR_STATION_INVERTED_BAR_A_NW_SE },
    };
    const bool stationDrawn = TrackPaintUtilDrawStationInverted(
        session, ride, direction, height, trackElement, STATION_VARIANT_TALL);
    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(imageIds[direction][0]), { 0, 0, height + 29 },
        { { 0, 6, height + 29 }, { 32, 20, 3 } });
    if (stationDrawn)
    {
        PaintAddImageAsChildRotated(
            session, direction, session.SupportColours.WithIndex(imageIds[direction][1]), { 0, 6, height + 29 },
            { { 0, 6, height + 29 }, { 32, 20, 3 } });
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal);
    }
    TrackPaintUtilDrawStationTunnelTall(session, direction, height);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AE6F0 */
static void CompactInvertedRCTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26621), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26622), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26623), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26624), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26569), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26570), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26571), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26572), { 0, 0, height + 29 },
                    { { 0, 6, height + 45 }, { 32, 20, 3 } });
                break;
        }
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:

                break;
            case 1:

                break;
            case 2:

                break;
            case 3:

                break;
        }
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

/** rct2: 0x008AE700 */
static void CompactInvertedRCTrack60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26637), { 0, 0, height + 29 },
                    { { 0, 6, height + 93 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26638), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 81 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26639), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 81 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26640), { 0, 0, height + 29 },
                    { { 0, 6, height + 93 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26585), { 0, 0, height + 29 },
                    { { 0, 6, height + 93 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26586), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 81 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26587), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 81 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26588), { 0, 0, height + 29 },
                    { { 0, 6, height + 93 }, { 32, 20, 3 } });
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 120);
}

/** rct2: 0x008AE710 */
static void CompactInvertedRCTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26613), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26614), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26615), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26616), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26561), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26562), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26563), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26564), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
        }
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:

                break;
            case 1:

                break;
            case 2:

                break;
            case 3:

                break;
        }
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

/** rct2: 0x008AE720 */
static void CompactInvertedRCTrack25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26625), { 0, 0, height + 29 },
                    { { 0, 6, height + 61 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26629), { 0, 0, height + 29 },
                    { { 0, 10, height + 11 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26626), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 49 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26630), { 0, 0, height + 29 },
                    { { 0, 10, height + 11 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26627), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 49 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26628), { 0, 0, height + 29 },
                    { { 0, 6, height + 61 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26573), { 0, 0, height + 29 },
                    { { 0, 6, height + 61 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26577), { 0, 0, height + 29 },
                    { { 0, 10, height + 11 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26574), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 49 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26578), { 0, 0, height + 29 },
                    { { 0, 10, height + 11 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26575), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 49 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26576), { 0, 0, height + 29 },
                    { { 0, 6, height + 61 }, { 32, 20, 3 } });
                break;
        }
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 88);
}

/** rct2: 0x008AE730 */
static void CompactInvertedRCTrack60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26631), { 0, 0, height + 29 },
                    { { 0, 6, height + 61 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26635), { 0, 0, height + 29 },
                    { { 0, 10, height + 11 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26632), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 49 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26636), { 0, 0, height + 29 },
                    { { 0, 10, height + 11 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26633), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 49 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26634), { 0, 0, height + 29 },
                    { { 0, 6, height + 61 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26579), { 0, 0, height + 29 },
                    { { 0, 6, height + 61 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26583), { 0, 0, height + 29 },
                    { { 0, 10, height + 11 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26580), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 49 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26584), { 0, 0, height + 29 },
                    { { 0, 10, height + 11 }, { 32, 10, 49 } });
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26581), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 49 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26582), { 0, 0, height + 29 },
                    { { 0, 6, height + 61 }, { 32, 20, 3 } });
                break;
        }
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:

                break;
            case 1:

                break;
            case 2:

                break;
            case 3:

                break;
        }
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetGeneralSupportHeight(session, height + 88);
}

/** rct2: 0x008AE740 */
static void CompactInvertedRCTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26617), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26618), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26619), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26620), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26565), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26566), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26567), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26568), { 0, 0, height + 29 },
                    { { 0, 6, height + 37 }, { 32, 20, 3 } });
                break;
        }
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:

                break;
            case 1:

                break;
            case 2:

                break;
            case 3:

                break;
        }
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

/** rct2: 0x008AE750 */
static void CompactInvertedRCTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrack25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE760 */
static void CompactInvertedRCTrack60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrack60DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE770 */
static void CompactInvertedRCTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE780 */
static void CompactInvertedRCTrack25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrack60DegUpTo25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE790 */
static void CompactInvertedRCTrack60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrack25DegUpTo60DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE7A0 */
static void CompactInvertedRCTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE7B0 */
static void CompactInvertedRCTrackLeftQuarterTurn5(
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
                        session, direction, session.TrackColours.WithIndex(26694), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26699), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26704), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26689), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26693), { 0, 0, height + 29 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26698), { 0, 0, height + 29 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26703), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26688), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26692), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26697), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26702), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26687), { 0, 0, height + 29 }, { 16, 16, 3 });
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
                        session, direction, session.TrackColours.WithIndex(26691), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26696), { 0, 0, height + 29 }, { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26701), { 0, 0, height + 29 }, { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26686), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26690), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26695), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26700), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26685), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
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

/** rct2: 0x008AE7C0 */
static void CompactInvertedRCTrackRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    CompactInvertedRCTrackLeftQuarterTurn5(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE7D0 */
static void CompactInvertedRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26657), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26658), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26659), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26660), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AE7E0 */
static void CompactInvertedRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26661), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26662), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26663), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26664), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AE7F0 */
static void CompactInvertedRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26663), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26664), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26661), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26662), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AE800 */
static void CompactInvertedRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26659), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26660), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26657), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26658), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AE810 */
static void CompactInvertedRCTrackBankedLeftQuarterTurn5(
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
                        session, direction, session.TrackColours.WithIndex(26738), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26743), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26748), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26733), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26737), { 0, 0, height + 29 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26742), { 0, 0, height + 29 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26747), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26732), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26736), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26741), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26746), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26731), { 0, 0, height + 29 }, { 16, 16, 3 });
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
                        session, direction, session.TrackColours.WithIndex(26735), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26740), { 0, 0, height + 29 }, { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26745), { 0, 0, height + 29 }, { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26730), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26734), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26739), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26744), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26729), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
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

/** rct2: 0x008AE820 */
static void CompactInvertedRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    CompactInvertedRCTrackBankedLeftQuarterTurn5(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE830 */
static void CompactInvertedRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26665), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26666), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26667), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26668), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:

                break;
            case 1:

                break;
            case 2:

                break;
            case 3:

                break;
        }
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

/** rct2: 0x008AE840 */
static void CompactInvertedRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26669), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26670), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26671), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26672), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:

                break;
            case 1:

                break;
            case 2:

                break;
            case 3:

                break;
        }
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

/** rct2: 0x008AE850 */
static void CompactInvertedRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26673), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26674), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26675), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26676), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:

                break;
            case 1:

                break;
            case 2:

                break;
            case 3:

                break;
        }
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

/** rct2: 0x008AE860 */
static void CompactInvertedRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26677), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26678), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26679), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26680), { 0, 0, height + 29 },
                { { 0, 6, height + 37 }, { 32, 20, 3 } });
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        switch (direction)
        {
            case 0:

                break;
            case 1:

                break;
            case 2:

                break;
            case 3:

                break;
        }
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

/** rct2: 0x008AE870 */
static void CompactInvertedRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrack25DegUpToRightBank(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE880 */
static void CompactInvertedRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrack25DegUpToLeftBank(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE890 */
static void CompactInvertedRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrackRightBankTo25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE8A0 */
static void CompactInvertedRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrackLeftBankTo25DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE8B0 */
static void CompactInvertedRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26681), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26682), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26683), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26684), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AE8C0 */
static void CompactInvertedRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE8D0 */
static void CompactInvertedRCTrackLeftQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours.WithIndex(26949), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26954), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26959), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26964), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26950), { 0, 0, height + 29 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26955), { 0, 0, height + 29 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26960), { 0, 16, height + 29 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26965), { 0, 16, height + 29 }, { 32, 16, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26951), { 0, 16, height + 29 }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26956), { 16, 16, height + 29 }, { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26961), { 16, 0, height + 29 }, { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26966), { 0, 0, height + 29 }, { 16, 16, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26952), { 16, 0, height + 29 }, { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26957), { 0, 0, height + 29 }, { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26962), { 0, 0, height + 29 }, { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26967), { 16, 0, height + 29 }, { 16, 32, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26953), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26958), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26963), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26968), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
            }

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
    }
}

/** rct2: 0x008AE8E0 */
static void CompactInvertedRCTrackRightQuarterTurn525DegUp(
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
                        session, direction, session.TrackColours.WithIndex(26929), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26934), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26939), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26944), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26930), { 0, 16, height + 29 }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26935), { 0, 16, height + 29 }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26940), { 0, 0, height + 29 }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26945), { 0, 0, height + 29 }, { 32, 16, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26931), { 0, 0, height + 29 }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26936), { 16, 0, height + 29 }, { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26941), { 16, 16, height + 29 }, { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26946), { 0, 16, height + 29 }, { 16, 16, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26932), { 16, 0, height + 29 }, { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26937), { 0, 0, height + 29 }, { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26942), { 0, 0, height + 29 }, { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26947), { 16, 0, height + 29 }, { 16, 32, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26933), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26938), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26943), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26948), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
            }

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
    }
}

/** rct2: 0x008AE8F0 */
static void CompactInvertedRCTrackLeftQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    CompactInvertedRCTrackRightQuarterTurn525DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE900 */
static void CompactInvertedRCTrackRightQuarterTurn525DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    CompactInvertedRCTrackLeftQuarterTurn525DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE910 */
static void CompactInvertedRCTrackSBendLeft(
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
                        session, direction, session.TrackColours.WithIndex(26641), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26645), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26644), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26648), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26642), { 0, 0, height + 29 }, { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26646), { 0, 0, height + 29 }, { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26643), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26647), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26643), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26647), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26642), { 0, 0, height + 29 }, { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26646), { 0, 0, height + 29 }, { 32, 26, 3 });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26644), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26648), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26641), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26645), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
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

/** rct2: 0x008AE920 */
static void CompactInvertedRCTrackSBendRight(
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
                        session, direction, session.TrackColours.WithIndex(26649), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26653), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26652), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26656), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26650), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26654), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26651), { 0, 0, height + 29 }, { 32, 26, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26655), { 0, 0, height + 29 }, { 32, 26, 3 });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26651), { 0, 0, height + 29 }, { 32, 26, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26655), { 0, 0, height + 29 }, { 32, 26, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26650), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26654), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 26, 3 } });
                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26652), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26656), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26649), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26653), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
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

/** rct2: 0x008AE930 */
static void CompactInvertedRCTrackLeftVerticalLoop(
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
                        session, direction, session.TrackColours.WithIndex(27017), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27025), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27024), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27032), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 10, 7 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27018), { 0, 0, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27026), { 0, 14, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 63 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27023), { 0, 6, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27031), { 0, 6, height + 29 },
                        { { 29, 20, height + 29 }, { 2, 2, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27019), { 16, 0, height + 2 },
                        { { 16, 29, height + 2 }, { 5, 2, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27027), { 12, 0, height + 2 },
                        { { 0, -6, height + 2 }, { 32, 2, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27022), { 10, 16, height + 2 },
                        { { 10, 0, height + 2 }, { 4, 2, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27030), { 16, 16, height + 2 },
                        { { 29, 29, height + 2 }, { 2, 2, 119 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27020), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27028), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27021), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27029), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27021), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27029), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27020), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27028), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27022), { 10, 16, height + 2 },
                        { { 10, 0, height + 2 }, { 4, 2, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27030), { 16, 16, height + 2 },
                        { { 29, 29, height + 2 }, { 2, 2, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27019), { 16, 0, height + 2 },
                        { { 16, 29, height + 2 }, { 5, 2, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27027), { 12, 0, height + 2 },
                        { { 0, -6, height + 2 }, { 32, 2, 119 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27023), { 0, 6, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27031), { 0, 6, height + 29 },
                        { { 29, 20, height + 29 }, { 2, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27018), { 0, 0, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27026), { 0, 14, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 63 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27024), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27032), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 10, 7 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27017), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27025), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008AE940 */
static void CompactInvertedRCTrackRightVerticalLoop(
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
                        session, direction, session.TrackColours.WithIndex(27048), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 10, 7 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27040), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27041), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27033), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27047), { 0, 6, height + 29 },
                        { { 29, 20, height + 29 }, { 2, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27039), { 0, 6, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27042), { 0, 14, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 63 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27034), { 0, 0, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27046), { 16, 16, height + 2 },
                        { { 29, 29, height + 2 }, { 2, 2, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27038), { 10, 16, height + 2 },
                        { { 10, 0, height + 2 }, { 4, 2, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27043), { 12, 0, height + 2 },
                        { { 0, -6, height + 2 }, { 32, 2, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27035), { 16, 0, height + 2 },
                        { { 16, 29, height + 2 }, { 5, 2, 119 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27045), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27037), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27044), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27036), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 5:
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27044), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27036), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27045), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27037), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 7:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27043), { 12, 0, height + 2 },
                        { { 0, -6, height + 2 }, { 32, 2, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27035), { 16, 0, height + 2 },
                        { { 16, 29, height + 2 }, { 5, 2, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27046), { 16, 16, height + 2 },
                        { { 29, 29, height + 2 }, { 2, 2, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27038), { 10, 16, height + 2 },
                        { { 10, 0, height + 2 }, { 4, 2, 119 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 8:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27042), { 0, 14, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 63 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27034), { 0, 0, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27047), { 0, 6, height + 29 },
                        { { 29, 20, height + 29 }, { 2, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27039), { 0, 6, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 2, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 9:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27041), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27033), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27048), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 10, 7 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27040), { 0, 6, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008AE980 */
static void CompactInvertedRCTrackLeftQuarterTurn3(
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
                        session, direction, session.TrackColours.WithIndex(26710), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26713), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26716), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26707), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26709), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26712), { 0, 0, height + 29 }, { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26715), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26706), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26708), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26711), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26714), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26705), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
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

/** rct2: 0x008AE990 */
static void CompactInvertedRCTrackRightQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    CompactInvertedRCTrackLeftQuarterTurn3(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE9A0 */
static void CompactInvertedRCTrackLeftQuarterTurn3Bank(
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
                        session, direction, session.TrackColours.WithIndex(26722), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26725), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26728), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26719), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26721), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26724), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26727), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26718), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26720), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26723), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26726), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26717), { 0, 0, height + 29 },
                        { { 6, 0, height + 29 }, { 20, 32, 3 } });
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

/** rct2: 0x008AE9B0 */
static void CompactInvertedRCTrackRightQuarterTurn3Bank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    CompactInvertedRCTrackLeftQuarterTurn3Bank(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE9C0 */
static void CompactInvertedRCTrackLeftQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours.WithIndex(26924), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26926), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26928), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26922), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26923), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26925), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26927), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26921), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
            }

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
    }
}

/** rct2: 0x008AE9D0 */
static void CompactInvertedRCTrackRightQuarterTurn325DegUp(
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
                        session, direction, session.TrackColours.WithIndex(26913), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26915), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26917), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26919), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26914), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26916), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26918), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26920), { 6, 0, height + 29 }, { 20, 32, 3 });
                    break;
            }

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
    }
}

/** rct2: 0x008AE9E0 */
static void CompactInvertedRCTrackLeftQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    CompactInvertedRCTrackRightQuarterTurn325DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AE9F0 */
static void CompactInvertedRCTrackRightQuarterTurn325DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    CompactInvertedRCTrackLeftQuarterTurn325DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AEA00 */
static void CompactInvertedRCTrackLeftTwistDownToUp(
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
                        session, direction, session.TrackColours.WithIndex(26993), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26999), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26998), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27004), { 0, 6, height + 29 }, { 32, 20, 3 });
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
                        session, direction, session.TrackColours.WithIndex(26994), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27000), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26997), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27003), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26995), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27001), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26996), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27002), { 0, 6, height }, { 32, 20, 3 });
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

/** rct2: 0x008AEA10 */
static void CompactInvertedRCTrackRightTwistDownToUp(
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
                        session, direction, session.TrackColours.WithIndex(27005), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27011), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27010), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27016), { 0, 6, height + 29 }, { 32, 20, 3 });
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
                        session, direction, session.TrackColours.WithIndex(27006), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27012), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27009), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27015), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27007), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27013), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27008), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27014), { 0, 6, height }, { 32, 20, 3 });
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

/** rct2: 0x008AEA20 */
static void CompactInvertedRCTrackLeftTwistUpToDown(
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
                        session, direction, session.TrackColours.WithIndex(26996), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27002), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26995), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27001), { 0, 6, height }, { 32, 20, 3 });
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
                        session, direction, session.TrackColours.WithIndex(26997), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27003), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26994), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27000), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26998), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27004), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26993), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26999), { 0, 6, height + 29 }, { 32, 20, 3 });
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

/** rct2: 0x008AEA30 */
static void CompactInvertedRCTrackRightTwistUpToDown(
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
                        session, direction, session.TrackColours.WithIndex(27008), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27014), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27007), { 0, 6, height }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27013), { 0, 6, height }, { 32, 20, 3 });
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
                        session, direction, session.TrackColours.WithIndex(27009), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27015), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27006), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27012), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27010), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27016), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27005), { 0, 6, height + 29 }, { 32, 20, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27011), { 0, 6, height + 29 }, { 32, 20, 3 });
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

/** rct2: 0x008AEA40 */
static void CompactInvertedRCTrackHalfLoopUp(
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
                        session, direction, session.TrackColours.WithIndex(27073), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27081), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27080), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27088), { 0, 6, height + 29 },
                        { { 0, 6, height + 45 }, { 32, 20, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27074), { 0, 0, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27082), { 0, 14, height + 29 },
                        { { 0, 29, height + 29 }, { 32, 2, 63 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27079), { 0, 6, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27087), { 0, 6, height + 29 },
                        { { 29, 20, height + 29 }, { 2, 2, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27075), { 16, 0, height + 2 },
                        { { 16, 29, height + 2 }, { 5, 2, 119 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27083), { 12, 0, height + 2 },
                        { { 0, -6, height + 2 }, { 32, 2, 119 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27078), { 10, 16, height + 2 },
                        { { 10, 0, height + 2 }, { 4, 2, 119 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27086), { 16, 16, height + 2 },
                        { { 29, 29, height + 2 }, { 2, 2, 119 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 168);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27076), { 0, 0, height + 34 },
                        { { 0, 6, height + 34 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27084), { 0, 0, height + 34 },
                        { { 0, 0, height - 6 }, { 2, 2, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27077), { 0, 16, height + 34 },
                        { { 0, 0, height - 6 }, { 32, 2, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27085), { 0, 16, height + 34 },
                        { { 0, 6, height + 34 }, { 32, 20, 3 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 32, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

/** rct2: 0x008AEA50 */
static void CompactInvertedRCTrackHalfLoopDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrackHalfLoopUp(session, ride, 3 - trackSequence, direction, height, trackElement, supportType);
}

/** rct2: 0x008AEA60 */
static void CompactInvertedRCTrackLeftCorkscrewUp(
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
                        session, direction, session.TrackColours.WithIndex(27049), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27052), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27055), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27058), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27050), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27053), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27056), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27059), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27051), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27054), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27057), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27060), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 40, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 40, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

/** rct2: 0x008AEA70 */
static void CompactInvertedRCTrackRightCorkscrewUp(
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
                        session, direction, session.TrackColours.WithIndex(27061), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27064), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27067), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27070), { 0, 0, height + 29 },
                        { { 0, 6, height + 33 }, { 32, 20, 3 } });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27062), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27065), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27068), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27071), { 0, 0, height + 14 },
                        { { 6, 6, height + 24 }, { 20, 20, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27063), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27066), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27069), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27072), { 0, 0, height + 29 },
                        { { 6, 0, height + 39 }, { 20, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 40, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 40, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

/** rct2: 0x008AEA80 */
static void CompactInvertedRCTrackLeftCorkscrewDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrackRightCorkscrewUp(
        session, ride, 2 - trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AEA90 */
static void CompactInvertedRCTrackRightCorkscrewDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrackLeftCorkscrewUp(
        session, ride, 2 - trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AEAD0 */
static void CompactInvertedRCTrackLeftQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26910), { 0, 0, height + 5 },
                { { 2, 2, height + 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26906), { 0, 0, height + 5 },
                { { 2, 2, height + 104 }, { 28, 28, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26911), { 0, 0, height + 5 },
                { { 2, 2, height + 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26907), { 0, 0, height + 5 },
                { { 2, 2, height + 104 }, { 28, 28, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26912), { 0, 0, height + 5 },
                { { 2, 2, height + 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26908), { 0, 0, height + 5 },
                { { 2, 2, height + 104 }, { 28, 28, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26909), { 0, 0, height + 5 },
                { { 2, 2, height + 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26905), { 0, 0, height + 5 },
                { { 2, 2, height + 104 }, { 28, 28, 1 } });
            break;
    }
    TrackPaintUtilLeftQuarterTurn1TileTunnel(
        session, kTunnelGroup, direction, height, -8, TunnelSubType::SlopeStart, +56, TunnelSubType::SlopeEnd);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x008AEAB0 */
static void CompactInvertedRCTrackRightQuarterTurn160DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26901), { 0, 0, height + 5 },
                { { 2, 2, height + 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26897), { 0, 0, height + 5 },
                { { 2, 2, height + 104 }, { 28, 28, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26902), { 0, 0, height + 5 },
                { { 2, 2, height + 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26898), { 0, 0, height + 5 },
                { { 2, 2, height + 104 }, { 28, 28, 1 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26903), { 0, 0, height + 5 },
                { { 2, 2, height + 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26899), { 0, 0, height + 5 },
                { { 2, 2, height + 104 }, { 28, 28, 1 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26904), { 0, 0, height + 5 },
                { { 2, 2, height + 5 }, { 28, 28, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26900), { 0, 0, height + 5 },
                { { 2, 2, height + 104 }, { 28, 28, 1 } });
            break;
    }
    TrackPaintUtilRightQuarterTurn1TileTunnel(
        session, kTunnelGroup, direction, height, -8, TunnelSubType::SlopeStart, +56, TunnelSubType::SlopeEnd);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

/** rct2: 0x008AEAC0 */
static void CompactInvertedRCTrackLeftQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrackRightQuarterTurn160DegUp(
        session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AEAE0 */
static void CompactInvertedRCTrackRightQuarterTurn160DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrackLeftQuarterTurn160DegUp(
        session, ride, trackSequence, (direction - 1) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AEAA0 */
static void CompactInvertedRCTrackBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26559), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26560), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AEAF0 */
static void CompactInvertedRCTrackLeftQuarterBankedHelixLargeUp(
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
                        session, direction, session.TrackColours.WithIndex(27118), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27123), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27128), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27113), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(27117), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27122), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27127), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27112), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27116), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27121), { 0, 0, height + 29 },
                        { { 16, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27126), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27111), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(27115), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27120), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27125), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27110), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27114), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27119), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27124), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27109), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 16, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 16, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AEB00 */
static void CompactInvertedRCTrackRightQuarterBankedHelixLargeUp(
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
                        session, direction, session.TrackColours.WithIndex(27089), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27094), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27099), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27104), { 0, 0, height + 29 },
                        { { 0, 6, height + 35 }, { 32, 20, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(27090), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27095), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27100), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27105), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27091), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27096), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27101), { 0, 0, height + 29 },
                        { { 16, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27106), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(27092), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27097), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27102), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27107), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27093), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27098), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27103), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27108), { 0, 0, height + 29 },
                        { { 6, 0, height + 43 }, { 20, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 16, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 16, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AEB10 */
static void CompactInvertedRCTrackLeftQuarterBankedHelixLargeDown(
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
                        session, direction, session.TrackColours.WithIndex(27098), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27103), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27108), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27093), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::Flat);
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
                        session, direction, session.TrackColours.WithIndex(27097), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27102), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27107), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27092), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27096), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27101), { 0, 0, height + 29 },
                        { { 16, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27106), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27091), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(27095), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27100), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27105), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27090), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27094), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27099), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27104), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27089), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
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

/** rct2: 0x008AEB20 */
static void CompactInvertedRCTrackRightQuarterBankedHelixLargeDown(
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
                        session, direction, session.TrackColours.WithIndex(27109), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27114), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27119), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27124), { 0, 0, height + 29 },
                        { { 0, 6, height + 43 }, { 32, 20, 3 } });
                    break;
            }

            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::Flat);
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
                        session, direction, session.TrackColours.WithIndex(27110), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27115), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27120), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27125), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27111), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27116), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27121), { 0, 0, height + 29 },
                        { { 16, 16, height + 43 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27126), { 0, 0, height + 29 },
                        { { 0, 16, height + 43 }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(27112), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27117), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27122), { 0, 0, height + 29 },
                        { { 0, 0, height + 43 }, { 16, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27127), { 0, 0, height + 29 },
                        { { 16, 0, height + 43 }, { 16, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27113), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27118), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27123), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(27128), { 0, 0, height + 29 },
                        { { 6, 0, height + 35 }, { 20, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AEB30 */
static void CompactInvertedRCTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackPaintUtilOnridePhotoPlatformPaint(session, direction, height, supportType.metal);

    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26555), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26556), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26555), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26556), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    TrackPaintUtilOnridePhotoPaint2(session, direction, trackElement, height, kGeneralSupportHeightOnRidePhotoInverted);
}

/** rct2: 0x008AEDB0 */
static void CompactInvertedRCTrack90DegUp(
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
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26609), { 0, 0, height + 29 },
                            { { 0, 6, height + 61 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26610), { 0, 0, height + 29 },
                            { { 0, 4, height + 11 }, { 32, 2, 31 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26611), { 0, 0, height + 29 },
                            { { 0, 4, height + 11 }, { 32, 2, 31 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26612), { 0, 0, height + 29 },
                            { { 0, 6, height + 61 }, { 32, 20, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26605), { 0, 0, height + 29 },
                            { { 0, 6, height + 61 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26606), { 0, 0, height + 29 },
                            { { 0, 4, height + 11 }, { 32, 2, 31 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26607), { 0, 0, height + 29 },
                            { { 0, 4, height + 11 }, { 32, 2, 31 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26608), { 0, 0, height + 29 },
                            { { 0, 6, height + 61 }, { 32, 20, 3 } });
                        break;
                }
            }
            PaintUtilSetVerticalTunnel(session, height + 32);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008AEDC0 */
static void CompactInvertedRCTrack90DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrack90DegUp(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AED70 */
static void CompactInvertedRCTrack60DegUpTo90DegUp(
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
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26597), { 0, 0, height + 29 },
                            { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26598), { 0, 0, height + 29 },
                            { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26599), { 0, 0, height + 29 },
                            { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26600), { 0, 0, height + 29 },
                            { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26589), { 0, 0, height + 29 },
                            { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26590), { 0, 0, height + 29 },
                            { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26591), { 0, 0, height + 29 },
                            { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26592), { 0, 0, height + 29 },
                            { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                }
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetVerticalTunnel(session, height + 56);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008AED80 */
static void CompactInvertedRCTrack90DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrack60DegUpTo90DegUp(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AED90 */
static void CompactInvertedRCTrack90DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (trackElement.HasChain())
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26601), { 0, 0, height + 29 },
                    { { 0, 6, height + 85 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26602), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 55 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26603), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 55 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26604), { 0, 0, height + 29 },
                    { { 0, 6, height + 85 }, { 32, 20, 3 } });
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26593), { 0, 0, height + 29 },
                    { { 0, 6, height + 85 }, { 32, 20, 3 } });
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26594), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 55 } });
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26595), { 0, 0, height + 29 },
                    { { 0, 4, height + 11 }, { 32, 2, 55 } });
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex(26596), { 0, 0, height + 29 },
                    { { 0, 6, height + 85 }, { 32, 20, 3 } });
                break;
        }
    }
    switch (direction)
    {
        case 1:
            PaintUtilPushTunnelRight(session, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
    }
    PaintUtilSetGeneralSupportHeight(session, height + 96);
}

/** rct2: 0x008AEDA0 */
static void CompactInvertedRCTrack60DegDownTo90DegDown(
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
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26603), { 0, 0, height + 29 },
                            { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26604), { 0, 0, height + 29 },
                            { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26601), { 0, 0, height + 29 },
                            { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26602), { 0, 0, height + 29 },
                            { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26595), { 0, 0, height + 29 },
                            { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26596), { 0, 0, height + 29 },
                            { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26593), { 0, 0, height + 29 },
                            { { 0, 6, height + 85 }, { 32, 20, 3 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26594), { 0, 0, height + 29 },
                            { { 0, 4, height + 11 }, { 32, 2, 55 } });
                        break;
                }
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
            }
            PaintUtilSetGeneralSupportHeight(session, height + 96);
            break;
        case 1:
            break;
    }
}

/** rct2: 0x008AEB40 */
static void CompactInvertedRCTrackLeftEighthToDiag(
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
                        session, direction, session.TrackColours.WithIndex(26765), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26769), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26773), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26777), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26766), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26770), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 34, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26774), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26778), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26767), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26771), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26775), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26779), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26768), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26772), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 18, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26776), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26780), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AEB50 */
static void CompactInvertedRCTrackRightEighthToDiag(
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
                        session, direction, session.TrackColours.WithIndex(26749), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26753), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26757), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26761), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26750), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26754), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26758), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 34, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26762), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26751), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26755), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26759), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26763), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26752), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26756), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26760), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 18, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26764), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AEB60 */
static void CompactInvertedRCTrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    CompactInvertedRCTrackRightEighthToDiag(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AEB70 */
static void CompactInvertedRCTrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    CompactInvertedRCTrackLeftEighthToDiag(
        session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AED30 */
static void CompactInvertedRCTrackLeftEighthBankToDiag(
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
                        session, direction, session.TrackColours.WithIndex(26853), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26857), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26861), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26865), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26854), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26858), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 34, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26862), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26866), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26855), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26859), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26863), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26867), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26856), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26860), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 18, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26864), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26868), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AED40 */
static void CompactInvertedRCTrackRightEighthBankToDiag(
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
                        session, direction, session.TrackColours.WithIndex(26837), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26841), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26845), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26849), { 0, 0, height + 29 },
                        { { 0, 6, height + 29 }, { 32, 20, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26838), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26842), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26846), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 34, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26850), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26839), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26843), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26847), { 0, 0, height + 29 },
                        { { 4, 4, height + 29 }, { 28, 28, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26851), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 16, 3 } });
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
                        session, direction, session.TrackColours.WithIndex(26840), { 0, 0, height + 29 },
                        { { 16, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26844), { 0, 0, height + 29 },
                        { { 0, 0, height + 29 }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26848), { 0, 0, height + 29 },
                        { { 0, 16, height + 29 }, { 16, 18, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26852), { 0, 0, height + 29 },
                        { { 16, 16, height + 29 }, { 16, 16, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AED50 */
static void CompactInvertedRCTrackLeftEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    CompactInvertedRCTrackRightEighthBankToDiag(
        session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

/** rct2: 0x008AED60 */
static void CompactInvertedRCTrackRightEighthBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    CompactInvertedRCTrackLeftEighthBankToDiag(
        session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
}

inline void CompactInvertedRCTrackDiagFlatBase(
    PaintSession& session, uint8_t trackSequence, const ImageIndex* images, uint8_t direction, int32_t height,
    SupportType supportType)
{
    TrackPaintUtilDiagTilesPaint(
        session, 3, height + 29, direction, trackSequence, images, defaultDiagTileOffsets, defaultDiagBoundLengths, nullptr);

    if (trackSequence == 3)
    {
    }
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x008AEB80 */
static void CompactInvertedRCTrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const auto* images = kCompactInvertedDiagFlatImages[trackElement.HasChain()];
    CompactInvertedRCTrackDiagFlatBase(session, trackSequence, images, direction, height, supportType);
}

static void CompactInvertedRCTrackDiagBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    CompactInvertedRCTrackDiagFlatBase(session, trackSequence, kCompactInvertedDiagBrakeImages, direction, height, supportType);
}

static void CompactInvertedRCTrackDiagBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const auto* images = kCompactInvertedDiagBlockBrakeImages[trackElement.IsBrakeClosed()];
    CompactInvertedRCTrackDiagFlatBase(session, trackSequence, images, direction, height, supportType);
}

/** rct2: 0x008AEBB0 */
static void CompactInvertedRCTrackDiag25DegUp(
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
                            session, direction, session.TrackColours.WithIndex(26824), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26796), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26821), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26793), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26823), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26795), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26822), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26794), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008AEC10 */
static void CompactInvertedRCTrackDiag60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26808), { -16, -16, height + 29 },
                        { { -16, -16, height + 93 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26805), { -16, -16, height + 29 },
                        { { -16, -16, height + 93 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26807), { -16, -16, height + 29 },
                        { { -16, -16, height + 93 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26806), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
    }
}

/** rct2: 0x008AEB90 */
static void CompactInvertedRCTrackDiagFlatTo25DegUp(
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
                            session, direction, session.TrackColours.WithIndex(26816), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26788), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26813), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26785), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26815), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26787), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26814), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26786), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

/** rct2: 0x008AEBF0 */
static void CompactInvertedRCTrackDiag25DegUpTo60DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26800), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26797), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26799), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26798), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
    }
}

/** rct2: 0x008AEC00 */
static void CompactInvertedRCTrackDiag60DegUpTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26804), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26801), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26803), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26802), { -16, -16, height + 29 },
                        { { 0, 0, height + 61 }, { 16, 16, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
    }
}

/** rct2: 0x008AEBA0 */
static void CompactInvertedRCTrackDiag25DegUpToFlat(
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
                            session, direction, session.TrackColours.WithIndex(26820), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26792), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26817), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26789), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26819), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26791), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26818), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26790), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008AEBE0 */
static void CompactInvertedRCTrackDiag25DegDown(
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
                            session, direction, session.TrackColours.WithIndex(26822), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26794), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26823), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26795), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26821), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26793), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26824), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26796), { -16, -16, height + 29 },
                            { { -16, -16, height + 45 }, { 32, 32, 3 } });
                        break;
                }
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008AEC40 */
static void CompactInvertedRCTrackDiag60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26806), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26807), { -16, -16, height + 29 },
                        { { -16, -16, height + 93 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26805), { -16, -16, height + 29 },
                        { { -16, -16, height + 93 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26808), { -16, -16, height + 29 },
                        { { -16, -16, height + 93 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 120);
            break;
    }
}

/** rct2: 0x008AEBC0 */
static void CompactInvertedRCTrackDiagFlatTo25DegDown(
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
                            session, direction, session.TrackColours.WithIndex(26818), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26790), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26819), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26791), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26817), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26789), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26820), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26792), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x008AEC20 */
static void CompactInvertedRCTrackDiag25DegDownTo60DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26802), { -16, -16, height + 29 },
                        { { 0, 0, height + 61 }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26803), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26801), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26804), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
    }
}

/** rct2: 0x008AEC30 */
static void CompactInvertedRCTrackDiag60DegDownTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26798), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26799), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26797), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26800), { -16, -16, height + 29 },
                        { { -16, -16, height + 61 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 88);
            break;
    }
}

/** rct2: 0x008AEBD0 */
static void CompactInvertedRCTrackDiag25DegDownToFlat(
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
                            session, direction, session.TrackColours.WithIndex(26814), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26786), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26815), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26787), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26813), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26785), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
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
                            session, direction, session.TrackColours.WithIndex(26816), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }
            else
            {
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex(26788), { -16, -16, height + 29 },
                            { { -16, -16, height + 37 }, { 32, 32, 3 } });
                        break;
                }
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

/** rct2: 0x008AEC70 */
static void CompactInvertedRCTrackDiagFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26876), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26873), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26875), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26874), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AEC80 */
static void CompactInvertedRCTrackDiagFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26880), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26877), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26879), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26878), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AEC90 */
static void CompactInvertedRCTrackDiagLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26878), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26879), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26877), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26880), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AECA0 */
static void CompactInvertedRCTrackDiagRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26874), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26875), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26873), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26876), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AECD0 */
static void CompactInvertedRCTrackDiagLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26892), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26889), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26891), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26890), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

/** rct2: 0x008AECE0 */
static void CompactInvertedRCTrackDiagRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26896), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26893), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26895), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26894), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

/** rct2: 0x008AECB0 */
static void CompactInvertedRCTrackDiag25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26884), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26881), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26883), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26882), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008AECC0 */
static void CompactInvertedRCTrackDiag25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26888), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26885), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26887), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26886), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

/** rct2: 0x008AECF0 */
static void CompactInvertedRCTrackDiagLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26886), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26887), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26885), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26888), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x008AED00 */
static void CompactInvertedRCTrackDiagRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26882), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26883), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26881), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26884), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

/** rct2: 0x008AED10 */
static void CompactInvertedRCTrackDiag25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26894), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26895), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26893), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26896), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

/** rct2: 0x008AED20 */
static void CompactInvertedRCTrackDiag25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26890), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26891), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26889), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26892), { -16, -16, height + 29 },
                        { { -16, -16, height + 37 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
    }
}

/** rct2: 0x008AEC50 */
static void CompactInvertedRCTrackDiagLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26872), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26869), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26871), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26870), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AEC60 */
static void CompactInvertedRCTrackDiagRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26870), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26871), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26869), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(26872), { -16, -16, height + 29 },
                        { { -16, -16, height + 29 }, { 32, 32, 3 } });
                    break;
            }

            switch (direction)
            {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                case 3:

                    break;
            }

            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
    }
}

/** rct2: 0x008AEAA0 */
static void CompactInvertedRCTrackBlockBrakes(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26559), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
        case 1:
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(26560), { 0, 0, height + 29 },
                { { 0, 6, height + 29 }, { 32, 20, 3 } });
            break;
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

TrackPaintFunction GetTrackPaintFunctionCompactInvertedRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return CompactInvertedRCTrackFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return CompactInvertedRCTrackStation;
        case TrackElemType::Up25:
            return CompactInvertedRCTrack25DegUp;
        case TrackElemType::Up60:
            return CompactInvertedRCTrack60DegUp;
        case TrackElemType::FlatToUp25:
            return CompactInvertedRCTrackFlatTo25DegUp;
        case TrackElemType::Up25ToUp60:
            return CompactInvertedRCTrack25DegUpTo60DegUp;
        case TrackElemType::Up60ToUp25:
            return CompactInvertedRCTrack60DegUpTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return CompactInvertedRCTrack25DegUpToFlat;
        case TrackElemType::Down25:
            return CompactInvertedRCTrack25DegDown;
        case TrackElemType::Down60:
            return CompactInvertedRCTrack60DegDown;
        case TrackElemType::FlatToDown25:
            return CompactInvertedRCTrackFlatTo25DegDown;
        case TrackElemType::Down25ToDown60:
            return CompactInvertedRCTrack25DegDownTo60DegDown;
        case TrackElemType::Down60ToDown25:
            return CompactInvertedRCTrack60DegDownTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return CompactInvertedRCTrack25DegDownToFlat;
        case TrackElemType::LeftQuarterTurn5Tiles:
            return CompactInvertedRCTrackLeftQuarterTurn5;
        case TrackElemType::RightQuarterTurn5Tiles:
            return CompactInvertedRCTrackRightQuarterTurn5;
        case TrackElemType::FlatToLeftBank:
            return CompactInvertedRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return CompactInvertedRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return CompactInvertedRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return CompactInvertedRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return CompactInvertedRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return CompactInvertedRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankToUp25:
            return CompactInvertedRCTrackLeftBankTo25DegUp;
        case TrackElemType::RightBankToUp25:
            return CompactInvertedRCTrackRightBankTo25DegUp;
        case TrackElemType::Up25ToLeftBank:
            return CompactInvertedRCTrack25DegUpToLeftBank;
        case TrackElemType::Up25ToRightBank:
            return CompactInvertedRCTrack25DegUpToRightBank;
        case TrackElemType::LeftBankToDown25:
            return CompactInvertedRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return CompactInvertedRCTrackRightBankTo25DegDown;
        case TrackElemType::Down25ToLeftBank:
            return CompactInvertedRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return CompactInvertedRCTrack25DegDownToRightBank;
        case TrackElemType::LeftBank:
            return CompactInvertedRCTrackLeftBank;
        case TrackElemType::RightBank:
            return CompactInvertedRCTrackRightBank;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return CompactInvertedRCTrackLeftQuarterTurn525DegUp;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return CompactInvertedRCTrackRightQuarterTurn525DegUp;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return CompactInvertedRCTrackLeftQuarterTurn525DegDown;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return CompactInvertedRCTrackRightQuarterTurn525DegDown;
        case TrackElemType::SBendLeft:
            return CompactInvertedRCTrackSBendLeft;
        case TrackElemType::SBendRight:
            return CompactInvertedRCTrackSBendRight;
        case TrackElemType::LeftVerticalLoop:
            return CompactInvertedRCTrackLeftVerticalLoop;
        case TrackElemType::RightVerticalLoop:
            return CompactInvertedRCTrackRightVerticalLoop;
        case TrackElemType::LeftQuarterTurn3Tiles:
            return CompactInvertedRCTrackLeftQuarterTurn3;
        case TrackElemType::RightQuarterTurn3Tiles:
            return CompactInvertedRCTrackRightQuarterTurn3;
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return CompactInvertedRCTrackLeftQuarterTurn3Bank;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return CompactInvertedRCTrackRightQuarterTurn3Bank;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return CompactInvertedRCTrackLeftQuarterTurn325DegUp;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return CompactInvertedRCTrackRightQuarterTurn325DegUp;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return CompactInvertedRCTrackLeftQuarterTurn325DegDown;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return CompactInvertedRCTrackRightQuarterTurn325DegDown;
        case TrackElemType::LeftTwistDownToUp:
            return CompactInvertedRCTrackLeftTwistDownToUp;
        case TrackElemType::RightTwistDownToUp:
            return CompactInvertedRCTrackRightTwistDownToUp;
        case TrackElemType::LeftTwistUpToDown:
            return CompactInvertedRCTrackLeftTwistUpToDown;
        case TrackElemType::RightTwistUpToDown:
            return CompactInvertedRCTrackRightTwistUpToDown;
        case TrackElemType::HalfLoopUp:
            return CompactInvertedRCTrackHalfLoopUp;
        case TrackElemType::HalfLoopDown:
            return CompactInvertedRCTrackHalfLoopDown;
        case TrackElemType::LeftCorkscrewUp:
            return CompactInvertedRCTrackLeftCorkscrewUp;
        case TrackElemType::RightCorkscrewUp:
            return CompactInvertedRCTrackRightCorkscrewUp;
        case TrackElemType::LeftCorkscrewDown:
            return CompactInvertedRCTrackLeftCorkscrewDown;
        case TrackElemType::RightCorkscrewDown:
            return CompactInvertedRCTrackRightCorkscrewDown;
        case TrackElemType::LeftQuarterTurn1TileUp60:
            return CompactInvertedRCTrackLeftQuarterTurn160DegUp;
        case TrackElemType::RightQuarterTurn1TileUp60:
            return CompactInvertedRCTrackRightQuarterTurn160DegUp;
        case TrackElemType::LeftQuarterTurn1TileDown60:
            return CompactInvertedRCTrackLeftQuarterTurn160DegDown;
        case TrackElemType::RightQuarterTurn1TileDown60:
            return CompactInvertedRCTrackRightQuarterTurn160DegDown;
        case TrackElemType::Brakes:
            return CompactInvertedRCTrackBrakes;
        case TrackElemType::LeftQuarterBankedHelixLargeUp:
            return CompactInvertedRCTrackLeftQuarterBankedHelixLargeUp;
        case TrackElemType::RightQuarterBankedHelixLargeUp:
            return CompactInvertedRCTrackRightQuarterBankedHelixLargeUp;
        case TrackElemType::LeftQuarterBankedHelixLargeDown:
            return CompactInvertedRCTrackLeftQuarterBankedHelixLargeDown;
        case TrackElemType::RightQuarterBankedHelixLargeDown:
            return CompactInvertedRCTrackRightQuarterBankedHelixLargeDown;
        case TrackElemType::OnRidePhoto:
            return CompactInvertedRCTrackOnRidePhoto;
        case TrackElemType::Up90:
            return CompactInvertedRCTrack90DegUp;
        case TrackElemType::Down90:
            return CompactInvertedRCTrack90DegDown;
        case TrackElemType::Up60ToUp90:
            return CompactInvertedRCTrack60DegUpTo90DegUp;
        case TrackElemType::Down90ToDown60:
            return CompactInvertedRCTrack90DegDownTo60DegDown;
        case TrackElemType::Up90ToUp60:
            return CompactInvertedRCTrack90DegUpTo60DegUp;
        case TrackElemType::Down60ToDown90:
            return CompactInvertedRCTrack60DegDownTo90DegDown;
        case TrackElemType::LeftEighthToDiag:
            return CompactInvertedRCTrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return CompactInvertedRCTrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return CompactInvertedRCTrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return CompactInvertedRCTrackRightEighthToOrthogonal;
        case TrackElemType::LeftEighthBankToDiag:
            return CompactInvertedRCTrackLeftEighthBankToDiag;
        case TrackElemType::RightEighthBankToDiag:
            return CompactInvertedRCTrackRightEighthBankToDiag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return CompactInvertedRCTrackLeftEighthBankToOrthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return CompactInvertedRCTrackRightEighthBankToOrthogonal;
        case TrackElemType::DiagFlat:
            return CompactInvertedRCTrackDiagFlat;
        case TrackElemType::DiagUp25:
            return CompactInvertedRCTrackDiag25DegUp;
        case TrackElemType::DiagUp60:
            return CompactInvertedRCTrackDiag60DegUp;
        case TrackElemType::DiagFlatToUp25:
            return CompactInvertedRCTrackDiagFlatTo25DegUp;
        case TrackElemType::DiagUp25ToUp60:
            return CompactInvertedRCTrackDiag25DegUpTo60DegUp;
        case TrackElemType::DiagUp60ToUp25:
            return CompactInvertedRCTrackDiag60DegUpTo25DegUp;
        case TrackElemType::DiagUp25ToFlat:
            return CompactInvertedRCTrackDiag25DegUpToFlat;
        case TrackElemType::DiagDown25:
            return CompactInvertedRCTrackDiag25DegDown;
        case TrackElemType::DiagDown60:
            return CompactInvertedRCTrackDiag60DegDown;
        case TrackElemType::DiagFlatToDown25:
            return CompactInvertedRCTrackDiagFlatTo25DegDown;
        case TrackElemType::DiagDown25ToDown60:
            return CompactInvertedRCTrackDiag25DegDownTo60DegDown;
        case TrackElemType::DiagDown60ToDown25:
            return CompactInvertedRCTrackDiag60DegDownTo25DegDown;
        case TrackElemType::DiagDown25ToFlat:
            return CompactInvertedRCTrackDiag25DegDownToFlat;
        case TrackElemType::DiagFlatToLeftBank:
            return CompactInvertedRCTrackDiagFlatToLeftBank;
        case TrackElemType::DiagFlatToRightBank:
            return CompactInvertedRCTrackDiagFlatToRightBank;
        case TrackElemType::DiagLeftBankToFlat:
            return CompactInvertedRCTrackDiagLeftBankToFlat;
        case TrackElemType::DiagRightBankToFlat:
            return CompactInvertedRCTrackDiagRightBankToFlat;
        case TrackElemType::DiagLeftBankToUp25:
            return CompactInvertedRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::DiagRightBankToUp25:
            return CompactInvertedRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::DiagUp25ToLeftBank:
            return CompactInvertedRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::DiagUp25ToRightBank:
            return CompactInvertedRCTrackDiag25DegUpToRightBank;
        case TrackElemType::DiagLeftBankToDown25:
            return CompactInvertedRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::DiagRightBankToDown25:
            return CompactInvertedRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::DiagDown25ToLeftBank:
            return CompactInvertedRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::DiagDown25ToRightBank:
            return CompactInvertedRCTrackDiag25DegDownToRightBank;
        case TrackElemType::DiagLeftBank:
            return CompactInvertedRCTrackDiagLeftBank;
        case TrackElemType::DiagRightBank:
            return CompactInvertedRCTrackDiagRightBank;
        case TrackElemType::BlockBrakes:
            return CompactInvertedRCTrackBlockBrakes;
        case TrackElemType::DiagBrakes:
            return CompactInvertedRCTrackDiagBrakes;
        case TrackElemType::DiagBlockBrakes:
            return CompactInvertedRCTrackDiagBlockBrakes;
        default:
            return TrackPaintFunctionDummy;
    }
}

// clang-format off
const TrackElementMetalSupports kCompactInvertedCoasterMetalSupports[EnumValue(TrackElemType::Count)] = {
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Flat
    { { { kMetalSupportDrawAll, MetalSupportPlace::BottomRightSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { kMetalSupportDrawAll, MetalSupportPlace::TopLeftSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // EndStation
    { { { kMetalSupportDrawAll, MetalSupportPlace::BottomRightSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { kMetalSupportDrawAll, MetalSupportPlace::TopLeftSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BeginStation
    { { { kMetalSupportDrawAll, MetalSupportPlace::BottomRightSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { kMetalSupportDrawAll, MetalSupportPlace::TopLeftSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // MiddleStation
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::TopRightSide, { 56, 56, 56, 56 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::TopRightSide, { 48, 48, 48, 48 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToUp60
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::TopRightSide, { 70, 70, 70, 70 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up60ToUp25
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::TopRightSide, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToFlat
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::BottomLeftSide, { 56, 56, 56, 56 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::BottomLeftSide, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToDown25
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::BottomLeftSide, { 70, 70, 70, 70 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down60ToDown25
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::BottomLeftSide, { 48, 48, 48, 48 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToFlat
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn5Tiles
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn5Tiles
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToLeftBank
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // FlatToRightBank
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankToFlat
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankToFlat
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BankedLeftQuarterTurn5Tiles
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BankedRightQuarterTurn5Tiles
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::TopRightSide, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankToUp25
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::TopRightSide, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankToUp25
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::TopRightSide, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToLeftBank
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::TopRightSide, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25ToRightBank
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::BottomLeftSide, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankToDown25
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::BottomLeftSide, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankToDown25
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::BottomLeftSide, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToLeftBank
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::BottomLeftSide, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Down25ToRightBank
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBank
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBank
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 48, 48, 48, 48 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 48, 48, 48, 48 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn5TilesUp25
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 48, 48, 48, 48 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 48, 48, 48, 48 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn5TilesUp25
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 48, 48, 48, 48 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 48, 48, 48, 48 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn5TilesDown25
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 48, 48, 48, 48 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 48, 48, 48, 48 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn5TilesDown25
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { EnumsToFlags(MetalSupportFlags::drawDirection0, MetalSupportFlags::drawDirection1), MetalSupportPlace::TopLeftSide, { 38, 38, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { EnumsToFlags(MetalSupportFlags::drawDirection2, MetalSupportFlags::drawDirection3), MetalSupportPlace::BottomRightSide, { 0, 0, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // SBendLeft
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { EnumsToFlags(MetalSupportFlags::drawDirection0, MetalSupportFlags::drawDirection1), MetalSupportPlace::BottomRightSide, { 38, 38, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { EnumsToFlags(MetalSupportFlags::drawDirection2, MetalSupportFlags::drawDirection3), MetalSupportPlace::TopLeftSide, { 0, 0, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // SBendRight
    { { { kMetalSupportDrawAll, MetalSupportPlace::TopRightSide, { 49, 49, 49, 49 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::BottomLeftSide, { 49, 49, 49, 49 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftVerticalLoop
    { { { kMetalSupportDrawAll, MetalSupportPlace::TopRightSide, { 49, 49, 49, 49 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::BottomLeftSide, { 49, 49, 49, 49 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightVerticalLoop
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn3Tiles
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn3Tiles
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftBankedQuarterTurn3Tiles
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightBankedQuarterTurn3Tiles
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn3TilesUp25
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn3TilesUp25
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn3TilesDown25
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 46, 46, 46, 46 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn3TilesDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterTurn1Tile
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterTurn1Tile
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftTwistDownToUp
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightTwistDownToUp
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftTwistUpToDown
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightTwistUpToDown
    { { { kMetalSupportDrawAll, MetalSupportPlace::TopRightSide, { 56, 56, 56, 56 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // HalfLoopUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::TopRightSide, { 56, 56, 56, 56 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // HalfLoopDown
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 28, 28, 28, 28 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftCorkscrewUp
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 28, 28, 28, 28 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightCorkscrewUp
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 28, 28, 28, 28 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftCorkscrewDown
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 28, 28, 28, 28 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightCorkscrewDown
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
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Brakes
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Booster
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Maze
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 56, 56, 56, 56 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterBankedHelixLargeUp
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 56, 56, 56, 56 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterBankedHelixLargeUp
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 56, 56, 56, 56 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterBankedHelixLargeDown
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 56, 56, 56, 56 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterBankedHelixLargeDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterHelixLargeUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterHelixLargeUp
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftQuarterHelixLargeDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightQuarterHelixLargeDown
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25LeftBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Up25RightBanked
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Waterfall
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // Rapids
    { { { kMetalSupportDrawAll, MetalSupportPlace::BottomRightSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { kMetalSupportDrawAll, MetalSupportPlace::TopLeftSide, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // OnRidePhoto
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
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::BottomCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToDiag
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToDiag
    { { { kMetalSupportDrawAll, MetalSupportPlace::RightCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthToOrthogonal
    { { { kMetalSupportDrawAll, MetalSupportPlace::RightCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthToOrthogonal
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::BottomCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToDiag
    { { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToDiag
    { { { kMetalSupportDrawAll, MetalSupportPlace::RightCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // LeftEighthBankToOrthogonal
    { { { kMetalSupportDrawAll, MetalSupportPlace::RightCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // RightEighthBankToOrthogonal
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 50, 50, 50, 50 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::noLongCrossbeam), MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 32, 36, 32, 36 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 42, 42, 42, 42 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::noLongCrossbeam), MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 16, 16, 16, 16 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::noLongCrossbeam), MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 21, 21, 21, 21 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp60ToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 50, 50, 50, 50 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::noLongCrossbeam), MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 24, 28, 24, 28 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::noLongCrossbeam), MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 17, 17, 17, 17 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::noLongCrossbeam), MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 8, 8, 8, 8 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown60ToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 42, 42, 42, 42 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToUp60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp60ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToDown60
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown60ToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagFlatToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankToFlat
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 42, 42, 42, 42 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 42, 42, 42, 42 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankToUp25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagUp25ToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBankToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 44, 44, 44, 44 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBankToDown25
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 42, 42, 42, 42 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 42, 42, 42, 42 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25ToRightBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagLeftBank
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagRightBank
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
    { { { kMetalSupportDrawAll | EnumToFlag(MetalSupportFlags::alternateTiles), MetalSupportPlace::Centre, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // BlockBrakes
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
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagBrakes
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 38, 38, 38, 38 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagBlockBrakes
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
    { { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, { { kMetalSupportDrawAll, MetalSupportPlace::LeftCorner, { 50, 50, 50, 50 }, { 0, 0, 0, 0 } }, { 0, MetalSupportPlace::None, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } } }, }, // DiagDown25Brakes
};
// clang-format on
