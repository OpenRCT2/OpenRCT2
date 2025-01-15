/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../ride/Ride.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../sprites.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
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
    SPR_GO_KARTS_FLAT_SW_NE = 20752,
    SPR_GO_KARTS_FLAT_NW_SE = 20753,
    SPR_GO_KARTS_FLAT_FRONT_SW_NE = 20754,
    SPR_GO_KARTS_FLAT_FRONT_NW_SE = 20755,
    SPR_GO_KARTS_STARTING_GRID_END_SW_NE = 20756,
    SPR_GO_KARTS_STARTING_GRID_END_NW_SE = 20757,
    SPR_GO_KARTS_STARTING_GRID_END_NE_SW = 20758,
    SPR_GO_KARTS_STARTING_GRID_END_SE_NW = 20759,
    SPR_GO_KARTS_STARTING_GRID_END_FRONT_SW_NE = 20760,
    SPR_GO_KARTS_STARTING_GRID_END_FRONT_NW_SE = 20761,
    SPR_GO_KARTS_STARTING_GRID_END_FRONT_NE_SW = 20762,
    SPR_GO_KARTS_STARTING_GRID_END_FRONT_SE_NW = 20763,
    SPR_GO_KARTS_STARTING_GRID_SW_NE = 20764,
    SPR_GO_KARTS_STARTING_GRID_NW_SE = 20765,
    SPR_GO_KARTS_STARTING_GRID_NE_SW = 20766,
    SPR_GO_KARTS_STARTING_GRID_SE_NW = 20767,
    SPR_GO_KARTS_STARTING_GRID_FRONT_SW_NE = 20768,
    SPR_GO_KARTS_STARTING_GRID_FRONT_NW_SE = 20769,
    SPR_GO_KARTS_STARTING_GRID_FRONT_NE_SW = 20770,
    SPR_GO_KARTS_STARTING_GRID_FRONT_SE_NW = 20771,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SW_NE = 20772,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NW_SE = 20773,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NE_SW = 20774,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SE_NW = 20775,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SW_NE = 20776,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NW_SE = 20777,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NE_SW = 20778,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SE_NW = 20779,
    SPR_GO_KARTS_25_DEG_UP_SW_NE = 20780,
    SPR_GO_KARTS_25_DEG_UP_NW_SE = 20781,
    SPR_GO_KARTS_25_DEG_UP_NE_SW = 20782,
    SPR_GO_KARTS_25_DEG_UP_SE_NW = 20783,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SW_NE = 20784,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NW_SE = 20785,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NE_SW = 20786,
    SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SE_NW = 20787,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SW_NE = 20788,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NW_SE = 20789,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NE_SW = 20790,
    SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SE_NW = 20791,
    SPR_GO_KARTS_25_DEG_UP_FRONT_SW_NE = 20792,
    SPR_GO_KARTS_25_DEG_UP_FRONT_NW_SE = 20793,
    SPR_GO_KARTS_25_DEG_UP_FRONT_NE_SW = 20794,
    SPR_GO_KARTS_25_DEG_UP_FRONT_SE_NW = 20795,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_SW_NW = 20796,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_NW_NE = 20797,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_NE_SE = 20798,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_SE_SW = 20799,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_SW_NW = 20800,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_NW_NE = 20801,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_NE_SE = 20802,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_SE_SW = 20803,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_SW_NW = 20804, // Empty
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_NW_NE = 20805,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_NE_SE = 20806,
    SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_SE_SW = 20807,
    SPR_GO_KARTS_START_POLE_RED_SW_NE = 20808,
    SPR_GO_KARTS_START_POLE_GREEN_SW_NE = 20809,
    SPR_GO_KARTS_START_POLE_NW_SE = 20810,
    SPR_GO_KARTS_START_POLE_NE_SW = 20811,
    SPR_GO_KARTS_START_POLE_RED_SE_NW = 20812,
    SPR_GO_KARTS_START_POLE_GREEN_SE_NW = 20813,
    SPR_GO_KARTS_START_LIGHTS_RED_SW_NE = 20814,
    SPR_GO_KARTS_START_LIGHTS_GREEN_SW_NE = 20815,
    SPR_GO_KARTS_START_LIGHTS_NW_SE = 20816,
    SPR_GO_KARTS_START_LIGHTS_NE_SW = 20817,
    SPR_GO_KARTS_START_LIGHTS_RED_SE_NW = 20818,
    SPR_GO_KARTS_START_LIGHTS_GREEN_SE_NW = 20819,
};

static constexpr uint32_t go_karts_track_pieces_starting_grid_end[4][2] = {
    { SPR_GO_KARTS_STARTING_GRID_END_SW_NE, SPR_GO_KARTS_STARTING_GRID_END_FRONT_SW_NE },
    { SPR_GO_KARTS_STARTING_GRID_END_NW_SE, SPR_GO_KARTS_STARTING_GRID_END_FRONT_NW_SE },
    { SPR_GO_KARTS_STARTING_GRID_END_NE_SW, SPR_GO_KARTS_STARTING_GRID_END_FRONT_NE_SW },
    { SPR_GO_KARTS_STARTING_GRID_END_SE_NW, SPR_GO_KARTS_STARTING_GRID_END_FRONT_SE_NW },
};

static constexpr uint32_t go_karts_track_pieces_starting_grid[4][2] = {
    { SPR_GO_KARTS_STARTING_GRID_SW_NE, SPR_GO_KARTS_STARTING_GRID_FRONT_SW_NE },
    { SPR_GO_KARTS_STARTING_GRID_NW_SE, SPR_GO_KARTS_STARTING_GRID_FRONT_NW_SE },
    { SPR_GO_KARTS_STARTING_GRID_NE_SW, SPR_GO_KARTS_STARTING_GRID_FRONT_NE_SW },
    { SPR_GO_KARTS_STARTING_GRID_SE_NW, SPR_GO_KARTS_STARTING_GRID_FRONT_SE_NW },
};

static constexpr uint32_t go_karts_track_pieces_25_deg_up[4][2] = {
    { SPR_GO_KARTS_25_DEG_UP_SW_NE, SPR_GO_KARTS_25_DEG_UP_FRONT_SW_NE },
    { SPR_GO_KARTS_25_DEG_UP_NW_SE, SPR_GO_KARTS_25_DEG_UP_FRONT_NW_SE },
    { SPR_GO_KARTS_25_DEG_UP_NE_SW, SPR_GO_KARTS_25_DEG_UP_FRONT_NE_SW },
    { SPR_GO_KARTS_25_DEG_UP_SE_NW, SPR_GO_KARTS_25_DEG_UP_FRONT_SE_NW },
};

