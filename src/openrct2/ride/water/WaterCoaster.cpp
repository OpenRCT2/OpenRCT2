/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../common.h"
#include "../Ride.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../coaster/JuniorRollerCoaster.h"

static void water_rc_track_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRcPaintTrackFlat(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRcChainType::ChainLift : JuniorRcChainType::None);
}

static void water_rc_track_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRcPaintTrack25DegUp(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRcChainType::ChainLift : JuniorRcChainType::None);
}

static void water_rc_track_flat_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRcPaintTrackFlatTo25DegUp(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRcChainType::ChainLift : JuniorRcChainType::None);
}

static void water_rc_track_25_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRcPaintTrack25DegUpToFlat(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRcChainType::ChainLift : JuniorRcChainType::None);
}

static void water_rc_track_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    water_rc_track_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void water_rc_track_flat_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    water_rc_track_25_deg_up_to_flat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void water_rc_track_25_deg_down_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    water_rc_track_flat_to_25_deg_up(session, ride, trackSequence, (direction + 2) & 3, height, trackElement);
}

static void water_rc_track_diag_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRcPaintTrackDiagFlat(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRcChainType::ChainLift : JuniorRcChainType::None);
}

static void water_rc_track_diag_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRcPaintTrackDiag25DegUp(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRcChainType::ChainLift : JuniorRcChainType::None);
}

static void water_rc_track_diag_flat_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRcPaintTrackDiagFlatTo25DegUp(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRcChainType::ChainLift : JuniorRcChainType::None);
}

static void water_rc_track_diag_25_deg_up_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRcPaintTrackDiag25DegUpToFlat(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRcChainType::ChainLift : JuniorRcChainType::None);
}

static void water_rc_track_diag_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRcPaintTrackDiag25DegDown(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRcChainType::ChainLift : JuniorRcChainType::None);
}

static void water_rc_track_diag_flat_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRcPaintTrackDiagFlatTo25DegDown(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRcChainType::ChainLift : JuniorRcChainType::None);
}

static void water_rc_track_diag_25_deg_down_to_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    bool isChained = trackElement.HasChain();
    JuniorRcPaintTrackDiag25DegDownToFlat(
        session, ride, trackSequence, direction, height, trackElement,
        isChained ? JuniorRcChainType::ChainLift : JuniorRcChainType::None);
}

static void water_rc_track_station(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRcPaintStation(session, ride, trackSequence, direction, height, trackElement, false);
}

static void water_rc_track_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRcPaintTrack60DegUp(session, ride, trackSequence, direction, height, trackElement, JuniorRcChainType::None);
}

static void water_rc_track_25_deg_up_to_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRcPaintTrack25DegUpTo60DegUp(
        session, ride, trackSequence, direction, height, trackElement, JuniorRcChainType::None);
}

static void water_rc_track_60_deg_up_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRcPaintTrack60DegUpTo25DegUp(
        session, ride, trackSequence, direction, height, trackElement, JuniorRcChainType::None);
}

static void water_rc_track_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    water_rc_track_60_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

static void water_rc_track_25_deg_down_to_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    water_rc_track_60_deg_up_to_25_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

static void water_rc_track_60_deg_down_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    water_rc_track_25_deg_up_to_60_deg_up(session, ride, trackSequence, (direction + 2) % 4, height, trackElement);
}

// 5 tile turns

static void water_rc_track_left_quarter_turn_5_tiles_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRcPaintTrackLeftQuarterTurn5Tiles25DegUp(
        session, ride, trackSequence, direction, height, trackElement, JuniorRcChainType::None);
}

static void water_rc_track_right_quarter_turn_5_tiles_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRcPaintTrackRightQuarterTurn5Tiles25DegUp(
        session, ride, trackSequence, direction, height, trackElement, JuniorRcChainType::None);
}

static constexpr const uint8_t water_rc_left_quarter_turn_5_tiles_to_right_turn_map[] = {
    6, 4, 5, 3, 1, 2, 0,
};

static void water_rc_track_left_quarter_turn_5_tiles_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    water_rc_track_right_quarter_turn_5_tiles_25_deg_up(
        session, ride, water_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence], (direction + 1) % 4, height,
        trackElement);
}

static void water_rc_track_right_quarter_turn_5_tiles_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    water_rc_track_left_quarter_turn_5_tiles_25_deg_up(
        session, ride, water_rc_left_quarter_turn_5_tiles_to_right_turn_map[trackSequence], (direction + 3) % 4, height,
        trackElement);
}

// 3 tile turns

static void water_rc_track_right_quarter_turn_3_tiles_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    juniorRcPaintTrackRightQuarterTurn3Tiles25DegUp(
        session, ride, trackSequence, direction, height, trackElement, JuniorRcChainType::None);
}

