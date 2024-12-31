/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../Game.h"
#include "../../../GameState.h"
#include "../../../config/Config.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/Vehicle.h"
#include "../../../scenario/Scenario.h"
#include "../../../world/Map.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

enum
{
    SPR_RIVER_RAPIDS_FLAT_SW_NE = 21132,
    SPR_RIVER_RAPIDS_FLAT_NW_SE = 21133,
    SPR_RIVER_RAPIDS_FLAT_NE_SW = 21134,
    SPR_RIVER_RAPIDS_FLAT_SE_NW = 21135,
    SPR_RIVER_RAPIDS_FLAT_FRONT_SW_NE = 21136,
    SPR_RIVER_RAPIDS_FLAT_FRONT_NW_SE = 21137,
    SPR_RIVER_RAPIDS_FLAT_FRONT_NE_SW = 21138,
    SPR_RIVER_RAPIDS_FLAT_FRONT_SE_NW = 21139,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SW_NE = 21140,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NW_SE = 21141,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NE_SW = 21142,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SE_NW = 21143,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SW_NE = 21144,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NW_SE = 21145,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NE_SW = 21146,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SE_NW = 21147,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_SW_NE = 21148,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_NW_SE = 21149,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_NE_SW = 21150,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_SE_NW = 21151,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_SW_NE = 21152,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_NW_SE = 21153,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_NE_SW = 21154,
    SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_SE_NW = 21155,
    SPR_RIVER_RAPIDS_25_DEG_UP_SW_NE = 21156,
    SPR_RIVER_RAPIDS_25_DEG_UP_NW_SE = 21157,
    SPR_RIVER_RAPIDS_25_DEG_UP_NE_SW = 21158,
    SPR_RIVER_RAPIDS_25_DEG_UP_SE_NW = 21159,
    SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SW_NE = 21160,
    SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NW_SE = 21161,
    SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NE_SW = 21162,
    SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SE_NW = 21163,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_SW_NE = 21164,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_NW_SE = 21165,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_NE_SW = 21166,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_SE_NW = 21167,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_SW_NE = 21168,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_NW_SE = 21169,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_NE_SW = 21170,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_SE_NW = 21171,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_SW_NE = 21172,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_NW_SE = 21173,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_NE_SW = 21174,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_SE_NW = 21175,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_SW_NE = 21176,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_NW_SE = 21177,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_NE_SW = 21178,
    SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_SE_NW = 21179,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_SW_NE = 21180,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_NW_SE = 21181,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_NE_SW = 21182,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_SE_NW = 21183,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_SW_NE = 21184,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_NW_SE = 21185,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_NE_SW = 21186,
    SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_SE_NW = 21187,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_SW_SE = 21188,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_SE_NE = 21189,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_NE_NW = 21190,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_NW_SW = 21191,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_SE_SW = 21192,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_SW_NW = 21193,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_NW_NE = 21194,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_NE_SE = 21195,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_SW_SE = 21196,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_SE_NE = 21197,
    SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_LEFT_NW_NE = 21198,
    SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_NW_SW = 21199,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_SE_SW = 21200,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_SW_NW = 21201,
    SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_RIGHT_NW_NE = 21202,
    SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_NE_SE = 21203,
    SPR_RIVER_RAPIDS_WATERFALL_SW_NE = 21204,
    SPR_RIVER_RAPIDS_WATERFALL_NW_SE = 21205,
    SPR_RIVER_RAPIDS_WATERFALL_NE_SW = 21206,
    SPR_RIVER_RAPIDS_WATERFALL_SE_NW = 21207,
    SPR_RIVER_RAPIDS_WATERFALL_FRONT_SW_NE = 21208,
    SPR_RIVER_RAPIDS_WATERFALL_FRONT_NW_SE = 21209,
    SPR_RIVER_RAPIDS_WATERFALL_FRONT_NE_SW = 21210,
    SPR_RIVER_RAPIDS_WATERFALL_FRONT_SE_NW = 21211,
    SPR_RIVER_RAPIDS_WATERFALL_BASE_NW_FRAME_0 = 21212,