static constexpr uint32_t go_karts_track_pieces_flat_to_25_deg_up[4][2] = {
    { SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SW_NE, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SW_NE },
    { SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NW_SE, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NW_SE },
    { SPR_GO_KARTS_FLAT_TO_25_DEG_UP_NE_SW, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_NE_SW },
    { SPR_GO_KARTS_FLAT_TO_25_DEG_UP_SE_NW, SPR_GO_KARTS_FLAT_TO_25_DEG_UP_FRONT_SE_NW },
};

static constexpr uint32_t go_karts_track_pieces_25_deg_up_to_flat[4][2] = {
    { SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SW_NE, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SW_NE },
    { SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NW_SE, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NW_SE },
    { SPR_GO_KARTS_25_DEG_UP_TO_FLAT_NE_SW, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_NE_SW },
    { SPR_GO_KARTS_25_DEG_UP_TO_FLAT_SE_NW, SPR_GO_KARTS_25_DEG_UP_TO_FLAT_FRONT_SE_NW },
};

/** rct2: 0x0074A748 */
static void PaintGoKartsTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    if (direction == 0 || direction == 2)
    {
        imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 28, 1 } });

        imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_FRONT_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 2 }, { 32, 1, 3 } });

        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 28, 32, 1 } });

        imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_FRONT_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 0, height + 2 }, { 1, 32, 3 } });

        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0074A758 */
static void PaintGoKartsTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    PaintStruct* ps;

    imageId = session.TrackColours.WithIndex(go_karts_track_pieces_25_deg_up[direction][0]);
    if (direction == 0 || direction == 2)
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 28, 1 } });
    }
    else
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 28, 32, 1 } });
    }

    imageId = session.TrackColours.WithIndex(go_karts_track_pieces_25_deg_up[direction][1]);
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 2 }, { 32, 1, 11 } });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 0, height + 2 }, { 1, 32, 11 } });
    }

    session.WoodenSupportsPrependTo = ps;

    DrawSupportForSequenceA<TrackElemType::Up25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0074A768 */
static void PaintGoKartsTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    PaintStruct* ps;

    imageId = session.TrackColours.WithIndex(go_karts_track_pieces_flat_to_25_deg_up[direction][0]);
    if (direction == 0 || direction == 2)
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 28, 1 } });
    }
    else
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 28, 32, 1 } });
    }

    imageId = session.TrackColours.WithIndex(go_karts_track_pieces_flat_to_25_deg_up[direction][1]);
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 2 }, { 32, 1, 11 } });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 0, height + 2 }, { 1, 32, 11 } });
    }

    session.WoodenSupportsPrependTo = ps;

    DrawSupportForSequenceA<TrackElemType::FlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x */
static void PaintGoKartsTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    PaintStruct* ps;

    imageId = session.TrackColours.WithIndex(go_karts_track_pieces_25_deg_up_to_flat[direction][0]);
    if (direction == 0 || direction == 2)
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 28, 1 } });
    }
    else
    {
        ps = PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 28, 32, 1 } });
    }

    imageId = session.TrackColours.WithIndex(go_karts_track_pieces_25_deg_up_to_flat[direction][1]);
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 2 }, { 32, 1, 11 } });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 0, height + 2 }, { 1, 32, 11 } });
    }

    session.WoodenSupportsPrependTo = ps;

    DrawSupportForSequenceA<TrackElemType::Up25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    switch (direction)
    {
        case 0:

            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x0074A788 */
static void PaintGoKartsTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGoKartsTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0074A798 */
static void PaintGoKartsTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGoKartsTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0074A7A8 */
static void PaintGoKartsTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGoKartsTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x */
static void PaintGoKartsStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const auto* stationObj = ride.GetStationObject();

    bool hasFence;
    ImageId imageId;
    const uint32_t(*sprites)[2] = go_karts_track_pieces_starting_grid;

    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        sprites = go_karts_track_pieces_starting_grid_end;
    }

    imageId = session.TrackColours.WithIndex(sprites[direction][0]);
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 28, 1 } });
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 28, 32, 1 } });
    }

    auto stationColour = GetStationColourScheme(session, trackElement);
    if (direction == 0 || direction == 2)
    {
        hasFence = TrackPaintUtilHasFence(EDGE_NW, session.MapPosition, trackElement, ride, session.CurrentRotation);
        TrackPaintUtilDrawStationCovers(session, EDGE_NW, hasFence, stationObj, height, stationColour);
    }
    else
    {
        hasFence = TrackPaintUtilHasFence(EDGE_NE, session.MapPosition, trackElement, ride, session.CurrentRotation);
        TrackPaintUtilDrawStationCovers(session, EDGE_NE, hasFence, stationObj, height, stationColour);
    }

    imageId = session.TrackColours.WithIndex(sprites[direction][1]);
    if (direction == 0 || direction == 2)
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 29, height + 2 }, { 32, 1, 3 } });

        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 0, height + 2 }, { 1, 32, 3 } });

        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    if (direction == 0 || direction == 2)
    {
        hasFence = TrackPaintUtilHasFence(EDGE_SE, session.MapPosition, trackElement, ride, session.CurrentRotation);
        TrackPaintUtilDrawStationCovers(session, EDGE_SE, hasFence, stationObj, height, stationColour);
    }
    else
    {
        hasFence = TrackPaintUtilHasFence(EDGE_SW, session.MapPosition, trackElement, ride, session.CurrentRotation);
        TrackPaintUtilDrawStationCovers(session, EDGE_SW, hasFence, stationObj, height, stationColour);
    }

    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        const bool hasGreenLight = trackElement.HasGreenLight();

        switch (direction)
        {
            case 0:
                imageId = session.TrackColours.WithIndex(
                    (hasGreenLight ? SPR_GO_KARTS_START_POLE_GREEN_SW_NE : SPR_GO_KARTS_START_POLE_RED_SW_NE));
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 1, height + 4 }, { 3, 3, 13 } });

                imageId = session.TrackColours.WithIndex(
                    (hasGreenLight ? SPR_GO_KARTS_START_LIGHTS_GREEN_SW_NE : SPR_GO_KARTS_START_LIGHTS_RED_SW_NE));
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 28, height + 4 }, { 3, 3, 13 } });
                break;
            case 1:
                imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_START_POLE_NW_SE);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 28, height + 4 }, { 3, 3, 13 } });

                imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_START_LIGHTS_NW_SE);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 28, height + 4 }, { 3, 3, 13 } });
                break;
            case 2:
                imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_START_POLE_NE_SW);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 1, height + 4 }, { 3, 3, 13 } });

                imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_START_LIGHTS_NE_SW);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 28, height + 4 }, { 3, 3, 13 } });
                break;
            case 3:
                imageId = session.TrackColours.WithIndex(
                    (hasGreenLight ? SPR_GO_KARTS_START_POLE_GREEN_SE_NW : SPR_GO_KARTS_START_POLE_RED_SE_NW));
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 1, 1, height + 4 }, { 3, 3, 13 } });

                imageId = session.TrackColours.WithIndex(
                    (hasGreenLight ? SPR_GO_KARTS_START_LIGHTS_GREEN_SE_NW : SPR_GO_KARTS_START_LIGHTS_RED_SE_NW));
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 28, 1, height + 4 }, { 3, 3, 13 } });
                break;
        }
    }

    DrawSupportForSequenceA<TrackElemType::EndStation>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0074A7E8 */