static void water_rc_track_right_quarter_turn_3_tiles_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRcPaintTrackRightQuarterTurn3Tiles25DegDown(
        session, ride, trackSequence, direction, height, trackElement, JuniorRcChainType::None);
}

static constexpr const uint8_t water_rc_left_quarter_turn_3_tiles_to_right_turn_map[] = {
    3,
    1,
    2,
    0,
};

static void water_rc_track_left_quarter_turn_3_tiles_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = water_rc_left_quarter_turn_3_tiles_to_right_turn_map[trackSequence];
    water_rc_track_right_quarter_turn_3_tiles_25_deg_down(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

static void water_rc_track_left_quarter_turn_3_tiles_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = water_rc_left_quarter_turn_3_tiles_to_right_turn_map[trackSequence];
    water_rc_track_right_quarter_turn_3_tiles_25_deg_up(
        session, ride, trackSequence, (direction + 1) % 4, height, trackElement);
}

static void water_rc_track_diag_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRcPaintTrackDiag60DegUp(
        session, ride, trackSequence, direction, height, trackElement, JuniorRcChainType::None);
}

static void water_rc_track_diag_25_deg_up_to_60_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRcPaintTrackDiag25DegUpTo60DegUp(
        session, ride, trackSequence, direction, height, trackElement, JuniorRcChainType::None);
}

static void water_rc_track_diag_60_deg_up_to_25_deg_up(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRcPaintTrackDiag60DegUpTo25DegUp(
        session, ride, trackSequence, direction, height, trackElement, JuniorRcChainType::None);
}

static void water_rc_track_diag_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRcPaintTrackDiag60DegDown(
        session, ride, trackSequence, direction, height, trackElement, JuniorRcChainType::None);
}

static void water_rc_track_diag_25_deg_down_to_60_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRcPaintTrackDiag25DegDownTo60DegDown(
        session, ride, trackSequence, direction, height, trackElement, JuniorRcChainType::None);
}

static void water_rc_track_diag_60_deg_down_to_25_deg_down(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    JuniorRcPaintTrackDiag60DegDownTo25DegDown(
        session, ride, trackSequence, direction, height, trackElement, JuniorRcChainType::None);
}