    SPR_RIVER_RAPIDS_WATERFALL_BASE_NE_FRAME_0 = 21220,

    SPR_RIVER_RAPIDS_WATERFALL_SIDE_SE_FRAME_0 = 21228,

    SPR_RIVER_RAPIDS_WATERFALL_SIDE_SW_FRAME_0 = 21236,

    SPR_RIVER_RAPIDS_WATERFALL_TOP_NW_FRAME_0 = 21244,

    SPR_RIVER_RAPIDS_WATERFALL_TOP_NE_FRAME_0 = 21252,

    SPR_RIVER_RAPIDS_RAPIDS_SW_NE_FRAME_0 = 21260,

    SPR_RIVER_RAPIDS_RAPIDS_FRONT_SW_NE = 21268,
    SPR_RIVER_RAPIDS_RAPIDS_NW_SE_FRAME_0 = 21269,

    SPR_RIVER_RAPIDS_RAPIDS_FRONT_NW_SE = 21277,
    SPR_RIVER_RAPIDS_RAPIDS_WHIRLPOOL_FRAME_0 = 21278,
};

static constexpr uint32_t river_rapids_track_pieces_25_deg_up[][2] = {
    { SPR_RIVER_RAPIDS_25_DEG_UP_SW_NE, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_25_DEG_UP_NW_SE, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_25_DEG_UP_NE_SW, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_25_DEG_UP_SE_NW, SPR_RIVER_RAPIDS_25_DEG_UP_FRONT_SE_NW },
};
static constexpr uint32_t river_rapids_track_pieces_flat_to_25_deg_up[][2] = {
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SW_NE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NW_SE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_NE_SW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_SE_NW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_UP_FRONT_SE_NW },
};

static constexpr uint32_t river_rapids_track_pieces_25_deg_up_to_flat[][2] = {
    { SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_SW_NE, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_NW_SE, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_NE_SW, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_SE_NW, SPR_RIVER_RAPIDS_25_DEG_UP_TO_FLAT_FRONT_SE_NW },
};

static constexpr uint32_t river_rapids_track_pieces_25_deg_down[][2] = {
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_SW_NE, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_NW_SE, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_NE_SW, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_SE_NW, SPR_RIVER_RAPIDS_25_DEG_DOWN_FRONT_SE_NW },
};

static constexpr uint32_t river_rapids_track_pieces_flat_to_25_deg_down[][2] = {
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_SW_NE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_NW_SE, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_NE_SW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_SE_NW, SPR_RIVER_RAPIDS_FLAT_TO_25_DEG_DOWN_FRONT_SE_NW },
};

static constexpr uint32_t river_rapids_track_pieces_25_deg_down_to_flat[][2] = {
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_SW_NE, SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_SW_NE },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_NW_SE, SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_NW_SE },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_NE_SW, SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_NE_SW },
    { SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_SE_NW, SPR_RIVER_RAPIDS_25_DEG_DOWN_TO_FLAT_FRONT_SE_NW },
};

/** rct2: 0x00757650 */
static void PaintRiverRapidsTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    if (direction & 1)
    {
        imageId = session.TrackColours.WithIndex((direction == 1 ? SPR_RIVER_RAPIDS_FLAT_NW_SE : SPR_RIVER_RAPIDS_FLAT_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 11 } });

        imageId = session.TrackColours.WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_FRONT_NW_SE : SPR_RIVER_RAPIDS_FLAT_FRONT_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 10, 10, height }, { 1, 2, 5 } });
    }
    else
    {
        imageId = session.TrackColours.WithIndex((direction == 0 ? SPR_RIVER_RAPIDS_FLAT_SW_NE : SPR_RIVER_RAPIDS_FLAT_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 11 } });

        imageId = session.TrackColours.WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_FRONT_SW_NE : SPR_RIVER_RAPIDS_FLAT_FRONT_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 10, 10, height }, { 1, 2, 5 } });
    }

    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x007576C0 */