static void PaintGoKartsTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    switch (direction)
    {
        case 0:
            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 2, height }, { 32, 28, 1 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 2, height + 2 }, { 1, 1, 3 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_NW_NE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 14, 29, height + 2 }, { 16, 1, 3 } });
            break;
        case 1:
            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_NE_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 30, 30, 1 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_NE_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 29, height + 2 }, { 16, 1, 3 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_NE_SE);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 2, height + 2 }, { 1, 16, 3 } });
            break;
        case 2:
            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_SE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 0, height }, { 28, 32, 1 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_SE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 2, 2, height + 2 }, { 1, 1, 3 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_B_SE_SW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 14, height + 2 }, { 1, 16, 3 } });
            break;
        case 3:
            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_SW_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 8, 8, height }, { 24, 24, 1 } });

            imageId = session.TrackColours.WithIndex(SPR_GO_KARTS_FLAT_QUARTER_TURN_1_TILE_EDGE_A_SW_NW);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 29, 29, height + 2 }, { 1, 1, 3 } });

            // The empty sprite isn't drawn
            break;
    }

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 2:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 3:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0074A7F8 */
static void PaintGoKartsTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintGoKartsTrackLeftQuarterTurn1Tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

static void TrackUp60(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 16), { 0, 0, height },
                { { 0, 1, height }, { 32, 1, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 17), { 0, 0, height },
                { { 0, 0, height + 86 }, { 32, 32, 1 } });
            break;
        case 1:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 18), { 0, 0, height },
                { { 28, 4, height }, { 2, 24, 77 } });
            break;
        case 2:
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 19), { 0, 0, height },
                { { 28, 4, height }, { 2, 24, 77 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 20), { 0, 0, height },
                { { 0, 1, height }, { 32, 1, 1 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 21), { 0, 0, height },
                { { 0, 0, height + 86 }, { 32, 32, 1 } });
            break;
    }
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
        WoodenSupportTransitionType::Up60Deg);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

static void TrackUp25ToUp60(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 0), { 0, 0, height },
                { { 0, 0, height }, { 32, 32, 0 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 1), { 0, 0, height },
                { { 0, 0, height + 54 }, { 32, 32, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 2), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 3), { 0, 0, height },
                { { 28, 4, height }, { 2, 24, 56 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 4), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 5), { 0, 0, height },
                { { 28, 4, height }, { 2, 24, 56 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 6), { 0, 0, height },
                { { 0, 0, height }, { 32, 32, 0 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 7), { 0, 0, height },
                { { 0, 0, height + 54 }, { 32, 32, 1 } });
            break;
    }
    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
        WoodenSupportTransitionType::Up25DegToUp60Deg);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackUp60ToUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (direction)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 8), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 9), { 0, 0, height },
                { { 0, 6, height + 54 }, { 32, 20, 1 } });
            break;
        case 1:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 10), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 11), { 0, 0, height },
                { { 28, 4, height }, { 2, 24, 56 } });
            break;
        case 2:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 12), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 13), { 0, 0, height },
                { { 28, 4, height }, { 2, 24, 56 } });
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 14), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_STEEP + 15), { 0, 0, height },
                { { 0, 6, height + 54 }, { 32, 20, 1 } });
            break;
    }
    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
        WoodenSupportTransitionType::Up60DegToUp25Deg);
    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackDown60(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackUp60(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDown25ToDown60(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackUp60ToUp25(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDown60ToDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackUp25ToUp60(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackFlatToUp60LongBase(
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
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 0),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 1),
                        { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 8),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 9),
                        { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 16),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 17),
                        { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 24),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 25),
                        { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq0);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 2),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 3),
                        { 0, 0, height }, { { 0, 6, height + 38 }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 10),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 11),
                        { 0, 0, height }, { { 0, 6, height + 38 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 18),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 19),
                        { 0, 0, height }, { { 0, 6, height + 38 }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 26),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 27),
                        { 0, 0, height }, { { 0, 6, height + 38 }, { 32, 20, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq1);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 48);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 4),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 5),
                        { 0, 0, height }, { { 0, 6, height + 46 }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 13),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 12),
                        { 0, 0, height }, { { 0, 6, height + 46 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 21),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 20),
                        { 0, 0, height }, { { 0, 6, height + 46 }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 28),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 29),
                        { 0, 0, height }, { { 0, 6, height + 46 }, { 32, 20, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq2);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 6),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 7),
                        { 0, 0, height }, { { 0, 6, height + 70 }, { 32, 20, 1 } });
                    break;
                case 1:
                    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 14),
                        { 0, 0, height }, { { 28, 4, height - 16 }, { 2, 24, 56 } });
                    break;
                case 2:
                    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 22),
                        { 0, 0, height }, { { 28, 4, height - 16 }, { 2, 24, 56 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 30),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 31),
                        { 0, 0, height }, { { 0, 6, height + 70 }, { 32, 20, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::FlatToUp60DegLongBaseSeq3);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void TrackUp60ToFlatLongBase(
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
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 32),
                        { 0, 0, height }, { { 0, 1, height }, { 32, 1, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 33),
                        { 0, 0, height }, { { 0, 6, height + 70 }, { 32, 20, 1 } });
                    break;
                case 1:
                    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 40),
                        { 0, 0, height }, { { 28, 4, height }, { 2, 24, 70 } });
                    break;
                case 2:
                    session.WoodenSupportsPrependTo = PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 48),
                        { 0, 0, height }, { { 28, 4, height }, { 2, 24, 70 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 56),
                        { 0, 0, height }, { { 0, 1, height }, { 32, 1, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 57),
                        { 0, 0, height }, { { 0, 6, height + 70 }, { 32, 20, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq0);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 34),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 35),
                        { 0, 0, height }, { { 0, 6, height + 46 }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 42),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 43),
                        { 0, 0, height }, { { 0, 6, height + 46 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 50),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 51),
                        { 0, 0, height }, { { 0, 6, height + 46 }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 58),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 59),
                        { 0, 0, height }, { { 0, 6, height + 46 }, { 32, 20, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq1);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 36),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 37),
                        { 0, 0, height }, { { 0, 6, height + 38 }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 44),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 45),
                        { 0, 0, height }, { { 0, 6, height + 38 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 52),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 53),
                        { 0, 0, height }, { { 0, 6, height + 38 }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 60),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 61),
                        { 0, 0, height }, { { 0, 6, height + 38 }, { 32, 20, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq2);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 38),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 39),
                        { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 46),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 47),
                        { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 54),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 55),
                        { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 62),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 63),
                        { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up60DegToFlatLongBaseSeq3);
            switch (direction)
            {
                case 1:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 2:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 40);
            break;
    }
}