TRACK_PAINT_FUNCTION get_track_paint_function_water_rc(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return water_rc_track_flat;
        case TrackElemType::Up25:
            return water_rc_track_25_deg_up;
        case TrackElemType::FlatToUp25:
            return water_rc_track_flat_to_25_deg_up;
        case TrackElemType::Up25ToFlat:
            return water_rc_track_25_deg_up_to_flat;
        case TrackElemType::Down25:
            return water_rc_track_25_deg_down;
        case TrackElemType::FlatToDown25:
            return water_rc_track_flat_to_25_deg_down;
        case TrackElemType::Down25ToFlat:
            return water_rc_track_25_deg_down_to_flat;
        case TrackElemType::DiagFlat:
            return water_rc_track_diag_flat;
        case TrackElemType::DiagUp25:
            return water_rc_track_diag_25_deg_up;
        case TrackElemType::DiagFlatToUp25:
            return water_rc_track_diag_flat_to_25_deg_up;
        case TrackElemType::DiagUp25ToFlat:
            return water_rc_track_diag_25_deg_up_to_flat;
        case TrackElemType::DiagDown25:
            return water_rc_track_diag_25_deg_down;
        case TrackElemType::DiagFlatToDown25:
            return water_rc_track_diag_flat_to_25_deg_down;
        case TrackElemType::DiagDown25ToFlat:
            return water_rc_track_diag_25_deg_down_to_flat;

        // Use Junior RC without lift hill
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return water_rc_track_station;
        case TrackElemType::Up60:
            return water_rc_track_60_deg_up;
        case TrackElemType::Up25ToUp60:
            return water_rc_track_25_deg_up_to_60_deg_up;
        case TrackElemType::Up60ToUp25:
            return water_rc_track_60_deg_up_to_25_deg_up;
        case TrackElemType::Down60:
            return water_rc_track_60_deg_down;
        case TrackElemType::Down25ToDown60:
            return water_rc_track_25_deg_down_to_60_deg_down;
        case TrackElemType::Down60ToDown25:
            return water_rc_track_60_deg_down_to_25_deg_down;
        case TrackElemType::LeftQuarterTurn5TilesUp25:
            return water_rc_track_left_quarter_turn_5_tiles_25_deg_up;
        case TrackElemType::RightQuarterTurn5TilesUp25:
            return water_rc_track_right_quarter_turn_5_tiles_25_deg_up;
        case TrackElemType::LeftQuarterTurn5TilesDown25:
            return water_rc_track_left_quarter_turn_5_tiles_25_deg_down;
        case TrackElemType::RightQuarterTurn5TilesDown25:
            return water_rc_track_right_quarter_turn_5_tiles_25_deg_down;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
            return water_rc_track_left_quarter_turn_3_tiles_25_deg_up;
        case TrackElemType::RightQuarterTurn3TilesUp25:
            return water_rc_track_right_quarter_turn_3_tiles_25_deg_up;
        case TrackElemType::LeftQuarterTurn3TilesDown25:
            return water_rc_track_left_quarter_turn_3_tiles_25_deg_down;
        case TrackElemType::RightQuarterTurn3TilesDown25:
            return water_rc_track_right_quarter_turn_3_tiles_25_deg_down;
        case TrackElemType::DiagUp60:
            return water_rc_track_diag_60_deg_up;
        case TrackElemType::DiagUp25ToUp60:
            return water_rc_track_diag_25_deg_up_to_60_deg_up;
        case TrackElemType::DiagUp60ToUp25:
            return water_rc_track_diag_60_deg_up_to_25_deg_up;
        case TrackElemType::DiagDown60:
            return water_rc_track_diag_60_deg_down;
        case TrackElemType::DiagDown25ToDown60:
            return water_rc_track_diag_25_deg_down_to_60_deg_down;
        case TrackElemType::DiagDown60ToDown25:
            return water_rc_track_diag_60_deg_down_to_25_deg_down;

        //////////////////////////////////////////////////////
        // Use Junior RC
        //////////////////////////////////////////////////////
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
        case TrackElemType::SBendLeft:
        case TrackElemType::SBendRight:
        case TrackElemType::LeftQuarterTurn3Tiles:
        case TrackElemType::RightQuarterTurn3Tiles:
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
        case TrackElemType::RightBankedQuarterTurn3Tiles:
        case TrackElemType::FlatToUp60:
        case TrackElemType::Up60ToFlat:
        case TrackElemType::FlatToDown60:
        case TrackElemType::Down60ToFlat:
        case TrackElemType::LeftHalfBankedHelixUpSmall:
        case TrackElemType::RightHalfBankedHelixUpSmall:
        case TrackElemType::LeftHalfBankedHelixDownSmall:
        case TrackElemType::RightHalfBankedHelixDownSmall:
        case TrackElemType::LeftHalfBankedHelixUpLarge:
        case TrackElemType::RightHalfBankedHelixUpLarge:
        case TrackElemType::LeftHalfBankedHelixDownLarge:
        case TrackElemType::RightHalfBankedHelixDownLarge:
        case TrackElemType::Brakes:
        case TrackElemType::LeftEighthToDiag:
        case TrackElemType::RightEighthToDiag:
        case TrackElemType::LeftEighthToOrthogonal:
        case TrackElemType::RightEighthToOrthogonal:
        case TrackElemType::LeftEighthBankToDiag:
        case TrackElemType::RightEighthBankToDiag:
        case TrackElemType::LeftEighthBankToOrthogonal:
        case TrackElemType::RightEighthBankToOrthogonal:
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
        case TrackElemType::BlockBrakes:
        case TrackElemType::Booster:
            return get_track_paint_function_junior_rc(trackType);

        //////////////////////////////////////////////////////
        // Use River Rafts
        //////////////////////////////////////////////////////
        case TrackElemType::FlatCovered:
            return get_track_paint_function_splash_boats(TrackElemType::Flat);
        case TrackElemType::LeftQuarterTurn5TilesCovered:
            return get_track_paint_function_splash_boats(TrackElemType::LeftQuarterTurn5Tiles);
        case TrackElemType::RightQuarterTurn5TilesCovered:
            return get_track_paint_function_splash_boats(TrackElemType::RightQuarterTurn5Tiles);
        case TrackElemType::SBendLeftCovered:
            return get_track_paint_function_splash_boats(TrackElemType::SBendLeft);
        case TrackElemType::SBendRightCovered:
            return get_track_paint_function_splash_boats(TrackElemType::SBendRight);
        case TrackElemType::FlatToDown25Covered:
            return get_track_paint_function_splash_boats(TrackElemType::FlatToDown25);
        case TrackElemType::Down25Covered:
            return get_track_paint_function_splash_boats(TrackElemType::Down25);
        case TrackElemType::Down25ToDown60Covered:
            return get_track_paint_function_splash_boats(TrackElemType::Down25ToDown60);
        case TrackElemType::Down60Covered:
            return get_track_paint_function_splash_boats(TrackElemType::Down60);
        case TrackElemType::Down60ToDown25Covered:
            return get_track_paint_function_splash_boats(TrackElemType::Down60ToDown25);
        case TrackElemType::Down25ToFlatCovered:
            return get_track_paint_function_splash_boats(TrackElemType::Down25ToFlat);

        case TrackElemType::OnRidePhoto:
            return get_track_paint_function_junior_rc(trackType);
    }
    return nullptr;
}