static void PaintRiverRapidsStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintRiverRapidsTrackFlat(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilDrawNarrowStationPlatform(session, ride, direction, height, 12, trackElement);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void PaintRiverRapidsTrack25Deg(
    PaintSession& session, uint8_t direction, int32_t height, const uint32_t sprites[4][2], SupportType supportType)
{
    bool isStart = direction == 0 || direction == 3;

    ImageId imageId = session.TrackColours.WithIndex(sprites[direction][0]);
    PaintStruct* ps = PaintAddImageAsParentRotated(
        session, direction, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 4 } });
    if (!isStart)
    {
        session.WoodenSupportsPrependTo = ps;
    }

    imageId = session.TrackColours.WithIndex(sprites[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 27, height + 16 }, { 32, 1, 34 } });

    DrawSupportForSequenceA<TrackElemType::Up25>(session, supportType.wooden, 0, direction, height, session.SupportColours);

    int8_t tunnelHeightOffset = isStart ? -8 : 8;
    auto tunnelSubType = isStart ? TunnelSubType::SlopeStart : TunnelSubType::SlopeEnd;
    PaintUtilPushTunnelRotated(session, direction, height + tunnelHeightOffset, kTunnelGroup, tunnelSubType);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void PaintRiverRapidsTrack25DegToFlatA(
    PaintSession& session, uint8_t direction, int32_t height, const uint32_t sprites[4][2], SupportType supportType)
{
    ImageId imageId;
    PaintStruct* ps;

    bool isStart = direction == 0 || direction == 3;
    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 4 } });

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height + 16 }, { 32, 1, 18 } });
            break;

        case 1:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 4 } });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 16 }, { 1, 32, 18 } });
            break;

        case 2:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 4 } });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);

            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height }, { 32, 1, 18 } });
            break;

        case 3:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 4 } });

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 16 }, { 1, 32, 18 } });
            break;
    }
    DrawSupportForSequenceA<TrackElemType::Up25ToFlat>(
        session, supportType.wooden, 0, direction, height, session.SupportColours);

    int8_t heightOffset = isStart ? -8 : 8;
    auto tunnelSubType = isStart ? TunnelSubType::Flat : TunnelSubType::FlatTo25Deg;
    PaintUtilPushTunnelRotated(session, direction, height + heightOffset, kTunnelGroup, tunnelSubType);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

static void PaintRiverRapidsTrack25DegToFlatB(
    PaintSession& session, uint8_t direction, int32_t height, const uint32_t sprites[4][2], SupportType supportType)
{
    ImageId imageId;
    PaintStruct* ps;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 11 } });

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height + 16 }, { 32, 1, 26 } });
            break;

        case 1:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 11 } });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 16 }, { 1, 32, 26 } });
            break;

        case 2:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 11 } });
            session.WoodenSupportsPrependTo = ps;

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height + 16 }, { 32, 1, 26 } });
            break;

        case 3:
            imageId = session.TrackColours.WithIndex(sprites[direction][0]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 11 } });

            imageId = session.TrackColours.WithIndex(sprites[direction][1]);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 16 }, { 1, 32, 26 } });
            break;
    }
    DrawSupportForSequenceA<TrackElemType::FlatToUp25>(
        session, supportType.wooden, 0, direction, height, session.SupportColours);

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x00757660 */
static void PaintRiverRapidsTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintRiverRapidsTrack25Deg(session, direction, height, river_rapids_track_pieces_25_deg_up, supportType);
}

/** rct2: 0x00757670 */
static void PaintRiverRapidsTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintRiverRapidsTrack25DegToFlatB(session, direction, height, river_rapids_track_pieces_flat_to_25_deg_up, supportType);
}