static void TrackFlatToDown60LongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackUp60ToFlatLongBase(session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDown60ToFlatLongBase(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    TrackFlatToUp60LongBase(session, ride, 3 - trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 0)),
                        { 0, 0, height }, { { 0, 0, height + 1 }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 1)),
                        { 0, 0, height }, { { 0, 0, height + 24 }, { 32, 32, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 8)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 9)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 20, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 16)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 17)),
                        { 0, 0, height }, { { 0, 0, height + 24 }, { 32, 32, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 24)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 25)),
                        { 0, 0, height }, { { 0, 0, height + 24 }, { 32, 32, 0 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 2)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 3)),
                        { 0, 0, height }, { { 7, 3, height + 2 }, { 1, 1, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 10)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 11)),
                        { 0, 0, height }, { { 16, 16, height + 24 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 18)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 19)),
                        { 0, 0, height }, { { 3, 7, height + 1 }, { 1, 1, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 26)),
                        { 0, 0, height }, { { 0, 0, height }, { 8, 8, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 27)),
                        { 0, 0, height }, { { 7, 7, height + 1 }, { 1, 1, 1 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 4)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 5)),
                        { 0, 0, height }, { { 16, 0, height + 24 }, { 16, 16, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 12)),
                        { 0, 0, height }, { { 0, 0, height + 1 }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 13)),
                        { 0, 0, height }, { { 15, 15, height + 2 }, { 1, 1, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 20)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 21)),
                        { 0, 0, height }, { { 0, 16, height + 24 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 28)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 29)),
                        { 0, 0, height }, { { 16, 16, height + 24 }, { 16, 16, 0 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 6)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 7)),
                        { 0, 0, height }, { { 0, 0, height + 24 }, { 32, 32, 0 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 14)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 15)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 20, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 22)),
                        { 0, 0, height }, { { 0, 0, height + 1 }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 23)),
                        { 0, 0, height }, { { 0, 0, height + 24 }, { 32, 32, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 30)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 31)),
                        { 0, 0, height }, { { 0, 0, height + 24 }, { 32, 32, 0 } });
                    break;
            }
            break;
    }

    TrackPaintUtilLeftQuarterTurn3TilesTunnel(session, kTunnelGroup, TunnelSubType::Flat, height, direction, trackSequence);

    static constexpr int blockedSegments[4] = {
        kSegmentsAll,
        kSegmentsAll,
        EnumsToFlags(PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomLeftSide),
        kSegmentsAll,
    };

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftQuarterTurn3Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    TrackLeftQuarterTurn3Tiles(session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

static void TrackLeftQuarterTurn5Tiles(
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
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 0)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 1)),
                        { 0, 0, height }, { { 30, 30, height + 1 }, { 1, 1, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 13)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 14)),
                        { 0, 0, height }, { { 0, 31, height + 1 }, { 32, 1, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 26)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 27)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 39)),
                        { 0, 0, height }, { { 0, 4, height }, { 32, 28, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 40)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 2)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 15)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 28)),
                        { 0, 0, height }, { { 16, 0, height + 1 }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 29)),
                        { 0, 0, height }, { { 31, 7, height + 2 }, { 1, 1, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 41)),
                        { 0, 0, height }, { { 0, 0, height + 1 }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 42)),
                        { 0, 0, height }, { { 16, 0, height + 2 }, { 1, 1, 1 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 3)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 4)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 16)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 17)),
                        { 0, 0, height }, { { 15, 31, height + 1 }, { 1, 1, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 30)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 31)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 16, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 43)),
                        { 0, 0, height }, { { 0, 16, height + 2 }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 44)),
                        { 0, 0, height }, { { 0, 16, height + 23 }, { 32, 16, 1 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 5)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 6)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 16, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 18)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 19)),
                        { 0, 0, height }, { { 16, 16, height + 23 }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 20)),
                        { 0, 0, height }, { { 32, 32, height }, { 1, 1, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 32)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 33)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 45)),
                        { 0, 0, height }, { { 0, 0, height + 1 }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 46)),
                        { 0, 0, height }, { { 16, 16, height + 2 }, { 1, 1, 1 } });
                    break;
            }
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 7)),
                        { 0, 0, height }, { { 0, 16, height + 1 }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 8)),
                        { 0, 0, height }, { { 7, 31, height + 2 }, { 1, 1, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 21)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 34)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 47)),
                        { 0, 0, height }, { { 0, 0, height + 1 }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 48)),
                        { 0, 0, height }, { { 0, 16, height + 2 }, { 1, 1, 1 } });
                    break;
            }
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 9)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 10)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 22)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 23)),
                        { 0, 0, height }, { { 31, 15, height + 1 }, { 1, 1, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 35)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 36)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 16, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 49)),
                        { 0, 0, height }, { { 16, 0, height + 2 }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 50)),
                        { 0, 0, height }, { { 16, 0, height + 23 }, { 16, 32, 1 } });
                    break;
            }
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 11)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 12)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 24)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 25)),
                        { 0, 0, height }, { { 31, 0, height + 1 }, { 1, 32, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 37)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 38)),
                        { 0, 0, height }, { { 30, 30, height + 1 }, { 1, 1, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 51)),
                        { 0, 0, height }, { { 4, 0, height }, { 28, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 52)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
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
            break;
    }

    static constexpr int blockedSegments[7] = {
        EnumsToFlags(PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
        kSegmentsAll,
        EnumsToFlags(
            PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
            PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
        EnumsToFlags(
            PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
        kSegmentsAll,
        EnumsToFlags(
            PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
            PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
        EnumsToFlags(
            PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::bottomRightSide),
    };

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftQuarterTurn5Tiles>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackRightQuarterTurn5Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    TrackLeftQuarterTurn5Tiles(session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

static void TrackLeftEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 0)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 1)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 10)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 11)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 20)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 21)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 22)),
                        { 0, 32, height }, { { 1, 33, height + 1 }, { 1, 1, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 30)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 31)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 2)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 3)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 12)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 13)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 23)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 24)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 32)),
                        { 32, 32, height }, { { 32, 32, height + 1 }, { 1, 1, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 33)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 34)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 4)),
                        { 0, 0, height }, { { -16, 16, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 5)),
                        { 0, 0, height }, { { -16, 16, height + 23 }, { 32, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 14)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 15)),
                        { 0, 0, height }, { { 16, 16, height + 23 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 25)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 26)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 35)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 36)),
                        { 0, 0, height }, { { 15, 15, height }, { 1, 1, 1 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 6)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 7)),
                        { 0, 0, height }, { { 16, 0, height + 23 }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 16)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 17)),
                        { 0, 0, height }, { { 8, 8, height + 1 }, { 1, 1, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 27)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 28)),
                        { 0, 0, height }, { { 0, 16, height + 23 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 37)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 38)),
                        { 0, 0, height }, { { 16, 16, height + 23 }, { 16, 16, 1 } });
                    break;
            }
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 8)),
                        { 0, 0, height }, { { 0, 0, height + 1 }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 9)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 18)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 19)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 29)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 39)),
                        { 0, 0, height }, { { 1, 0, height }, { 1, 1, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 40)),
                        { 0, 0, height }, { { 31, 30, height }, { 1, 1, 1 } });
                    break;
            }
            break;
    }

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::LeftEighthBankToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackRightEighthToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 41)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 42)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 52)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 53)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 54)),
                        { 0, 32, height }, { { 1, 33, height + 1 }, { 1, 1, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 62)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 63)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 72)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 73)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 43)),
                        { 32, 32, height }, { { 32, 32, height + 1 }, { 1, 1, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 44)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 45)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 55)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 56)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 64)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 65)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 74)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 75)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 46)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 47)),
                        { 0, 0, height }, { { 15, 15, height }, { 1, 1, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 57)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 58)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 66)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 67)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 76)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 77)),
                        { 0, 0, height }, { { 0, 16, height + 23 }, { 32, 16, 1 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 48)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 49)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 59)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 60)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 68)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 69)),
                        { 0, 0, height }, { { 16, 16, height + 1 }, { 1, 1, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 78)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 79)),
                        { 0, 0, height }, { { 16, 0, height + 23 }, { 16, 16, 1 } });
                    break;
            }
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 50)),
                        { 0, 0, height }, { { 1, 0, height }, { 1, 1, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 51)),
                        { 0, 0, height }, { { 31, 30, height }, { 1, 1, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 61)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 70)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 71)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 80)),
                        { 0, 0, height }, { { 0, 0, height + 1 }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 81)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
            }
            break;
    }

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::RightEighthBankToDiag>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackLeftEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    TrackRightEighthToDiag(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackRightEighthToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    TrackLeftEighthToDiag(session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

static void TrackDiagFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 0)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 1)),
                        { 0, 0, height }, { { 0, 0, height + 24 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 7)),
                        { 0, 0, height }, { { 0, 0, height + 1 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 4)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 5)),
                        { 0, 0, height }, { { 0, 0, height + 30 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 6)),
                        { 0, 0, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 24 }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 24 }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 4)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 5)),
                        { 0, 0, height }, { { 0, 0, height + 30 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 6)),
                        { 0, 0, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 0)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 1)),
                        { 0, 0, height }, { { 0, 0, height + 24 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 7)),
                        { 0, 0, height }, { { 0, 0, height + 1 }, { 32, 32, 1 } });
                    break;
            }
            break;
    }

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagFlat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackDiagFlatToUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 0)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 1)),
                        { 0, 0, height }, { { 0, 0, height + 30 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 6)),
                        { 0, 0, height }, { { 0, 0, height + 1 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 8)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 9)),
                        { 0, 0, height }, { { 0, 0, height + 30 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 14)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 10)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 11)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 4)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 5)),
                        { 0, 0, height }, { { 0, 0, height + 30 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 7)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 12)), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 13)), { 0, 0, height },
                        { { 0, 0, height + 30 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 15)), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 1 } });
                    break;
            }
            break;
    }

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::DiagFlatToUp25>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void TrackDiagUp25ToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 0)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 1)),
                        { 0, 0, height }, { { 0, 0, height + 30 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 6)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 8)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 9)),
                        { 0, 0, height }, { { 0, 0, height + 30 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 14)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 10)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 11)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 4)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 5)),
                        { 0, 0, height }, { { 0, 0, height + 30 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 7)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 12)), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 13)), { 0, 0, height },
                        { { 0, 0, height + 30 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 15)), { 0, 0, height },
                        { { 0, 0, height + 9 }, { 32, 32, 1 } });
                    break;
            }
            break;
    }

    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp25ToFlat>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void TrackDiagUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 0)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 1)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 6)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 8)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 9)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 14)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 10)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 11)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 4)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 5)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 7)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 12)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 13)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            break;
    }

    DrawSupportForSequenceB<OpenRCT2::TrackElemType::DiagUp25>(
        session, supportType.wooden, trackSequence, direction, height + 16, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void TrackDiagFlatToDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return TrackDiagUp25ToFlat(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDiagDown25ToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return TrackDiagFlatToUp25(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDiagDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return TrackDiagUp25(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDiagUp25ToUp60(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 0)), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 1)), { 0, 0, height },
                        { { 0, 0, height + 54 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 6)), { 0, 0, height },
                        { { 0, 0, height + 1 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 8)), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 9)), { 0, 0, height },
                        { { 0, 0, height + 54 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 14)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 54 }, { 32, 32, 1 } });
                    break;
            }
            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 10)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 11)),
                        { -16, -16, height }, { { -16, -16, height + 54 }, { 32, 32, 1 } });
                    break;
            }
            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 4)), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 5)), { 0, 0, height },
                        { { 0, 0, height + 54 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 7)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 12)), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 13)), { 0, 0, height },
                        { { 0, 0, height + 54 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 15)), { 0, 0, height },
                        { { 0, 0, height + 8 }, { 32, 32, 1 } });
                    break;
            }
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackDiagUp60ToUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 0)), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 1)), { 0, 0, height },
                        { { 0, 0, height + 54 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 6)), { 0, 0, height },
                        { { 0, 0, height + 32 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 8)), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 9)), { 0, 0, height },
                        { { 0, 0, height + 54 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 14)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 54 }, { 32, 32, 1 } });
                    break;
            }
            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 10)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 11)),
                        { -16, -16, height }, { { -16, -16, height + 54 }, { 32, 32, 1 } });
                    break;
            }
            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 4)), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 5)), { 0, 0, height },
                        { { 0, 0, height + 54 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 7)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 12)), { 0, 0, height },
                        { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 13)), { 0, 0, height },
                        { { 0, 0, height + 54 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 15)), { 0, 0, height },
                        { { 0, 0, height + 32 }, { 32, 32, 1 } });
                    break;
            }
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 72);
}

static void TrackDiagUp60(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 0)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 1)),
                        { 0, 0, height }, { { 0, 0, height + 86 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 6)),
                        { 0, 0, height }, { { 0, 0, height + 1 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 8)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 9)),
                        { 0, 0, height }, { { 0, 0, height + 86 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 14)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 86 }, { 32, 32, 1 } });
                    break;
            }
            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height + 16, session.SupportColours);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 10)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 11)),
                        { -16, -16, height }, { { -16, -16, height + 86 }, { 32, 32, 1 } });
                    break;
            }
            WoodenBSupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height + 16, session.SupportColours);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 4)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 5)),
                        { 0, 0, height }, { { 0, 0, height + 86 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 7)),
                        { -16, -16, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 12)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 13)),
                        { 0, 0, height }, { { 0, 0, height + 86 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 15)),
                        { 0, 0, height }, { { 0, 0, height + 8 }, { 32, 32, 1 } });
                    break;
            }
            break;
    }

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 104);
}

static void TrackDiagDown25ToDown60(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return TrackDiagUp60ToUp25(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDiagDown60ToDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return TrackDiagUp25ToUp60(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackDiagDown60(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return TrackDiagUp60(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackLeftQuarterTurn3TilesUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 0)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 1)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 8)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 9)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 14)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 15)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 20)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 21)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 2)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 3)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 4)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 5)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 10)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 11)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 16, 16, 0 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 16)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 17)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 16, 16, 0 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 22)),
                        { 0, 0, height }, { { 16, 16, height }, { 32, 32, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 23)),
                        { 0, 0, height }, { { 16, 16, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 6)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 7)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 12)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 13)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 18)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 19)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 24)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 25)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
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
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void TrackRightQuarterTurn3TilesUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 26)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 27)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 32)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 33)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 38)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 39)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 44)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 45)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 46)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 47)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 28)),
                        { 0, 0, height }, { { 16, 16, height }, { 32, 32, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 29)),
                        { 0, 0, height }, { { 16, 16, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 34)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 35)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 40)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 41)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 48)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 49)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 56);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 30)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 31)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 36)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 37)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 42)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 43)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 50)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 51)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
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
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void TrackLeftQuarterTurn3TilesDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    TrackRightQuarterTurn3TilesUp25(session, ride, trackSequence, DirectionNext(direction), height, trackElement, supportType);
}