/** rct2: 0x00757680 */
static void PaintRiverRapidsTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintRiverRapidsTrack25DegToFlatA(session, direction, height, river_rapids_track_pieces_25_deg_up_to_flat, supportType);
}

/** rct2: 0x00757690 */
static void PaintRiverRapidsTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintRiverRapidsTrack25Deg(session, (direction + 2) % 4, height, river_rapids_track_pieces_25_deg_down, supportType);
}

/** rct2: 0x007576A0 */
static void PaintRiverRapidsTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintRiverRapidsTrack25DegToFlatA(
        session, (direction + 2) % 4, height, river_rapids_track_pieces_flat_to_25_deg_down, supportType);
}

/** rct2: 0x007576B0 */
static void PaintRiverRapidsTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintRiverRapidsTrack25DegToFlatB(
        session, (direction + 2) % 4, height, river_rapids_track_pieces_25_deg_down_to_flat, supportType);
}

/** rct2: 0x007576F0 */
static void PaintRiverRapidsTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_SW_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 2, height }, { 28, 26, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_SW_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 27, height }, { 2, 1, 7 } });

            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;

        case 1:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 25, 25, 5 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_LEFT_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 1, height }, { 1, 30, 7 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_RIGHT_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 27, height }, { 30, 1, 7 } });
            break;

        case 2:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_NE_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 4, height }, { 26, 28, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_NE_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 10, 10, height }, { 1, 2, 5 } });

            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;

        case 3:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_SE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 4, height }, { 28, 28, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_LEFT_QUARTER_TURN_1_TILE_FRONT_SE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 28, height }, { 1, 1, 7 } });

            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00757700 */
static void PaintRiverRapidsTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_SW_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 4, height }, { 28, 28, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_SW_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 28, height + 2 }, { 1, 1, 7 } });

            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;

        case 1:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_SE_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 2, height }, { 28, 26, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_SE_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 27, height }, { 2, 1, 7 } });

            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;

        case 2:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_NE_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 28, 28, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_LEFT_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 1, height }, { 1, 22, 7 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_QUARTER_TURN_1_TILE_FRONT_RIGHT_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 27, height }, { 22, 1, 7 } });
            break;

        case 3:
            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_NW_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 4, height }, { 26, 28, 11 } });

            imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RIGHT_QUARTER_TURN_1_TILE_FRONT_NW_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 2, height }, { 1, 28, 7 } });

            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00757710 */
static void PaintRiverRapidsTrackWaterfall(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    uint16_t frameNum = (GetGameState().CurrentTicks / 2) & 7;

    if (direction & 1)
    {
        imageId = session.TrackColours.WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_WATERFALL_NW_SE : SPR_RIVER_RAPIDS_WATERFALL_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 11 } });

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_WATERFALL_BASE_NE_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 11 } });

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_WATERFALL_TOP_NE_FRAME_0 + frameNum));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height + 17 }, { 1, 32, 27 } });

        imageId = session.TrackColours.WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_WATERFALL_FRONT_NW_SE : SPR_RIVER_RAPIDS_WATERFALL_FRONT_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 17 }, { 1, 32, 27 } });

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_WATERFALL_SIDE_SW_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 27, 0, height + 17 }, { 1, 32, 27 } });
    }
    else
    {
        imageId = session.TrackColours.WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_WATERFALL_SW_NE : SPR_RIVER_RAPIDS_WATERFALL_NE_SW));

        auto bb = BoundBoxXYZ{ { 0, 4, height }, { 32, 24, 11 } };
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_WATERFALL_BASE_NW_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, bb);

        bb.offset = { 0, 4, height + 17 };
        bb.length = { 32, 1, 27 };

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_WATERFALL_TOP_NW_FRAME_0 + frameNum));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

        bb.offset = { 0, 27, height + 17 };

        imageId = session.TrackColours.WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_WATERFALL_FRONT_SW_NE : SPR_RIVER_RAPIDS_WATERFALL_FRONT_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_WATERFALL_SIDE_SE_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, bb);
    }

    DrawSupportForSequenceA<TrackElemType::Waterfall>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Tall);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Tall);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x00757720 */