static void TrackRightQuarterTurn3TilesDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    TrackLeftQuarterTurn3TilesUp25(session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

static void TrackLeftQuarterTurn5TilesUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 0)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 1)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetupRotated(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 10)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 11)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetupRotated(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 20)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 21)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetupRotated(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 30)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 31)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetupRotated(
                        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                        WoodenSupportTransitionType::Up25Deg);
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 2)),
                        { 0, 0, height }, { 32, 16, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 3)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 12)),
                        { 0, 0, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 13)),
                        { 0, 0, height }, { { -16, -16, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 22)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 23)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 32)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 33)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 4)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 5)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height - 16, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 14)),
                        { 0, 0, height }, { { 16, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 15)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height - 16, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 24)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 25)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height - 16, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 34)),
                        { 0, 0, height }, { 16, 16, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 35)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height - 16, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre,
                        PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::rightCorner, PaintSegment::topRightSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 6)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 7)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 16)),
                        { 0, 0, height }, { { -16, -16, height + 8 }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 17)),
                        { 0, 0, height }, { { -16, -16, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 26)),
                        { 0, 0, height }, { 16, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 27)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 36)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 37)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 8)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 9)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 18)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 19)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 28)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 29)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 38)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 39)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, DirectionPrev(direction), height,
                session.SupportColours);
            switch (direction)
            {
                case 2:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 3:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void TrackRightQuarterTurn5TilesUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 40)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 41)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 50)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 51)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 60)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 61)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 70)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 71)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25Deg);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 42)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 43)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 52)),
                        { 0, 0, height }, { 32, 16, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 53)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 62)),
                        { 0, 0, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 63)),
                        { 0, 0, height }, { { -16, -16, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 72)),
                        { 0, 0, height }, { 32, 16, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 73)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 44)),
                        { 0, 0, height }, { 16, 16, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 45)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height - 16, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 54)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 55)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height - 16, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 64)),
                        { 0, 0, height }, { { 16, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 65)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height - 16, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 74)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 75)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 16, 16, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height - 16, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::rightCorner, PaintSegment::centre,
                        PaintSegment::topLeftSide, PaintSegment::topRightSide, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 64);
            break;
        case 4:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::topCorner, PaintSegment::topLeftSide, PaintSegment::topRightSide), direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 46)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 47)),
                        { 0, 0, height }, { { 16, 32, height + 38 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner2, height, session.SupportColours);
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 56)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 57)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner3, height, session.SupportColours);
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 66)),
                        { 0, 0, height }, { { -16, -16, height + 8 }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 67)),
                        { 0, 0, height }, { { -16, -16, height + 38 }, { 32, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner0, height, session.SupportColours);
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 76)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 77)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 16, 32, 1 } });
                    WoodenASupportsPaintSetup(
                        session, supportType.wooden, WoodenSupportSubType::Corner1, height, session.SupportColours);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 6:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 48)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 49)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 58)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 59)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 68)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 69)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 78)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 79)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, DirectionNext(direction), height,
                session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
                case 1:
                    PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void TrackLeftQuarterTurn5TilesDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    TrackRightQuarterTurn5TilesUp25(session, ride, trackSequence, DirectionNext(direction), height, trackElement, supportType);
}

static void TrackRightQuarterTurn5TilesDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    TrackLeftQuarterTurn5TilesUp25(session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

static void TrackLeftEighthToDiagUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 0)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 1)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 8)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 9)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 16)),
                        { 0, 0, height }, { { 0, 0, height }, { 64, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 17)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 64, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 24)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 25)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25Deg);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 2)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 3)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 10)),
                        { 0, 0, height }, { { 0, 0, height }, { 34, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 11)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 34, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 18)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 19)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 26)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 27)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 32, 16, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 4)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 5)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 12)),
                        { 0, 0, height }, { { 16, 16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 13)),
                        { 0, 0, height }, { { 16, 16, height + 38 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 20)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 21)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 28)),
                        { 0, 0, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 29)),
                        { 0, 0, height }, { { -16, -16, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner1, direction, height - 16, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner3, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 6)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 7)),
                        { 0, 0, height }, { { 16, 16, height + 38 }, { 16, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 14)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 15)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 22)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 23)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 16, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 30)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 31)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void TrackRightEighthToDiagUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 32)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 33)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 40)),
                        { 0, 0, height }, { { 0, 0, height }, { 64, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 41)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 64, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 48)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 49)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 56)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 57)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours,
                WoodenSupportTransitionType::Up25Deg);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 34)),
                        { 0, 0, height }, { { 0, 16, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 35)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 32, 16, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 42)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 43)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 50)),
                        { 0, 0, height }, { { 0, 0, height }, { 34, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 51)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 34, 16, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 58)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 59)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 16, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 36)),
                        { 0, 0, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 37)),
                        { 0, 0, height }, { { -16, -16, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 44)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 45)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 52)),
                        { 0, 0, height }, { { 16, 16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 53)),
                        { 0, 0, height }, { { 16, 16, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 60)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 61)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 16, 16, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height - 16, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 38)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 39)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 46)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 47)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 54)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 55)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 62)),
                        { 0, 0, height }, { { 16, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 63)),
                        { 0, 0, height }, { { 16, 16, height + 38 }, { 16, 16, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner3, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void TrackLeftEighthToOrthogonalUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 64)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 65)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 72)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 73)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 80)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 81)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 88)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 89)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 16, 16, 0 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 66)),
                        { 0, 0, height }, { { 16, 16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 67)),
                        { 0, 0, height }, { { 16, 16, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 74)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 75)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 82)),
                        { 0, 0, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 83)),
                        { 0, 0, height }, { { -16, -16, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 90)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 91)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 16, 16, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 68)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 69)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 76)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 77)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 84)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 85)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 92)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 93)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 70)),
                        { 0, 0, height }, { { 0, 0, height }, { 48, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 71)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 78)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 79)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 86)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 87)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 94)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 95)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

            if (direction == 1 || direction == 2)
            {
                PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            }

            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void TrackRightEighthToOrthogonalUp25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 96)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 97)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 104)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 105)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 112)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 113)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 120)),
                        { 0, 0, height }, { { 0, 0, height }, { 16, 16, 0 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 121)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 16, 16, 1 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 98)),
                        { 0, 0, height }, { { -16, -16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 99)),
                        { 0, 0, height }, { { -16, -16, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 106)),
                        { 0, 0, height }, { { 16, 0, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 107)),
                        { 0, 0, height }, { { 16, 0, height + 38 }, { 16, 16, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 114)),
                        { 0, 0, height }, { { 16, 16, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 115)),
                        { 0, 0, height }, { { 16, 16, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 122)),
                        { 0, 0, height }, { { 0, 16, height }, { 16, 16, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 123)),
                        { 0, 0, height }, { { 0, 16, height + 38 }, { 16, 16, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner0, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 2:
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::Corner2, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 100)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 101)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 108)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 109)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 116)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 117)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 124)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 125)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NwSe, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 102)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 103)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 110)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 111)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 118)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 48, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 119)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 126)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 127)),
                        { 0, 0, height }, { { 0, 0, height + 38 }, { 32, 32, 1 } });
                    break;
            }
            WoodenASupportsPaintSetupRotated(
                session, supportType.wooden, WoodenSupportSubType::NwSe, DirectionNext(direction), height,
                session.SupportColours);
            if (direction == 0 || direction == 1)
            {
                PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 72);
            break;
    }
}

static void TrackLeftEighthToDiagDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t map[5] = { 4, 3, 1, 2, 0 };
    trackSequence = map[trackSequence];
    TrackRightEighthToOrthogonalUp25(session, ride, trackSequence, DirectionNext(direction), height, trackElement, supportType);
}

static void TrackRightEighthToDiagDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    uint8_t map[5] = { 4, 3, 1, 2, 0 };
    trackSequence = map[trackSequence];
    TrackLeftEighthToOrthogonalUp25(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackLeftEighthToOrthogonalDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    TrackRightEighthToDiagUp25(session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void TrackRightEighthToOrthogonalDown25(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    TrackLeftEighthToDiagUp25(session, ride, trackSequence, DirectionPrev(direction), height, trackElement, supportType);
}

static void TrackSBendLeft(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 0)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 1)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 8)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 9)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 6)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 7)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 15)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 16)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 2)),
                        { 0, 0, height }, { 32, 26, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 3)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 26, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 10)),
                        { 0, 0, height }, { 32, 26, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 11)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 26, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 4)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 5)),
                        { 0, 0, height }, { { 0, 6, height + 23 }, { 32, 26, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 12)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 13)),
                        { 0, 0, height }, { { 0, 6, height + 23 }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 14)),
                        { 32, 32, height }, { { 32, 32, height + 8 }, { 1, 1, 1 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 4)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 5)),
                        { 0, 0, height }, { { 0, 6, height + 23 }, { 32, 26, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 12)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 13)),
                        { 0, 0, height }, { { 0, 6, height + 23 }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 14)),
                        { 32, 32, height }, { { 32, 32, height + 23 }, { 1, 1, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 2)),
                        { 0, 0, height }, { 32, 26, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 3)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 26, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 10)),
                        { 0, 0, height }, { 32, 26, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 11)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 26, 1 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 6)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 7)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 15)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 16)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 0)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 1)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 8)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 9)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
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
            break;
    }

    static constexpr int blockedSegments[4] = {
        kSegmentsAll,
        EnumsToFlags(
            PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
            PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
        EnumsToFlags(
            PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
            PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
        kSegmentsAll,
    };

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::SBendLeft>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void TrackSBendRight(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 17)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 18)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 26)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 27)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 24)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 25)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 32)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 33)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
            }
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 19)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 20)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 21)),
                        { 32, 32, height }, { { 32, 32, height + 8 }, { 1, 1, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 28)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 29)),
                        { 0, 0, height }, { { 0, 6, height + 23 }, { 32, 26, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 22)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 23)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 30)),
                        { 0, 0, height }, { 32, 26, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 31)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 26, 1 } });
                    break;
            }
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 22)),
                        { 0, 0, height }, { 32, 32, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 23)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 30)),
                        { 0, 0, height }, { 32, 26, 1 });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 31)),
                        { 0, 0, height }, { 32, 26, 1 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 19)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 20)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 21)),
                        { 32, 32, height }, { { 32, 32, height + 23 }, { 1, 1, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 28)),
                        { 0, 0, height }, { { 0, 6, height }, { 32, 26, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 29)),
                        { 0, 0, height }, { { 0, 6, height + 23 }, { 32, 26, 1 } });
                    break;
            }
            break;
        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 24)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 25)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 32)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 33)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 17)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 18)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 26)),
                        { 0, 0, height }, { { 0, 0, height }, { 32, 32, 1 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_G2_GO_KARTS_TRACK_S_BEND + 27)),
                        { 0, 0, height }, { { 0, 0, height + 23 }, { 32, 32, 1 } });
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
            break;
    }

    static constexpr int blockedSegments[4] = {
        kSegmentsAll,
        EnumsToFlags(
            PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
            PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
        EnumsToFlags(
            PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
            PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
        kSegmentsAll,
    };

    DrawSupportForSequenceA<OpenRCT2::TrackElemType::SBendRight>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(blockedSegments[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/**
 * rct2: 0x0074A668
 */
TrackPaintFunction GetTrackPaintFunctionGoKarts(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintGoKartsTrackFlat;

        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintGoKartsStation;

        case TrackElemType::Up25:
            return PaintGoKartsTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintGoKartsTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintGoKartsTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintGoKartsTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintGoKartsTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintGoKartsTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintGoKartsTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintGoKartsTrackRightQuarterTurn1Tile;

        // Added by OpenRCT2

        // Steep
        case TrackElemType::Up60:
            return TrackUp60;
        case TrackElemType::Up25ToUp60:
            return TrackUp25ToUp60;
        case TrackElemType::Up60ToUp25:
            return TrackUp60ToUp25;
        case TrackElemType::Down60:
            return TrackDown60;
        case TrackElemType::Down25ToDown60:
            return TrackDown25ToDown60;
        case TrackElemType::Down60ToDown25:
            return TrackDown60ToDown25;

        // Flat to steep
        case TrackElemType::FlatToUp60LongBase:
            return TrackFlatToUp60LongBase;
        case TrackElemType::Up60ToFlatLongBase:
            return TrackUp60ToFlatLongBase;
        case TrackElemType::FlatToDown60LongBase:
            return TrackFlatToDown60LongBase;
        case TrackElemType::Down60ToFlatLongBase:
            return TrackDown60ToFlatLongBase;

        // Small turns
        case TrackElemType::LeftQuarterTurn3Tiles:
            return TrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return TrackRightQuarterTurn3Tiles;

        // Medium turns
        case TrackElemType::LeftQuarterTurn5Tiles:
            return TrackLeftQuarterTurn5Tiles;
        case TrackElemType::RightQuarterTurn5Tiles:
            return TrackRightQuarterTurn5Tiles;

        // Large turns
        case TrackElemType::LeftEighthToDiag:
            return TrackLeftEighthToDiag;
        case TrackElemType::RightEighthToDiag:
            return TrackRightEighthToDiag;
        case TrackElemType::LeftEighthToOrthogonal:
            return TrackLeftEighthToOrthogonal;
        case TrackElemType::RightEighthToOrthogonal:
            return TrackRightEighthToOrthogonal;

        // Diagonal
        case TrackElemType::DiagFlat:
            return TrackDiagFlat;
        case TrackElemType::DiagFlatToUp25:
            return TrackDiagFlatToUp25;
        case TrackElemType::DiagUp25ToFlat:
            return TrackDiagUp25ToFlat;
        case TrackElemType::DiagFlatToDown25:
            return TrackDiagFlatToDown25;
        case TrackElemType::DiagDown25ToFlat:
            return TrackDiagDown25ToFlat;
        case TrackElemType::DiagUp25:
            return TrackDiagUp25;
        case TrackElemType::DiagDown25:
            return TrackDiagDown25;
        case TrackElemType::DiagUp25ToUp60:
            return TrackDiagUp25ToUp60;
        case TrackElemType::DiagUp60ToUp25:
            return TrackDiagUp60ToUp25;
        case TrackElemType::DiagUp60:
            return TrackDiagUp60;
        case TrackElemType::DiagDown25ToDown60:
            return TrackDiagDown25ToDown60;
        case TrackElemType::DiagDown60ToDown25:
            return TrackDiagDown60ToDown25;
        case TrackElemType::DiagDown60:
            return TrackDiagDown60;

        // Small gentle turns
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return TrackLeftQuarterTurn3TilesUp25;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return TrackRightQuarterTurn3TilesUp25;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return TrackLeftQuarterTurn3TilesDown25;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return TrackRightQuarterTurn3TilesDown25;

        // Medium gentle turns
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return TrackLeftQuarterTurn5TilesUp25;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return TrackRightQuarterTurn5TilesUp25;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return TrackLeftQuarterTurn5TilesDown25;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return TrackRightQuarterTurn5TilesDown25;

        // Large gentle turns
        case TrackElemType::LeftEighthToDiagUp25:
            return TrackLeftEighthToDiagUp25;
        case TrackElemType::RightEighthToDiagUp25:
            return TrackRightEighthToDiagUp25;
        case TrackElemType::LeftEighthToDiagDown25:
            return TrackLeftEighthToDiagDown25;
        case TrackElemType::RightEighthToDiagDown25:
            return TrackRightEighthToDiagDown25;
        case TrackElemType::LeftEighthToOrthogonalUp25:
            return TrackLeftEighthToOrthogonalUp25;
        case TrackElemType::RightEighthToOrthogonalUp25:
            return TrackRightEighthToOrthogonalUp25;
        case TrackElemType::LeftEighthToOrthogonalDown25:
            return TrackLeftEighthToOrthogonalDown25;
        case TrackElemType::RightEighthToOrthogonalDown25:
            return TrackRightEighthToOrthogonalDown25;

        // S bends
        case TrackElemType::SBendLeft:
            return TrackSBendLeft;
        case TrackElemType::SBendRight:
            return TrackSBendRight;

        default:
            return TrackPaintFunctionDummy;
    }
}