static void PaintRiverRapidsTrackRapids(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    uint16_t frameNum = (GetGameState().CurrentTicks / 2) & 7;

    if (direction & 1)
    {
        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_RAPIDS_NW_SE_FRAME_0 + frameNum));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 4, 0, height }, { 24, 32, 11 } });

        imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RAPIDS_FRONT_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 27, 0, height + 17 }, { 1, 32, 11 } });
    }
    else
    {
        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_RAPIDS_SW_NE_FRAME_0 + frameNum));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 4, height }, { 32, 24, 11 } });

        imageId = session.TrackColours.WithIndex(SPR_RIVER_RAPIDS_RAPIDS_FRONT_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 27, height + 17 }, { 32, 1, 11 } });
    }

    DrawSupportForSequenceA<TrackElemType::Rapids>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x00757740 */
static void PaintRiverRapidsTrackOnRidePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintRiverRapidsTrackFlat(session, ride, trackSequence, direction, height, trackElement, supportType);
    TrackPaintUtilOnridePhotoPaint(session, direction, height + 13, trackElement);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x */
static void PaintRiverRapidsTrackWhirlpool(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    uint8_t frameNum = (GetGameState().CurrentTicks / 4) % 16;

    if (direction & 1)
    {
        imageId = session.TrackColours.WithIndex((direction == 1 ? SPR_RIVER_RAPIDS_FLAT_NW_SE : SPR_RIVER_RAPIDS_FLAT_SE_NW));

        auto bb = BoundBoxXYZ{ { 4, 0, height }, { 24, 32, 11 } };
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_RAPIDS_WHIRLPOOL_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, bb);

        bb.offset = { 10, 10, height };
        bb.length = { 1, 2, 5 };

        imageId = session.TrackColours.WithIndex(
            (direction == 1 ? SPR_RIVER_RAPIDS_FLAT_FRONT_NW_SE : SPR_RIVER_RAPIDS_FLAT_FRONT_SE_NW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);
    }
    else
    {
        imageId = session.TrackColours.WithIndex((direction == 0 ? SPR_RIVER_RAPIDS_FLAT_SW_NE : SPR_RIVER_RAPIDS_FLAT_NE_SW));

        auto bb = BoundBoxXYZ{ { 0, 4, height }, { 32, 24, 11 } };
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

        imageId = session.TrackColours.WithIndex((SPR_RIVER_RAPIDS_RAPIDS_WHIRLPOOL_FRAME_0 + frameNum));
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, bb);

        bb.offset = { 10, 10, height };
        bb.length = { 1, 2, 5 };

        imageId = session.TrackColours.WithIndex(
            (direction == 0 ? SPR_RIVER_RAPIDS_FLAT_FRONT_SW_NE : SPR_RIVER_RAPIDS_FLAT_FRONT_NE_SW));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);
    }

    DrawSupportForSequenceA<TrackElemType::Whirlpool>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x0075745C
 **/
TrackPaintFunction GetTrackPaintFunctionRiverRapids(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintRiverRapidsTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintRiverRapidsStation;

        case TrackElemType::Up25:
            return PaintRiverRapidsTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintRiverRapidsTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintRiverRapidsTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintRiverRapidsTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintRiverRapidsTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintRiverRapidsTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintRiverRapidsTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintRiverRapidsTrackRightQuarterTurn1Tile;

        case TrackElemType::Waterfall:
            return PaintRiverRapidsTrackWaterfall;

        case TrackElemType::Rapids:
            return PaintRiverRapidsTrackRapids;

        case TrackElemType::OnRidePhoto:
            return PaintRiverRapidsTrackOnRidePhoto;

        case TrackElemType::Whirlpool:
            return PaintRiverRapidsTrackWhirlpool;
        default:
            return TrackPaintFunctionDummy;
    }
}
